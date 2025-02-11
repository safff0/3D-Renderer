#include "application.h"
#include "except.h"

int main() {
    try {
        app::Application app;
        app.Run();
    } catch (...) {
        app::except::React();
    }
}
