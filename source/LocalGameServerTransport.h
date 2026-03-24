#ifndef DUEL6_LOCALGAMESERVERTRANSPORT_H
#define DUEL6_LOCALGAMESERVERTRANSPORT_H

#include "GameServer.h"
#include "GameServerTransport.h"

namespace Duel6 {
    class LocalGameServerTransport : public GameServerTransport {
    private:
        GameServer server;

    public:
        LocalGameServerTransport(Game &gameHost, AppService &appService, GameResources &resources,
                                 GameSettings &settings)
                : server(gameHost, appService, resources, settings) {}

        void start(const std::vector<GamePlayerDefinition> &playerDefinitions,
                   const std::vector<std::string> &levels,
                   const std::vector<Size> &backgrounds,
                   GameMode &gameMode) override;

        void submitPlayerInputs(const std::vector<Uint32> &controllerStates) override;

        void update(Float32 elapsedTime) override;

        void startNextRound() override;

        void endRound() override;

        GameServer &getServer() override {
            return server;
        }

        const GameServer &getServer() const override {
            return server;
        }
    };
}

#endif
