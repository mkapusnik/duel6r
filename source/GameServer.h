#ifndef DUEL6_GAMESERVER_H
#define DUEL6_GAMESERVER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "AppService.h"
#include "GamePlayerDefinition.h"
#include "GameResources.h"
#include "GameSettings.h"
#include "Player.h"
#include "Round.h"

namespace Duel6 {
    class Game;
    class GameMode;

    class GameServer {
    private:
        Game &gameHost;
        AppService &appService;
        GameResources &resources;
        GameSettings &settings;
        GameMode *gameMode;
        std::unique_ptr<Round> round;
        std::vector<std::string> levels;
        std::vector<Size> backgrounds;
        Int32 currentRound;
        Int32 playedRounds;
        std::vector<Player> players;
        std::vector<PlayerSkin> skins;
        std::unique_ptr<PlayerAnimations> playerAnimations;
        std::function<void()> onRoundEndCallback;
        bool roundClosed;

    public:
        GameServer(Game &gameHost, AppService &appService, GameResources &resources, GameSettings &settings);

        void start(const std::vector<GamePlayerDefinition> &playerDefinitions,
                   const std::vector<std::string> &levels,
                   const std::vector<Size> &backgrounds,
                   GameMode &gameMode);

        void setPlayerInputs(const std::vector<Uint32> &controllerStates);

        void update(Float32 elapsedTime);

        void startNextRound();

        void endRound();

        bool hasRound() const {
            return round != nullptr;
        }

        std::vector<Player> &getPlayers() {
            return players;
        }

        const std::vector<Player> &getPlayers() const {
            return players;
        }

        Round &getRound() {
            return *round;
        }

        const Round &getRound() const {
            return *round;
        }

        GameMode &getMode() {
            return *gameMode;
        }

        const GameMode &getMode() const {
            return *gameMode;
        }

        const std::vector<Size> &getBackgrounds() const {
            return backgrounds;
        }

        Int32 getCurrentRound() const {
            return currentRound;
        }

        Int32 getPlayedRounds() const {
            return playedRounds;
        }

        void setPlayedRounds(Int32 playedRounds) {
            this->playedRounds = playedRounds;
        }

        bool isOver() const {
            return hasRound() && getRound().isLast() && getRound().isOver();
        }

        void setOnRoundEnd(std::function<void()> callback) {
            onRoundEndCallback = callback;
        }

    private:
        void startRound();

        void onRoundEnd();
    };
}

#endif
