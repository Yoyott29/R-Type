#include "Check.hpp"
#include <exception>
#include <iostream>

int main()
{
    int failed = 0;
    for (const check::Test &test : check::tests()) {
        try {
            test.run();
        } catch (const std::exception &error) {
            std::cout << "FAIL " << test.name << ": " << error.what() << '\n';
            ++failed;
        }
    }
    std::cout << "network_tests: " << check::tests().size() - failed << " passed, " << failed
              << " failed\n";
    return failed == 0 ? 0 : 1;
}
