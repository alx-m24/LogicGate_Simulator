#include <LexviEngine/LexviEngine.hpp>
#include <LexviEngine/Game/Game.hpp>
#include <memory>
#include <iostream>

class LogicGateSimulator : public Game {
    public:
        bool loadResources([[maybe_unused]] Lexvi::Engine& engine) override {
            std::cout << "Init" << std::endl;
            return true;
        }

        void update([[maybe_unused]] Lexvi::Engine& engine, [[maybe_unused]] float dt) override {
            std::cout << "update" << std::endl;
        }

        void render([[maybe_unused]] Lexvi::Renderer& renderer) override {
            std::cout << "render" << std::endl;
        }

        void shutdown() override {
            std::cout << "Shutdown" << std::endl;
        }
};

int main() {
    constexpr bool VSYNC_ENABLED = true;
    constexpr bool BUILTIN_PERFORMANCE_UI_ENABLED = false;
    Lexvi::Engine engine("LogicGateSimulator", std::make_unique<LogicGateSimulator>(), VSYNC_ENABLED, BUILTIN_PERFORMANCE_UI_ENABLED);

    engine.run();

    return EXIT_SUCCESS;
}
