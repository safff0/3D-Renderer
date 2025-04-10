#include "application.h"
#include "except.h"

#include <iostream>

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            std::cerr << "No .obj path\n";
            return 0;
        }
        app::mv::Application app;
        app.Run(argv[1]);
    } catch (...) {
        app::except::React();
    }
}
