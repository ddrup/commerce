#include "say_hello.hpp"

#include <fmt/format.h>

namespace samples::hello {

    std::string hello(std::string_view name) {
        if (name.empty()) {
            return "unknown user";
        }

        return fmt::format("Hello, {}!\n", name);
    }

} // samples::hello