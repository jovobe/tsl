#include <tsl/Application.hpp>

int main() {
    auto& app = tsl::Application::getInstance();
    app.create_window("main", 640, 480);
    app.run();
    exit(EXIT_SUCCESS);
}
