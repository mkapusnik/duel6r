#ifndef DUEL6_SERVER_HEADLESSSERVER_H
#define DUEL6_SERVER_HEADLESSSERVER_H

#include <iosfwd>

#include "ServerConfig.h"
#include "../network/Protocol.h"

namespace Duel6::Server {
    class HeadlessServer {
    private:
        ServerConfig config;
        std::uint32_t nextClientId;

    public:
        explicit HeadlessServer(ServerConfig config);

        const ServerConfig &getConfig() const;

        Network::HandshakeAccept acceptHandshake(const Network::HandshakeRequest &request);

        int run(std::ostream &output);
    };
}

#endif
