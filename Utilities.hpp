#pragma once

#include <iostream>

namespace IO {
    template<typename... Values>
    auto print(Values &&... values) -> void {
        (std::cout << ... << std::forward<Values>(values));
    }
}  // end namespace IO
