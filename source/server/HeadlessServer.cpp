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

    Network::HandshakeAccept HeadlessServer::acceptHandshake(const Network::HandshakeRequest &request) {
        if (request.protocolVersion != Network::ProtocolVersion) {
            throw std::invalid_argument("Client protocol version is incompatible");
        }

        if (!config.authToken.empty() && request.authToken != config.authToken) {
            throw std::invalid_argument("Client authentication token is invalid");
        }

        Network::HandshakeAccept accept;
        accept.clientId = nextClientId++;
        accept.serverTickRate = config.tickRate;
        accept.serverName = config.serverName;
        return accept;
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
