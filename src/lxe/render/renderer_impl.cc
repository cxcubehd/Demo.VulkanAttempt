#include <format>
#include <stdexcept>

#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include <lxe/app/app.hh>

#include "renderer.hh"

namespace lxe
{
  auto Renderer::Impl_Init() -> void
  {
    // Create instance
    Impl_CreateInstance();

    // Create surface
    Impl_CreateSurface();

    // Select physical device
    Impl_SelectPhysicalDevice();

    // Get device
    Impl_GetDevice();
  }

  auto Renderer::Impl_CreateInstance() -> void
  {
    const auto instanceExt = GetSdlVkInstanceExtensions();

    vkb::InstanceBuilder builder;
    const auto instanceRes =
      builder.set_app_name("lxe")
        .enable_validation_layers()
        .use_default_debug_messenger()
        .enable_extensions(std::get<1>(instanceExt), std::get<0>(instanceExt))
        .build();

    if (!instanceRes)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to create instance: {}",
          instanceRes.error().message()));

    VbInstance_ = instanceRes.value();
  }

  auto Renderer::Impl_CreateSurface() -> void
  {
    VkSurfaceKHR surface{};
    if (!SDL_Vulkan_CreateSurface(
          App_->SdlWindow_, VbInstance_.instance, nullptr, &surface))
      throw std::runtime_error("(Renderer) Failed to create surface!");

    VkSurface_ = surface;
  }

  auto Renderer::Impl_SelectPhysicalDevice() -> void
  {
    // select physical device
    auto phyDeviceSelector = vkb::PhysicalDeviceSelector(VbInstance_);
    auto physicalDeviceRes = phyDeviceSelector.set_surface(VkSurface_)
                               .set_minimum_version(1, 2)
                               .require_dedicated_transfer_queue()
                               .select();

    if (!physicalDeviceRes)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to select suitable physical device: {}",
          physicalDeviceRes.error().message()));

    VbPhysicalDevice_ = physicalDeviceRes.value();
  }

  auto Renderer::Impl_GetDevice() -> void
  {
    const auto deviceBuilder = vkb::DeviceBuilder(VbPhysicalDevice_);
    auto deviceRes = deviceBuilder.build();

    if (!deviceRes)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to create device: {}",
          deviceRes.error().message()));

    VbDevice_ = deviceRes.value();
    VkDevice_ = VbDevice_.device;
  }
}  // namespace lxe
