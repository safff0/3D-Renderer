#include "application.h"
#include "except.h"

int main(int argc, char** argv) {
    try {
        app::mv::Application app;
        app.Run(argv[1]);
    } catch (...) {
        app::except::React();
    }
}
