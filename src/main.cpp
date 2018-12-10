#include <tsl/Application.hpp>

int main() {
    auto& app = tsl::Application::getInstance();
    app.create_window("main", 900, 900);
    app.run();
    exit(EXIT_SUCCESS);
}
