/*
 * NetTypes.h
 *
 *  Created on: Mar 14, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_TYPE_H_
#define SOURCE_NET_TYPE_H_

#include <cstdint>

namespace Duel6 {
    namespace net {
        typedef uint8_t  type_t; //for enums
        typedef uint16_t port_t;
        typedef uint16_t object_id_t;
        typedef uint16_t peer_id_t;
        typedef uint16_t tick_t;
        // server and client have their own tick counters
        // these numbers are almost the same most of the time
        // but must not be mixed - e.g. gamestateupdate.snapshottick
        // is part of the server-tick loop only
        typedef tick_t client_tick_t;
        typedef tick_t server_tick_t;
    }
}



#endif /* SOURCE_NET_TYPE_H_ */