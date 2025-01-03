#include <iostream>
#include <print>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>

#include "re.hh"

namespace lxe
{
  auto Entry() -> void;
}

[[noreturn]]
auto main() -> int
{
  try
  {
    lxe::Entry();
  }
  catch (const std::exception& e)
  {
    std::println(
      std::cerr, "Application has thrown the following exception:\n\t{}",
      e.what()
    );
    std::exit(1);
  }

  std::unreachable();
}

// ReSharper disable once CppDFAConstantFunctionResult
auto main_2() -> int
{
  using namespace re;

  std::println("Starting!");

  auto ctx = Ctx();

  // init sdl
  if (!SDL_Init(SDL_INIT_VIDEO))
    throw std::runtime_error("Failed to initialize SDL");
  if (!SDL_Vulkan_LoadLibrary(nullptr))
    throw std::runtime_error("Failed to load Vulkan");

  // create window
  ctx.s_window = SDL_CreateWindow(
    "app demo", 1422, 800,
    SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
  );
  if (!ctx.s_window) throw std::runtime_error("Failed to create window");

  SDL_SetWindowMinimumSize(ctx.s_window, 1422 / 2, 800 / 2);

  make_vk_instance(ctx);
  make_vk_suface(ctx);
  make_device(ctx);

  // Get the graphics queue with a helper function
  auto graphics_queue_ret = ctx.vb_device.get_queue(vkb::QueueType::graphics);
  if (!graphics_queue_ret)
  {
    std::println(
      std::cerr, "Failed to get graphics queue. Error: {}",
      graphics_queue_ret.error().message()
    );
    return false;
  }
  auto graphics_queue = vk::Queue(graphics_queue_ret.value());

  // show the window
  SDL_ShowWindow(ctx.s_window);

  // window loop
  {
    while (true)
    {
      SDL_Event windowEvent;
      while (SDL_PollEvent(&windowEvent))
      {
        if (windowEvent.type == SDL_EVENT_QUIT)
        {
          goto jmp_quit;
        }
      }
    }
  jmp_quit:
  {
  }
  }

  // uninitialize SDL
  SDL_DestroyWindow(ctx.s_window);
  SDL_Vulkan_UnloadLibrary();
  SDL_Quit();

  std::println("Exiting!");

  // exit
  return 0;
}
