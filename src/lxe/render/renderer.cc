#include <stdexcept>

#include <SDL3/SDL_vulkan.h>

#include "renderer.hh"

namespace lxe
{
  Renderer::Renderer(Secret, App* app) : App_{app}
  {
    // constructor
  }

  auto Renderer::Init() -> void
  {
    // TODO: Check if init

    Impl_Init();

    Impl_Render();

    return;
  }

  auto Renderer::GetSdlVkInstanceExtensions()
    -> std::tuple<char const* const*, std::size_t>
  {
    std::uint32_t vkExtensionCount{};
    const auto vkExtensions =
      SDL_Vulkan_GetInstanceExtensions(&vkExtensionCount);

    if (!vkExtensions)
      throw std::runtime_error("Failed to get SDL vk instance extensions");

    return {vkExtensions, vkExtensionCount};
  }
  auto Renderer::Make(App& app) -> std::unique_ptr<Renderer>
  {
    return std::make_unique<Renderer>(Secret{}, &app);
  }
}  // namespace lxe
