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

#ifndef DUEL6_GAME_H
#define DUEL6_GAME_H

#include <memory>
#include <utility>
#include <vector>
#include <string>
#include "Type.h"
#include "ScreenMode.h"
#include "Context.h"
#include "Player.h"
#include "WorldRenderer.h"
#include "AppService.h"
#include "GamePlayerDefinition.h"
#include "GameSettings.h"
#include "GameResources.h"
#include "GameServerTransport.h"
#include "Round.h"

namespace Duel6 {
    class GameMode;

    class Menu;

    class Game : public Context {
    private:
        AppService &appService;
        GameResources &resources;
        GameSettings &settings;
        std::unique_ptr<GameServerTransport> serverTransport;
        WorldRenderer worldRenderer;
        const Menu *menu;
        std::vector<const PlayerControls *> localPlayerControls;
        bool displayScoreTab = false;

    public:
        Game(AppService &appService, GameResources &resources, GameSettings &settings);

        void start(const std::vector<GamePlayerDefinition> &playerDefinitions, const std::vector<std::string> &levels,
                   const std::vector<Size> &backgrounds, ScreenMode screenMode, Int32 screenZoom, GameMode &gameMode);

        void keyEvent(const KeyPressEvent &event) override;

        void textInputEvent(const TextInputEvent &event) override;

        void mouseButtonEvent(const MouseButtonEvent &event) override;

        void mouseMotionEvent(const MouseMotionEvent &event) override;

        void mouseWheelEvent(const MouseWheelEvent &event) override;

        void joyDeviceAddedEvent(const JoyDeviceAddedEvent & event) override;

        void joyDeviceRemovedEvent(const JoyDeviceRemovedEvent & event) override;

        void update(Float32 elapsedTime) override;

        void render() const override;

        AppService &getAppService() const {
            return appService;
        }

        const std::vector<Size> &getBackgrounds() const;

        std::vector<Player> &getPlayers();

        const std::vector<Player> &getPlayers() const;

        GameResources &getResources() {
            return resources;
        }

        const GameResources &getResources() const {
            return resources;
        }

        GameSettings &getSettings() {
            return settings;
        }

        const GameSettings &getSettings() const {
            return settings;
        }

        Round &getRound();

        const Round &getRound() const;

        Int32 getPlayedRounds() const;

        void setPlayedRounds(Int32 playedRounds);

        Int32 getCurrentRound() const;

        bool isOver() const;

        bool isDisplayingScoreTab() const {
            return displayScoreTab;
        }

        WorldRenderer &getWorldRenderer() {
            return worldRenderer;
        }

        const WorldRenderer &getWorldRenderer() const {
            return worldRenderer;
        }

        GameMode &getMode();

        const GameMode &getMode() const;

        void setMenuReference(const Menu &menu) {
            this->menu = &menu;
        }

    private:
        void beforeStart(Context *prevContext) override;

        void beforeClose(Context *nextContext) override;

        void startRound();

        void prepareRoundClientView();

        void updatePlayerViews();

        void updatePlayerCameras();

        void splitScreenView(Player &player, Int32 x, Int32 y) const;

        std::vector<Uint32> collectPlayerInputs() const;

        void switchScreenMode();

        void nextRound();

        void endRound();

        void onRoundEnd();
    };
}

#endif
