#include "GameMode.h"
#include "LocalGameServerTransport.h"

namespace Duel6 {
    void LocalGameServerTransport::start(const std::vector<GamePlayerDefinition> &playerDefinitions,
                                         const std::vector<std::string> &levels,
                                         const std::vector<Size> &backgrounds,
                                         GameMode &gameMode) {
        server.start(playerDefinitions, levels, backgrounds, gameMode);
    }

    void LocalGameServerTransport::submitPlayerInputs(const std::vector<Uint32> &controllerStates) {
        server.setPlayerInputs(controllerStates);
    }

    void LocalGameServerTransport::update(Float32 elapsedTime) {
        server.update(elapsedTime);
    }

    void LocalGameServerTransport::startNextRound() {
        server.startNextRound();
    }

    void LocalGameServerTransport::endRound() {
        server.endRound();
    }
}
