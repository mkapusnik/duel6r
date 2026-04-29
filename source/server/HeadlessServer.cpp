#include "HeadlessServer.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace Duel6::Server {
    HeadlessServer::HeadlessServer(ServerConfig config)
            : config(std::move(config)), nextClientId(1) {}

    const ServerConfig &HeadlessServer::getConfig() const {
        return config;
    }

    HandshakeResult HeadlessServer::validateHandshake(const Network::HandshakeRequest &request) const {
        HandshakeResult result;
        if (request.protocolVersion != Network::ProtocolVersion) {
            result.reject.reason = Network::RejectReason::IncompatibleProtocol;
            result.reject.detail = "Client protocol version is incompatible";
            return result;
        }

        if (!config.authToken.empty() && request.authToken != config.authToken) {
            result.reject.reason = Network::RejectReason::AuthenticationFailed;
            result.reject.detail = "Client authentication token is invalid";
            return result;
        }

        result.accepted = true;
        result.accept.serverTickRate = config.tickRate;
        result.accept.serverName = config.serverName;
        return result;
    }

    Network::HandshakeAccept HeadlessServer::acceptHandshake(const Network::HandshakeRequest &request) {
        HandshakeResult result = validateHandshake(request);
        if (!result.accepted) {
            throw std::invalid_argument(result.reject.detail);
        }

        result.accept.clientId = nextClientId++;
        return result.accept;
    }

    int HeadlessServer::run(std::ostream &output) {
        output << "duel6r-server ready " << config.listenEndpoint.host << ':' << config.listenEndpoint.port
               << " tick-rate=" << config.tickRate
               << " max-clients=" << config.maxClients
               << " resources=" << config.resourcePath
               << " local-only=" << (config.localOnly ? "true" : "false")
               << '\n';

        if (config.once) {
            output << "duel6r-server exiting after readiness check (--once)\n";
            return 0;
        }

        output << "duel6r-server scaffold is running without transport; terminate the process to stop it.\n";
        return 0;
    }
}
