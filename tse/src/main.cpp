#include "tse/application.hpp"

int main() {
    auto& app = tse::application::get_instance();
    app.create_window("T-Spline Editor", 900, 900);
    app.run();
    exit(EXIT_SUCCESS);
}
