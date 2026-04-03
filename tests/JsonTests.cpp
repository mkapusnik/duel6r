#include <filesystem>
#include <fstream>

#include "source/json/JsonException.h"
#include "source/json/JsonParser.h"
#include "source/json/JsonWriter.h"
#include "tests/TestHarness.h"

namespace {
    std::filesystem::path makeTempFilePath(const std::string &fileName) {
        std::filesystem::path directory = std::filesystem::temp_directory_path() / "duel6r-tests";
        std::filesystem::create_directories(directory);
        return directory / fileName;
    }
}

D6R_TEST_CASE("JSON writer round-trips nested values") {
    Duel6::Json::Value inventory = Duel6::Json::Value::makeArray();
    inventory.add(Duel6::Json::Value::makeString("laser"));
    inventory.add(Duel6::Json::Value::makeNumber(3));
    inventory.add(Duel6::Json::Value::makeBoolean(true));

    Duel6::Json::Value spawn = Duel6::Json::Value::makeObject();
    spawn.set("x", Duel6::Json::Value::makeNumber(12));
    spawn.set("y", Duel6::Json::Value::makeNumber(4));

    Duel6::Json::Value root = Duel6::Json::Value::makeObject();
    root.set("name", Duel6::Json::Value::makeString("arena-bot"));
    root.set("health", Duel6::Json::Value::makeNumber(87));
    root.set("accuracy", Duel6::Json::Value::makeNumber(0.75));
    root.set("active", Duel6::Json::Value::makeBoolean(true));
    root.set("inventory", inventory);
    root.set("spawn", spawn);
    root.set("script", Duel6::Json::Value::makeNull());

    Duel6::Json::Writer writer(false);
    std::filesystem::path tempFile = makeTempFilePath("json-roundtrip.json");
    {
        std::ofstream stream(tempFile);
        stream << writer.writeToString(root);
    }

    Duel6::Json::Parser parser;
    Duel6::Json::Value parsed = parser.parse(tempFile.string());

    D6R_REQUIRE_EQ(std::string("arena-bot"), parsed.get("name").asString());
    D6R_REQUIRE_EQ(87, parsed.get("health").asInt());
    D6R_REQUIRE_NEAR(0.75, parsed.get("accuracy").asDouble(), 1e-9);
    D6R_REQUIRE(parsed.get("active").asBoolean());
    D6R_REQUIRE_EQ(3u, parsed.get("inventory").getLength());
    D6R_REQUIRE_EQ(std::string("laser"), parsed.get("inventory").get(0).asString());
    D6R_REQUIRE_EQ(12, parsed.get("spawn").get("x").asInt());
    D6R_REQUIRE(parsed.get("script").getType() == Duel6::Json::Value::Type::Null);

    std::filesystem::remove(tempFile);
}

D6R_TEST_CASE("JSON parser rejects malformed objects") {
    std::filesystem::path tempFile = makeTempFilePath("json-invalid.json");
    {
        std::ofstream stream(tempFile);
        stream << "{\"width\": 20, }";
    }

    Duel6::Json::Parser parser;
    D6R_REQUIRE_THROW(parser.parse(tempFile.string()), Duel6::JsonException);

    std::filesystem::remove(tempFile);
}
