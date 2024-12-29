#include <stdexcept>

#include <SDL3/SDL_vulkan.h>

#include "app.hh"
#include "lxe/render/renderer.hh"

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
  }

  auto App::Impl_CreateWindow() -> void
  {
    // Create window
    SdlWindow_ = SDL_CreateWindow(
      "app demo", 1422, 800,
      SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    if (!SdlWindow_) throw std::runtime_error("Failed to create window");

    // Set min window size
    SDL_SetWindowMinimumSize(SdlWindow_, 1422 / 2, 800 / 2);
  }
}  // namespace lxe
