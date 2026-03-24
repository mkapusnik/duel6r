#ifndef DUEL6_GAMESERVERTRANSPORT_H
#define DUEL6_GAMESERVERTRANSPORT_H

#include <string>
#include <vector>
#include "GamePlayerDefinition.h"
#include "Type.h"

namespace Duel6 {
    class GameMode;
    class GameServer;

    class GameServerTransport {
    public:
        virtual ~GameServerTransport() = default;

        virtual void start(const std::vector<GamePlayerDefinition> &playerDefinitions,
                           const std::vector<std::string> &levels,
                           const std::vector<Size> &backgrounds,
                           GameMode &gameMode) = 0;

        virtual void submitPlayerInputs(const std::vector<Uint32> &controllerStates) = 0;

        virtual void update(Float32 elapsedTime) = 0;

        virtual void startNextRound() = 0;

        virtual void endRound() = 0;

        virtual GameServer &getServer() = 0;

        virtual const GameServer &getServer() const = 0;
    };
}

#endif
