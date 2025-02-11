#include "except.h"

#include <iostream>

namespace app::except {

void React() {
    try {
        throw;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }
}

}  // namespace app::except
