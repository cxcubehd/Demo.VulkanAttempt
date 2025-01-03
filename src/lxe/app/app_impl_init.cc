#include <stdexcept>
#include <thread>

#include <SDL3/SDL_vulkan.h>

#include <lxe/render/renderer.hh>

#include "app.hh"

namespace lxe
{
  auto App::Impl_Init() -> void
  {
    // Init sdl
    if (!SDL_Init(SDL_INIT_VIDEO))
      throw std::runtime_error("Failed to initialize SDL");

    // Init vulkan
    if (!SDL_Vulkan_LoadLibrary(nullptr))
      throw std::runtime_error("Failed to load Vulkan");

    // Create window
    Impl_CreateWindow();

    SDL_ShowWindow(SdlWindow_);

    Renderer_->Init();

    Renderer_->Impl_Render();

    while (true)
    {
      //std::this_thread::sleep_for(std::chrono::seconds(1));
      // handle sdl events
      SDL_Event e;
      while (SDL_PollEvent(&e))
      {
        if (e.type == SDL_EVENT_QUIT) return;
      }
      Renderer_->Impl_Render();
    }
  }

  auto App::Impl_CreateWindow() -> void
  {
    // Create window
    SdlWindow_ = SDL_CreateWindow(
      "lxe demo", 1422, 800,
      SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN
      //| SDL_WINDOW_RESIZABLE
    );
    if (!SdlWindow_) throw std::runtime_error("Failed to create window");

    // Set min window size
    SDL_SetWindowMinimumSize(SdlWindow_, 1422 / 2, 800 / 2);
  }
}  // namespace lxe
