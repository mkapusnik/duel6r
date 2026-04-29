#include "ServerConfig.h"

#include <stdexcept>
#include <string>

namespace Duel6::Server {
    namespace {
        bool startsWith(const std::string &value, const std::string &prefix) {
            return value.compare(0, prefix.size(), prefix) == 0;
        }

        std::string valueAfter(const std::string &argument, const std::string &prefix) {
            if (!startsWith(argument, prefix)) {
                throw std::invalid_argument("Expected argument prefix: " + prefix);
            }
            return argument.substr(prefix.size());
        }

        std::uint16_t parsePort(const std::string &value) {
            unsigned long port = std::stoul(value);
            if (port == 0 || port > 65535) {
                throw std::invalid_argument("Port must be in range 1..65535");
            }
            return static_cast<std::uint16_t>(port);
        }

        std::uint32_t parsePositiveUint32(const std::string &name, const std::string &value) {
            unsigned long parsed = std::stoul(value);
            if (parsed == 0) {
                throw std::invalid_argument(name + " must be positive");
            }
            return static_cast<std::uint32_t>(parsed);
        }
    }

    ServerConfig parseServerConfig(int argc, char **argv) {
        ServerConfig config;

        for (int i = 1; i < argc; ++i) {
            std::string argument = argv[i];
            if (startsWith(argument, "--host=")) {
                config.listenEndpoint.host = valueAfter(argument, "--host=");
            } else if (startsWith(argument, "--port=")) {
                config.listenEndpoint.port = parsePort(valueAfter(argument, "--port="));
            } else if (startsWith(argument, "--name=")) {
                config.serverName = valueAfter(argument, "--name=");
            } else if (startsWith(argument, "--resources=")) {
                config.resourcePath = valueAfter(argument, "--resources=");
            } else if (startsWith(argument, "--token=")) {
                config.authToken = valueAfter(argument, "--token=");
            } else if (startsWith(argument, "--tick-rate=")) {
                config.tickRate = parsePositiveUint32("tick rate", valueAfter(argument, "--tick-rate="));
            } else if (startsWith(argument, "--max-clients=")) {
                config.maxClients = parsePositiveUint32("max clients", valueAfter(argument, "--max-clients="));
            } else if (argument == "--local-only") {
                config.localOnly = true;
                config.listenEndpoint.host = "127.0.0.1";
            } else if (argument == "--once") {
                config.once = true;
            } else if (argument == "--help") {
                throw std::invalid_argument(
                        "Usage: duel6r-server [--host=ADDR] [--port=PORT] [--name=NAME] [--resources=PATH] "
                        "[--token=TOKEN] [--tick-rate=N] [--max-clients=N] [--local-only] [--once]");
            } else {
                throw std::invalid_argument("Unknown server argument: " + argument);
            }
        }

        return config;
    }
}
