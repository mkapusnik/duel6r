/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Game.h"
#include "LocalGameServerTransport.h"
#include "Menu.h"
#include "GameMode.h"

namespace Duel6 {
    namespace {
        Uint32 readControllerState(const PlayerControls &controls) {
            Uint32 controllerState = 0;
            if (controls.getLeft().isPressed()) {
                controllerState |= Player::ButtonLeft;
            }
            if (controls.getRight().isPressed()) {
                controllerState |= Player::ButtonRight;
            }
            if (controls.getUp().isPressed()) {
                controllerState |= Player::ButtonUp;
            }
            if (controls.getDown().isPressed()) {
                controllerState |= Player::ButtonDown;
            }
            if (controls.getShoot().isPressed()) {
                controllerState |= Player::ButtonShoot;
            }
            if (controls.getPick().isPressed()) {
                controllerState |= Player::ButtonPick;
            }
            if (controls.getStatus().isPressed()) {
                controllerState |= Player::ButtonStatus;
            }
            return controllerState;
        }
    }

    Game::Game(AppService &appService, GameResources &resources, GameSettings &settings)
            : appService(appService), resources(resources), settings(settings),
              serverTransport(std::make_unique<LocalGameServerTransport>(*this, appService, resources, settings)),
              worldRenderer(appService, *this), menu(nullptr) {
        serverTransport->getServer().setOnRoundEnd([this]() {
            onRoundEnd();
        });
    }

    void Game::beforeStart(Context *prevContext) {
        SDL_ShowCursor(SDL_DISABLE);
    }

    void Game::beforeClose(Context *nextContext) {
        endRound();
    }

    void Game::render() const {
        if (!serverTransport->getServer().hasRound()) {
            return;
        }
        worldRenderer.render();
    }

    void Game::update(Float32 elapsedTime) {
        if (!serverTransport->getServer().hasRound()) {
            return;
        }

        if (getRound().isOver()) {
            if (!getRound().isLast()) {
                nextRound();
            }
        } else {
            serverTransport->submitPlayerInputs(collectPlayerInputs());
            serverTransport->update(elapsedTime);
            appService.getVideo().getRenderer().setGlobalTime(getRound().getWorld().getTime());
            updatePlayerCameras();
        }
    }

    void Game::keyEvent(const KeyPressEvent &event) {
        if (!serverTransport->getServer().hasRound()) {
            if (event.getCode() == SDLK_ESCAPE) {
                close();
            }
            return;
        }

        if (event.getCode() == SDLK_ESCAPE && (isOver() || event.withShift())) {
            close();
            return;
        }
        if (event.getCode() == SDLK_TAB && (!getRound().hasWinner())) {
            displayScoreTab = !displayScoreTab;
        }

        if (!getRound().isLast()) {
            if (event.getCode() == SDLK_F1 && (getRound().hasWinner() || event.withShift())) {
                nextRound();
                return;
            }
            if (getRound().hasWinner() && ((D6_GAME_OVER_WAIT - getRound().getRemainingGameOverWait()) > 3.0f)) {
                nextRound();
                return;
            }
        }

        if (event.getCode() == SDLK_F2 && getPlayers().size() < 5) {
            switchScreenMode();
        }

        if (event.getCode() == SDLK_F4) {
            settings.setShowRanking(!settings.isShowRanking());
        }

        if (event.getCode() == SDLK_F10) {
            Image image = appService.getVideo().getRenderer().makeScreenshot();
            std::string name = image.saveScreenshot();
            appService.getConsole().printLine(Format("Screenshot saved to {0}") << name);
        }
    }

    void Game::textInputEvent(const TextInputEvent &event) {}

    void Game::mouseButtonEvent(const MouseButtonEvent &event) {}

    void Game::mouseMotionEvent(const MouseMotionEvent &event) {}

    void Game::mouseWheelEvent(const MouseWheelEvent &event) {}

    void Game::joyDeviceAddedEvent(const JoyDeviceAddedEvent &event) {}

    void Game::joyDeviceRemovedEvent(const JoyDeviceRemovedEvent &event) {}

    void Game::start(const std::vector<GamePlayerDefinition> &playerDefinitions, const std::vector<std::string> &levels,
                     const std::vector<Size> &backgrounds, ScreenMode screenMode, Int32 screenZoom,
                     GameMode &gameMode) {
        localPlayerControls.clear();
        localPlayerControls.reserve(playerDefinitions.size());
        for (const GamePlayerDefinition &playerDef : playerDefinitions) {
            localPlayerControls.push_back(&playerDef.getControls());
        }

        settings.setScreenMode(screenMode);
        settings.setScreenZoom(screenZoom);
        displayScoreTab = false;
        serverTransport->start(playerDefinitions, levels, backgrounds, gameMode);
        startRound();
    }

    void Game::startRound() {
        prepareRoundClientView();
    }

    void Game::endRound() {
        serverTransport->endRound();
    }

    void Game::onRoundEnd() {
        if (menu != nullptr) {
            menu->savePersonData();
        }
    }

    void Game::nextRound() {
        displayScoreTab = false;
        serverTransport->startNextRound();
        startRound();
    }

    const std::vector<Size> &Game::getBackgrounds() const {
        return serverTransport->getServer().getBackgrounds();
    }

    std::vector<Player> &Game::getPlayers() {
        return serverTransport->getServer().getPlayers();
    }

    const std::vector<Player> &Game::getPlayers() const {
        return serverTransport->getServer().getPlayers();
    }

    Round &Game::getRound() {
        return serverTransport->getServer().getRound();
    }

    const Round &Game::getRound() const {
        return serverTransport->getServer().getRound();
    }

    Int32 Game::getPlayedRounds() const {
        return serverTransport->getServer().getPlayedRounds();
    }

    void Game::setPlayedRounds(Int32 playedRounds) {
        serverTransport->getServer().setPlayedRounds(playedRounds);
    }

    Int32 Game::getCurrentRound() const {
        return serverTransport->getServer().getCurrentRound();
    }

    bool Game::isOver() const {
        return serverTransport->getServer().isOver();
    }

    GameMode &Game::getMode() {
        return serverTransport->getServer().getMode();
    }

    const GameMode &Game::getMode() const {
        return serverTransport->getServer().getMode();
    }

    void Game::prepareRoundClientView() {
        if (!serverTransport->getServer().hasRound()) {
            return;
        }

        displayScoreTab = false;
        getRound().getWorld().getLevelRenderData().setScreenMode(settings.getScreenMode());
        getRound().getWorld().getLevelRenderData().generateFaces();
        updatePlayerViews();
        updatePlayerCameras();
        appService.getVideo().getRenderer().setGlobalTime(getRound().getWorld().getTime());
        worldRenderer.prerender();
    }

    void Game::splitScreenView(Player &player, Int32 x, Int32 y) const {
        const Video &video = appService.getVideo();
        PlayerView view(x, y, video.getScreen().getClientWidth() / 2 - 4, video.getScreen().getClientHeight() / 2 - 4);
        player.setView(view);
    }

    void Game::updatePlayerViews() {
        const Video &video = appService.getVideo();
        World &world = getRound().getWorld();
        std::vector<Player> &players = world.getPlayers();

        for (Player &player : players) {
            player.prepareCam(video, settings.getScreenMode(), settings.getScreenZoom(), world.getLevel().getWidth(),
                              world.getLevel().getHeight());
        }

        if (settings.getScreenMode() == ScreenMode::FullScreen) {
            for (Player &player : players) {
                player.setView(PlayerView(0, 0, video.getScreen().getClientWidth(), video.getScreen().getClientHeight()));
            }
            return;
        }

        if (players.size() == 2) {
            splitScreenView(players[0], video.getScreen().getClientWidth() / 4 + 2, 2);
            splitScreenView(players[1], video.getScreen().getClientWidth() / 4 + 2,
                            video.getScreen().getClientHeight() / 2 + 2);
        }

        if (players.size() == 3) {
            splitScreenView(players[0], 2, 2);
            splitScreenView(players[1], video.getScreen().getClientWidth() / 2 + 2, 2);
            splitScreenView(players[2], video.getScreen().getClientWidth() / 4 + 2,
                            video.getScreen().getClientHeight() / 2 + 2);
        }

        if (players.size() == 4) {
            splitScreenView(players[0], 2, 2);
            splitScreenView(players[1], video.getScreen().getClientWidth() / 2 + 2, 2);
            splitScreenView(players[2], 2, video.getScreen().getClientHeight() / 2 + 2);
            splitScreenView(players[3], video.getScreen().getClientWidth() / 2 + 2,
                            video.getScreen().getClientHeight() / 2 + 2);
        }
    }

    void Game::updatePlayerCameras() {
        if (settings.getScreenMode() != ScreenMode::SplitScreen || !serverTransport->getServer().hasRound()) {
            return;
        }

        const Level &level = getRound().getWorld().getLevel();
        for (Player &player : getPlayers()) {
            player.updateCamera(level.getWidth(), level.getHeight());
        }
    }

    std::vector<Uint32> Game::collectPlayerInputs() const {
        std::vector<Uint32> controllerStates;
        controllerStates.reserve(localPlayerControls.size());
        for (const PlayerControls *controls : localPlayerControls) {
            controllerStates.push_back(readControllerState(*controls));
        }
        return controllerStates;
    }

    void Game::switchScreenMode() {
        settings.setScreenMode((settings.getScreenMode() == ScreenMode::FullScreen) ? ScreenMode::SplitScreen
                                                                                    : ScreenMode::FullScreen);
        prepareRoundClientView();
    }
}
