#pragma once

#include <memory>

#include <SDL3/SDL.h>

namespace lxe
{
  class Renderer;

  class App final : std::enable_shared_from_this<App>
  {
    friend Renderer;

    class Secret
    {
    };

   public:
    explicit App(Secret);
    ~App();

    App(const App& other) = delete;
    App(App&& other) noexcept = delete;
    App& operator=(const App& other) = delete;
    App& operator=(App&& other) noexcept = delete;

   private:
    std::unique_ptr<Renderer> Renderer_;

   private:
    SDL_Window* SdlWindow_{};

   public:
    auto Run() -> void;

   private:
    auto Impl_Init() -> void;
    auto Impl_CreateWindow() -> void;

   public:
    static auto Make() -> std::unique_ptr<App>;
  };
}  // namespace lxe
