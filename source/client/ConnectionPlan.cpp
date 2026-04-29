#include "ConnectionPlan.h"

namespace Duel6::Client {
    ConnectionPlan createConnectionPlan(const Network::ClientConnectionConfig &config) {
        ConnectionPlan plan;
        plan.config = config;

        if (config.mode == Network::ConnectionMode::LocalGame) {
            plan.launchesLocalServer = true;
            plan.endpoint = config.localEndpoint;
            plan.localServerArguments.push_back(config.localServerExecutable);
            plan.localServerArguments.push_back("--local-only");
            plan.localServerArguments.push_back("--host=" + config.localEndpoint.host);
            plan.localServerArguments.push_back("--port=" + std::to_string(config.localEndpoint.port));
            if (!config.authToken.empty()) {
                plan.localServerArguments.push_back("--token=" + config.authToken);
            }
        } else {
            plan.launchesLocalServer = false;
            plan.endpoint = config.remoteEndpoint;
        }

        return plan;
    }
}
