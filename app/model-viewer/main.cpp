#include "application.h"
#include "except.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "No .obj path\n";
        return 0;
    }
    try {
        app::mv::Application app{argv[1]};
        app.Run();
    } catch (...) {
        app::except::React();
    }
}
