/*
 * NetHost.h
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_NETHOST_H_
#define SOURCE_NET_NETHOST_H_

#include <memory>
#include <string>
#include <vector>
#include <enet/enet.h>
#include "Service.h"
#include "Peer.h"
//#include "GameProxy.h"
namespace Duel6 {
    class Game;
    class Console;

    namespace net {
        class ClientGameProxy;
        class ServerGameProxy;

        class NetHost: public Service {
        private:
            Console &console;
            std::vector<std::unique_ptr<Peer>> peers;
        public:
            NetHost(ClientGameProxy & clientGameProxy, ServerGameProxy & serverGameProxy, Console &console);

            virtual ~NetHost();

            void listen(Game &game, const std::string &host, const Duel6::net::port_t port);

        private:
            void onStarting() override;
            void onStopping() override;
            void onStopped() override;

            void onPeerConnected(ENetPeer*) override;
            void onPeerDisconnected(ENetPeer*, enet_uint32 reason) override;
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_NETHOST_H_ */
