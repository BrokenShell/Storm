#pragma once

#include <iostream>
#include <utility>

namespace IO {
    template<typename... Values>
    auto print(Values&&... values) -> void {
        (std::cout << ... << std::forward<Values>(values));
    }
}
