#pragma once

#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>
#include <VkBootstrap.h>

namespace re
{
  struct Ctx
  {
    SDL_Window* s_window;
    vkb::Instance vb_instance;
    VkSurfaceKHR v_surface;
    vkb::Device vb_device;
    vk::Device v_device;
  };

  auto make_vk_instance(Ctx& ctx) -> void;

  auto make_vk_suface(Ctx& ctx) -> void;

  auto make_device(Ctx& ctx) -> void;
}  // namespace re
