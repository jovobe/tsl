#include <tsl/Application.hpp>

int main() {
    auto& app = tsl::Application::getInstance();
    app.create_window("main", 1920, 1080);
    app.run();
    exit(EXIT_SUCCESS);
}
