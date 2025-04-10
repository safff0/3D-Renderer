#include "application.h"
#include "except.h"

int main(int argc, char** argv) {
    try {
        app::donut::Application app;
        app.Run();
    } catch (...) {
        app::except::React();
    }
}
