#include "ProtocolSerialization.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>

namespace Duel6::Network {
    namespace {
        using Properties = std::map<std::string, std::vector<std::string>>;

        bool isUnescaped(char chr) {
            return std::isalnum(static_cast<unsigned char>(chr)) || chr == '-' || chr == '_' || chr == '.' || chr == '~';
        }

        std::string escape(const std::string &value) {
            std::ostringstream stream;
            stream << std::uppercase << std::hex;
            for (unsigned char chr: value) {
                if (isUnescaped(static_cast<char>(chr))) {
                    stream << static_cast<char>(chr);
                } else {
                    stream << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(chr);
                }
            }
            return stream.str();
        }

        int hexValue(char chr) {
            if (chr >= '0' && chr <= '9') {
                return chr - '0';
            }
            if (chr >= 'A' && chr <= 'F') {
                return chr - 'A' + 10;
            }
            if (chr >= 'a' && chr <= 'f') {
                return chr - 'a' + 10;
            }
            throw std::invalid_argument("Invalid escape sequence");
        }

        std::string unescape(const std::string &value) {
            std::string result;
            for (std::size_t i = 0; i < value.size(); ++i) {
                if (value[i] == '%') {
                    if (i + 2 >= value.size()) {
                        throw std::invalid_argument("Truncated escape sequence");
                    }
                    int high = hexValue(value[i + 1]);
                    int low = hexValue(value[i + 2]);
                    result += static_cast<char>((high << 4) | low);
                    i += 2;
                } else {
                    result += value[i];
                }
            }
            return result;
        }

        std::string serializeProperties(const Properties &properties) {
            std::ostringstream stream;
            for (const auto &property: properties) {
                for (const auto &value: property.second) {
                    stream << escape(property.first) << '=' << escape(value) << '\n';
                }
            }
            return stream.str();
        }

        Properties parseProperties(const std::string &payload) {
            Properties properties;
            std::istringstream stream(payload);
            std::string line;
            while (std::getline(stream, line)) {
                if (line.empty()) {
                    continue;
                }
                std::size_t separator = line.find('=');
                if (separator == std::string::npos) {
                    throw std::invalid_argument("Protocol property is missing '=' separator");
                }
                properties[unescape(line.substr(0, separator))].push_back(unescape(line.substr(separator + 1)));
            }
            return properties;
        }

        void set(Properties &properties, const std::string &key, const std::string &value) {
            properties[key].push_back(value);
        }

        std::string requiredString(const Properties &properties, const std::string &key) {
            auto it = properties.find(key);
            if (it == properties.end() || it->second.empty()) {
                throw std::invalid_argument("Missing protocol property: " + key);
            }
            return it->second.front();
        }

        unsigned long requiredUnsignedLong(const Properties &properties, const std::string &key, unsigned long maxValue) {
            const std::string value = requiredString(properties, key);
            if (value.empty() || value[0] == '+' || value[0] == '-') {
                throw std::invalid_argument("Invalid unsigned protocol property: " + key);
            }

            std::size_t consumed = 0;
            unsigned long parsed = std::stoul(value, &consumed);
            if (consumed != value.size()) {
                throw std::invalid_argument("Invalid unsigned protocol property: " + key);
            }
            if (parsed > maxValue) {
                throw std::out_of_range("Protocol property is out of range: " + key);
            }
            return parsed;
        }

        std::uint32_t requiredUint32(const Properties &properties, const std::string &key) {
            return static_cast<std::uint32_t>(requiredUnsignedLong(properties, key,
                                                                   std::numeric_limits<std::uint32_t>::max()));
        }

        std::uint16_t requiredUint16(const Properties &properties, const std::string &key) {
            return static_cast<std::uint16_t>(requiredUnsignedLong(properties, key,
                                                                   std::numeric_limits<std::uint16_t>::max()));
        }

        std::uint8_t requiredUint8(const Properties &properties, const std::string &key) {
            return static_cast<std::uint8_t>(requiredUnsignedLong(properties, key,
                                                                  std::numeric_limits<std::uint8_t>::max()));
        }

        bool requiredBool(const Properties &properties, const std::string &key) {
            const std::string value = requiredString(properties, key);
            if (value == "true") {
                return true;
            }
            if (value == "false") {
                return false;
            }
            throw std::invalid_argument("Invalid boolean protocol property: " + key);
        }

        float requiredFloat(const Properties &properties, const std::string &key) {
            const std::string value = requiredString(properties, key);
            std::size_t consumed = 0;
            float parsed = std::stof(value, &consumed);
            if (consumed != value.size()) {
                throw std::invalid_argument("Invalid float protocol property: " + key);
            }
            return parsed;
        }

        std::string indexedKey(const std::string &prefix, std::uint32_t index, const std::string &field) {
            return prefix + "." + std::to_string(index) + "." + field;
        }

        Endpoint endpointFromProperties(const Properties &properties, const std::string &prefix) {
            Endpoint endpoint;
            endpoint.host = requiredString(properties, prefix + ".host");
            endpoint.port = requiredUint16(properties, prefix + ".port");
            return endpoint;
        }

        void endpointToProperties(Properties &properties, const std::string &prefix, const Endpoint &endpoint) {
            set(properties, prefix + ".host", endpoint.host);
            set(properties, prefix + ".port", std::to_string(endpoint.port));
        }
    }

    std::string toString(ConnectionMode mode) {
        switch (mode) {
            case ConnectionMode::LocalGame:
                return "local-game";
            case ConnectionMode::RemoteServer:
                return "remote-server";
        }
        return "local-game";
    }

    ConnectionMode connectionModeFromString(const std::string &value) {
        if (value == "local-game") {
            return ConnectionMode::LocalGame;
        }
        if (value == "remote-server") {
            return ConnectionMode::RemoteServer;
        }
        throw std::invalid_argument("Unknown connection mode: " + value);
    }

    std::string toString(RejectReason reason) {
        switch (reason) {
            case RejectReason::None:
                return "none";
            case RejectReason::IncompatibleProtocol:
                return "incompatible-protocol";
            case RejectReason::IncompatibleBuild:
                return "incompatible-build";
            case RejectReason::ContentMismatch:
                return "content-mismatch";
            case RejectReason::ServerFull:
                return "server-full";
            case RejectReason::AuthenticationFailed:
                return "authentication-failed";
            case RejectReason::MatchInProgress:
                return "match-in-progress";
            case RejectReason::InvalidRequest:
                return "invalid-request";
        }
        return "invalid-request";
    }

    RejectReason rejectReasonFromString(const std::string &value) {
        if (value == "none") {
            return RejectReason::None;
        }
        if (value == "incompatible-protocol") {
            return RejectReason::IncompatibleProtocol;
        }
        if (value == "incompatible-build") {
            return RejectReason::IncompatibleBuild;
        }
        if (value == "content-mismatch") {
            return RejectReason::ContentMismatch;
        }
        if (value == "server-full") {
            return RejectReason::ServerFull;
        }
        if (value == "authentication-failed") {
            return RejectReason::AuthenticationFailed;
        }
        if (value == "match-in-progress") {
            return RejectReason::MatchInProgress;
        }
        if (value == "invalid-request") {
            return RejectReason::InvalidRequest;
        }
        throw std::invalid_argument("Unknown reject reason: " + value);
    }

    std::string serializeEndpoint(const Endpoint &endpoint) {
        Properties properties;
        endpointToProperties(properties, "endpoint", endpoint);
        return serializeProperties(properties);
    }

    Endpoint deserializeEndpoint(const std::string &payload) {
        return endpointFromProperties(parseProperties(payload), "endpoint");
    }

    std::string serializeHandshakeRequest(const HandshakeRequest &request) {
        Properties properties;
        set(properties, "protocolVersion", std::to_string(request.protocolVersion));
        set(properties, "buildVersion", request.buildVersion);
        set(properties, "clientName", request.clientName);
        set(properties, "authToken", request.authToken);
        for (const auto &resource: request.resources) {
            set(properties, "resource", resource.path + "\t" + resource.hash);
        }
        return serializeProperties(properties);
    }

    HandshakeRequest deserializeHandshakeRequest(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        HandshakeRequest request;
        request.protocolVersion = requiredUint16(properties, "protocolVersion");
        request.buildVersion = requiredString(properties, "buildVersion");
        request.clientName = requiredString(properties, "clientName");
        request.authToken = requiredString(properties, "authToken");
        auto resources = properties.find("resource");
        if (resources != properties.end()) {
            for (const auto &entry: resources->second) {
                std::size_t separator = entry.find('\t');
                if (separator == std::string::npos) {
                    throw std::invalid_argument("Invalid resource hash entry");
                }
                request.resources.push_back({entry.substr(0, separator), entry.substr(separator + 1)});
            }
        }
        return request;
    }

    std::string serializeHandshakeAccept(const HandshakeAccept &accept) {
        Properties properties;
        set(properties, "clientId", std::to_string(accept.clientId));
        set(properties, "serverTickRate", std::to_string(accept.serverTickRate));
        set(properties, "serverName", accept.serverName);
        return serializeProperties(properties);
    }

    HandshakeAccept deserializeHandshakeAccept(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        HandshakeAccept accept;
        accept.clientId = requiredUint32(properties, "clientId");
        accept.serverTickRate = requiredUint32(properties, "serverTickRate");
        accept.serverName = requiredString(properties, "serverName");
        return accept;
    }

    std::string serializeHandshakeReject(const HandshakeReject &reject) {
        Properties properties;
        set(properties, "reason", toString(reject.reason));
        set(properties, "detail", reject.detail);
        return serializeProperties(properties);
    }

    HandshakeReject deserializeHandshakeReject(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        HandshakeReject reject;
        reject.reason = rejectReasonFromString(requiredString(properties, "reason"));
        reject.detail = requiredString(properties, "detail");
        return reject;
    }

    std::string serializeLobbyState(const LobbyState &state) {
        Properties properties;
        set(properties, "gameMode", state.gameMode);
        set(properties, "maxRounds", std::to_string(state.maxRounds));
        set(properties, "assistance", state.assistance ? "true" : "false");
        set(properties, "quickLiquid", state.quickLiquid ? "true" : "false");
        for (const auto &level: state.selectedLevels) {
            set(properties, "selectedLevel", level);
        }
        set(properties, "player.count", std::to_string(state.players.size()));
        for (std::uint32_t i = 0; i < state.players.size(); ++i) {
            const LobbyPlayer &player = state.players[i];
            set(properties, indexedKey("player", i, "playerId"), std::to_string(player.playerId));
            set(properties, indexedKey("player", i, "clientId"), std::to_string(player.clientId));
            set(properties, indexedKey("player", i, "displayName"), player.displayName);
            set(properties, indexedKey("player", i, "team"), std::to_string(player.team));
            set(properties, indexedKey("player", i, "ready"), player.ready ? "true" : "false");
        }
        return serializeProperties(properties);
    }

    LobbyState deserializeLobbyState(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        LobbyState state;
        state.gameMode = requiredString(properties, "gameMode");
        state.maxRounds = requiredUint32(properties, "maxRounds");
        state.assistance = requiredBool(properties, "assistance");
        state.quickLiquid = requiredBool(properties, "quickLiquid");
        auto selectedLevels = properties.find("selectedLevel");
        if (selectedLevels != properties.end()) {
            state.selectedLevels = selectedLevels->second;
        }
        std::uint32_t playerCount = requiredUint32(properties, "player.count");
        for (std::uint32_t i = 0; i < playerCount; ++i) {
            LobbyPlayer player;
            player.playerId = requiredUint32(properties, indexedKey("player", i, "playerId"));
            player.clientId = requiredUint32(properties, indexedKey("player", i, "clientId"));
            player.displayName = requiredString(properties, indexedKey("player", i, "displayName"));
            player.team = requiredUint8(properties, indexedKey("player", i, "team"));
            player.ready = requiredBool(properties, indexedKey("player", i, "ready"));
            state.players.push_back(player);
        }
        return state;
    }

    std::string serializeInputCommand(const InputCommand &command) {
        Properties properties;
        set(properties, "clientId", std::to_string(command.clientId));
        set(properties, "playerId", std::to_string(command.playerId));
        set(properties, "sequence", std::to_string(command.sequence));
        set(properties, "targetTick", std::to_string(command.targetTick));
        set(properties, "actions", std::to_string(command.actions));
        return serializeProperties(properties);
    }

    InputCommand deserializeInputCommand(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        InputCommand command;
        command.clientId = requiredUint32(properties, "clientId");
        command.playerId = requiredUint32(properties, "playerId");
        command.sequence = requiredUint32(properties, "sequence");
        command.targetTick = requiredUint32(properties, "targetTick");
        command.actions = requiredUint32(properties, "actions");
        return command;
    }

    std::string serializeSnapshot(const Snapshot &snapshot) {
        Properties properties;
        set(properties, "snapshotId", std::to_string(snapshot.snapshotId));
        set(properties, "baselineId", std::to_string(snapshot.baselineId));
        set(properties, "tick", std::to_string(snapshot.tick));
        set(properties, "roundNumber", std::to_string(snapshot.roundNumber));
        set(properties, "player.count", std::to_string(snapshot.players.size()));
        for (std::uint32_t i = 0; i < snapshot.players.size(); ++i) {
            const PlayerSnapshot &player = snapshot.players[i];
            set(properties, indexedKey("player", i, "playerId"), std::to_string(player.playerId));
            set(properties, indexedKey("player", i, "x"), std::to_string(player.x));
            set(properties, indexedKey("player", i, "y"), std::to_string(player.y));
            set(properties, indexedKey("player", i, "life"), std::to_string(player.life));
            set(properties, indexedKey("player", i, "alive"), player.alive ? "true" : "false");
        }
        return serializeProperties(properties);
    }

    Snapshot deserializeSnapshot(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        Snapshot snapshot;
        snapshot.snapshotId = requiredUint32(properties, "snapshotId");
        snapshot.baselineId = requiredUint32(properties, "baselineId");
        snapshot.tick = requiredUint32(properties, "tick");
        snapshot.roundNumber = requiredUint32(properties, "roundNumber");
        std::uint32_t playerCount = requiredUint32(properties, "player.count");
        for (std::uint32_t i = 0; i < playerCount; ++i) {
            PlayerSnapshot player;
            player.playerId = requiredUint32(properties, indexedKey("player", i, "playerId"));
            player.x = requiredFloat(properties, indexedKey("player", i, "x"));
            player.y = requiredFloat(properties, indexedKey("player", i, "y"));
            player.life = requiredFloat(properties, indexedKey("player", i, "life"));
            player.alive = requiredBool(properties, indexedKey("player", i, "alive"));
            snapshot.players.push_back(player);
        }
        return snapshot;
    }

    std::string serializeEvent(const Event &event) {
        Properties properties;
        set(properties, "eventId", std::to_string(event.eventId));
        set(properties, "type", event.type);
        set(properties, "payload", event.payload);
        return serializeProperties(properties);
    }

    Event deserializeEvent(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        Event event;
        event.eventId = requiredUint32(properties, "eventId");
        event.type = requiredString(properties, "type");
        event.payload = requiredString(properties, "payload");
        return event;
    }

    std::string serializeDisconnect(const Disconnect &disconnect) {
        Properties properties;
        set(properties, "reason", disconnect.reason);
        set(properties, "canReconnect", disconnect.canReconnect ? "true" : "false");
        return serializeProperties(properties);
    }

    Disconnect deserializeDisconnect(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        Disconnect disconnect;
        disconnect.reason = requiredString(properties, "reason");
        disconnect.canReconnect = requiredBool(properties, "canReconnect");
        return disconnect;
    }

    std::string serializeClientConnectionConfig(const ClientConnectionConfig &config) {
        Properties properties;
        set(properties, "mode", toString(config.mode));
        endpointToProperties(properties, "remoteEndpoint", config.remoteEndpoint);
        endpointToProperties(properties, "localEndpoint", config.localEndpoint);
        set(properties, "localServerExecutable", config.localServerExecutable);
        set(properties, "authToken", config.authToken);
        return serializeProperties(properties);
    }

    ClientConnectionConfig deserializeClientConnectionConfig(const std::string &payload) {
        const Properties properties = parseProperties(payload);
        ClientConnectionConfig config;
        config.mode = connectionModeFromString(requiredString(properties, "mode"));
        config.remoteEndpoint = endpointFromProperties(properties, "remoteEndpoint");
        config.localEndpoint = endpointFromProperties(properties, "localEndpoint");
        config.localServerExecutable = requiredString(properties, "localServerExecutable");
        config.authToken = requiredString(properties, "authToken");
        return config;
    }
}
