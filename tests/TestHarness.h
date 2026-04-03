#ifndef DUEL6_TESTS_TESTHARNESS_H
#define DUEL6_TESTS_TESTHARNESS_H

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Duel6::Test {
    class Failure : public std::runtime_error {
    public:
        explicit Failure(const std::string &message)
                : std::runtime_error(message) {}
    };

    struct TestCase {
        const char *name;
        void (*function)();
    };

    inline std::vector<TestCase> &registry() {
        static std::vector<TestCase> testCases;
        return testCases;
    }

    class Registrar {
    public:
        Registrar(const char *name, void (*function)()) {
            registry().push_back({name, function});
        }
    };

    template<typename T>
    std::string toString(const T &value) {
        std::ostringstream stream;
        stream << value;
        return stream.str();
    }

    inline std::string toString(const std::string &value) {
        return '"' + value + '"';
    }

    inline std::string toString(const char *value) {
        return value == nullptr ? "<null>" : '"' + std::string(value) + '"';
    }

    [[noreturn]] inline void fail(const char *expression, const char *file, int line, const std::string &details = "") {
        std::ostringstream stream;
        stream << file << ':' << line << ": assertion failed: " << expression;
        if (!details.empty()) {
            stream << " (" << details << ')';
        }
        throw Failure(stream.str());
    }

    template<typename TExpected, typename TActual>
    void requireEqual(const TExpected &expected, const TActual &actual, const char *expectedExpr, const char *actualExpr,
                      const char *file, int line) {
        if (!(expected == actual)) {
            fail(actualExpr, file, line,
                 std::string("expected ") + expectedExpr + " == " + actualExpr + ", got " + toString(expected) +
                 " and " + toString(actual));
        }
    }

    template<typename TExpected, typename TActual, typename TEpsilon>
    void requireNear(const TExpected &expected, const TActual &actual, const TEpsilon &epsilon, const char *expectedExpr,
                     const char *actualExpr, const char *epsilonExpr, const char *file, int line) {
        auto diff = std::fabs(static_cast<double>(expected - actual));
        if (diff > static_cast<double>(epsilon)) {
            fail(actualExpr, file, line,
                 std::string("expected ") + expectedExpr + " ~= " + actualExpr + " within " + epsilonExpr +
                 ", diff is " + toString(diff));
        }
    }
}

#define D6R_TEST_CONCAT_IMPL(left, right) left##right
#define D6R_TEST_CONCAT(left, right) D6R_TEST_CONCAT_IMPL(left, right)

#define D6R_TEST_CASE(name) \
    static void D6R_TEST_CONCAT(testFunction_, __LINE__)(); \
    static ::Duel6::Test::Registrar D6R_TEST_CONCAT(testRegistrar_, __LINE__)(name, D6R_TEST_CONCAT(testFunction_, __LINE__)); \
    static void D6R_TEST_CONCAT(testFunction_, __LINE__)()

#define D6R_REQUIRE(condition) \
    do { \
        if (!(condition)) { \
            ::Duel6::Test::fail(#condition, __FILE__, __LINE__); \
        } \
    } while (false)

#define D6R_REQUIRE_EQ(expected, actual) \
    do { \
        ::Duel6::Test::requireEqual((expected), (actual), #expected, #actual, __FILE__, __LINE__); \
    } while (false)

#define D6R_REQUIRE_NEAR(expected, actual, epsilon) \
    do { \
        ::Duel6::Test::requireNear((expected), (actual), (epsilon), #expected, #actual, #epsilon, __FILE__, __LINE__); \
    } while (false)

#define D6R_REQUIRE_THROW(expression, exception_type) \
    do { \
        bool d6rCaughtExpectedException = false; \
        try { \
            expression; \
        } catch (const exception_type &) { \
            d6rCaughtExpectedException = true; \
        } catch (...) { \
            ::Duel6::Test::fail(#expression, __FILE__, __LINE__, "threw an unexpected exception type"); \
        } \
        if (!d6rCaughtExpectedException) { \
            ::Duel6::Test::fail(#expression, __FILE__, __LINE__, "did not throw " #exception_type); \
        } \
    } while (false)

#endif
