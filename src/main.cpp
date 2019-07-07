#include <tsl/application.hpp>

int main() {
    auto& app = tsl::application::get_instance();
    app.create_window("T-Spline Editor", 900, 900);
    app.run();
    exit(EXIT_SUCCESS);
}
