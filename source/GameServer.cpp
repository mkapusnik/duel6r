#include <algorithm>
#include "Sound.h"
#include "TextureManager.h"
#include "Game.h"
#include "GameMode.h"
#include "GameServer.h"
#include "math/Math.h"

namespace Duel6 {
    GameServer::GameServer(Game &gameHost, AppService &appService, GameResources &resources, GameSettings &settings)
            : gameHost(gameHost), appService(appService), resources(resources), settings(settings), gameMode(nullptr),
              currentRound(0), playedRounds(0), roundClosed(true) {}

    void GameServer::start(const std::vector<GamePlayerDefinition> &playerDefinitions,
                           const std::vector<std::string> &levels,
                           const std::vector<Size> &backgrounds,
                           GameMode &gameMode) {
        endRound();
        round.reset();

        Console &console = appService.getConsole();
        console.printLine("\n=== Starting new game ===");
        console.printLine(Format("...Rounds: {0}") << settings.getMaxRounds());

        TextureManager &textureManager = appService.getTextureManager();
        players.clear();
        for (auto &skin : skins) {
            textureManager.dispose(skin.getTexture());
        }
        skins.clear();

        players.reserve(playerDefinitions.size());
        playerAnimations = std::make_unique<PlayerAnimations>(resources.getPlayerAnimation());
        for (const GamePlayerDefinition &playerDef : playerDefinitions) {
            console.printLine(Format("...Generating player for person: {0}") << playerDef.getPerson().getName());
            skins.push_back(PlayerSkin(playerDef.getColors(), textureManager, *playerAnimations));
            players.emplace_back(playerDef.getPerson(), skins.back(), playerDef.getSounds());
        }

        this->levels = levels;
        std::shuffle(this->levels.begin(), this->levels.end(), Math::randomEngine);
        this->backgrounds = backgrounds;
        this->gameMode = &gameMode;
        gameMode.initializeGame(gameHost, players, settings.isQuickLiquid(), settings.isGlobalAssistances());
        startRound();
    }

    void GameServer::setPlayerInputs(const std::vector<Uint32> &controllerStates) {
        for (Size index = 0; index < players.size(); index++) {
            Uint32 controllerState = index < controllerStates.size() ? controllerStates[index] : 0;
            players[index].setControllerState(controllerState);
        }
    }

    void GameServer::update(Float32 elapsedTime) {
        if (round != nullptr && !round->isOver()) {
            round->update(elapsedTime);
        }
    }

    void GameServer::startNextRound() {
        endRound();
        startRound();
    }

    void GameServer::endRound() {
        if (round != nullptr && !roundClosed) {
            round->end();
            roundClosed = true;
        }
    }

    void GameServer::startRound() {
        currentRound = playedRounds;

        bool shuffle = settings.getLevelSelectionMode() == LevelSelectionMode::Shuffle;
        Int32 level = shuffle ? playedRounds % Int32(levels.size()) : Math::random(Int32(levels.size()));
        const std::string levelPath = levels[level];
        bool mirror = Math::random(2) == 0;

        Console &console = appService.getConsole();
        console.printLine(Format("\n===Loading level {0}===") << levelPath);
        console.printLine(Format("...Parameters: mirror: {0}") << mirror);

        round = std::make_unique<Round>(gameHost, playedRounds, levelPath, mirror);
        roundClosed = false;
        round->setOnRoundEnd([this]() {
            onRoundEnd();
        });
        round->start();
    }

    void GameServer::onRoundEnd() {
        playedRounds++;
        if (round->isLast()) {
            getMode().updateElo(players);
        }
        if (onRoundEndCallback) {
            onRoundEndCallback();
        }
    }
}
