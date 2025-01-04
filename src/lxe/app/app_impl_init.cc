#include <print>
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

    // Sdl resize callback
    SDL_AddEventWatch(
      [](void* userdata, SDL_Event* event) -> bool
      {
        const auto app = static_cast<App*>(userdata);
        if (event->type == SDL_EVENT_WINDOW_RESIZED)
        {
          // resizing
          app->Renderer_->Impl_RecreateSwapchain();
          app->Renderer_->Impl_Render();
        }
        return true;
      },
      this
    );

    while (true)
    {
      // std::this_thread::sleep_for(std::chrono::seconds(1));
      //  handle sdl events
      SDL_Event e;
      while (SDL_PollEvent(&e))
      {
        if (e.type == SDL_EVENT_QUIT) return;
        // handle f11 fullscreen
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_F11)
        {
          // store global mouse pos
          auto [x, y] = std::tuple<float, float>();
          SDL_GetGlobalMouseState(&x, &y);

          if (const auto flags = SDL_GetWindowFlags(SdlWindow_);
              flags & SDL_WINDOW_FULLSCREEN)
            SDL_SetWindowFullscreen(SdlWindow_, false);
          else SDL_SetWindowFullscreen(SdlWindow_, SDL_WINDOW_FULLSCREEN);

          // restore global mouse pos
          SDL_WarpMouseGlobal(x, y);
        }
      }
      Renderer_->Impl_Render();
      Renderer_->Impl_MaybeResize();
    }
  }

  auto App::Impl_CreateWindow() -> void
  {
    // Create window
    SdlWindow_ = SDL_CreateWindow(
      "lxe demo", 1422, 800,
      SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
    );
    if (!SdlWindow_) throw std::runtime_error("Failed to create window");

    // Set min window size
    SDL_SetWindowMinimumSize(SdlWindow_, 1422 / 2, 800 / 2);
  }
}  // namespace lxe
