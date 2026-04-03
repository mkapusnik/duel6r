#include <string>

#include "source/Format.h"
#include "source/FormatException.h"
#include "tests/TestHarness.h"

using Duel6::Format;

D6R_TEST_CASE("Format applies padding and alignment") {
    std::string value = static_cast<std::string>(Format("HP {0,4|0} {1,-5}") << 7 << "bot");
    D6R_REQUIRE_EQ(std::string("HP 0007 bot  "), value);
}

D6R_TEST_CASE("Format throws on unclosed placeholder") {
    D6R_REQUIRE_THROW(static_cast<std::string>(Format("{0,4") << 7), Duel6::FormatException);
}
