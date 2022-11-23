#include "StormTests.hpp"

auto main() -> int {
    Storm::Engine::seed(42);
    StormTests::run_tests();
}
