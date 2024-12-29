#include <iostream>
#include <print>

#include <vulkan/vulkan.hpp>

#include <SDL3/SDL_vulkan.h>

#include "re.hh"

namespace re
{

  auto make_vk_instance(Ctx& ctx) -> void
  {
    // get vulkan extensions
    std::uint32_t s_vk_instance_extension_count{};
    const auto s_vk_instance_extensions =
      SDL_Vulkan_GetInstanceExtensions(&s_vk_instance_extension_count);

    // make vulkan instance
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Example Vulkan Application")
                      .request_validation_layers()
                      .use_default_debug_messenger()
                      .enable_extensions(
                        s_vk_instance_extension_count, s_vk_instance_extensions)
                      .build();
    if (!inst_ret)
    {
      std::println(
        std::cerr, "Failed to create Vulkan instance. Error: {}",
        inst_ret.error().message());
      throw std::exception();
    }
    ctx.vb_instance = inst_ret.value();
  }

  auto make_vk_suface(Ctx& ctx) -> void
  {
    // create vulkan surface
    if (!SDL_Vulkan_CreateSurface(
          ctx.s_window, ctx.vb_instance.instance, nullptr, &ctx.v_surface))
      throw std::runtime_error("Failed to create Vulkan surface!");
  }

  auto make_device(Ctx& ctx) -> void
  {
    // select physical device
    auto selector = vkb::PhysicalDeviceSelector(ctx.vb_instance);
    auto phys_ret = selector.set_surface(ctx.v_surface)
                      .set_minimum_version(1, 2)
                      .require_dedicated_transfer_queue()
                      .select();
    if (!phys_ret)
    {
      std::println(
        std::cerr, "Failed to select Vulkan Physical Device. Error: {}",
        phys_ret.error().message());
      throw std::exception();
    }

    auto device_builder = vkb::DeviceBuilder(phys_ret.value());

    // automatically propagate needed data from instance & physical device
    auto dev_ret = device_builder.build();
    if (!dev_ret)
    {
      std::println(
        std::cerr, "Failed to create Vulkan device. Error: {}",
        dev_ret.error().message());
      throw std::exception();
    }
    ctx.vb_device = dev_ret.value();

    // Get the VkDevice handle used in the rest of a vulkan application
    ctx.v_device = vk::Device(ctx.vb_device.device);
  }

}  // namespace re
