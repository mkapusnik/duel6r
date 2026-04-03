#include <exception>
#include <iostream>

#include "source/Exception.h"
#include "tests/TestHarness.h"

int main() {
    const auto &tests = Duel6::Test::registry();
    std::size_t failed = 0;

    for (const auto &test: tests) {
        try {
            test.function();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const Duel6::Test::Failure &failure) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << '\n' << "  " << failure.what() << '\n';
        } catch (const Duel6::Exception &exception) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << '\n'
                      << "  Unexpected Duel6 exception at " << exception.getFile() << ':' << exception.getLine()
                      << " - " << exception.getMessage() << '\n';
        } catch (const std::exception &exception) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << '\n' << "  Unexpected std::exception: " << exception.what()
                      << '\n';
        } catch (...) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << '\n' << "  Unknown exception\n";
        }
    }

    std::cout << "Executed " << tests.size() << " test(s), failures: " << failed << '\n';
    return failed == 0 ? 0 : 1;
}
