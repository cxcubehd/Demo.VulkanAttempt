#pragma once

#include <memory>
#include <tuple>

#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

namespace lxe
{
  class App;

  class Renderer
  {
    friend App;

    struct Secret
    {
    };

   public:
    explicit Renderer(Secret, App* app);
    ~Renderer() = default;

    Renderer(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept = delete;
    Renderer& operator=(const Renderer& other) = delete;
    Renderer& operator=(Renderer&& other) noexcept = delete;

   private:
    App* const App_;

   private:
    vkb::Instance VbInstance_;

    vk::SurfaceKHR VkSurface_;

    vkb::PhysicalDevice VbPhysicalDevice_;
    vkb::Device VbDevice_;
    vk::Device VkDevice_;

   private:
    auto Impl_Init() -> void;
    auto Impl_CreateInstance() -> void;
    auto Impl_CreateSurface() -> void;
    auto Impl_SelectPhysicalDevice() -> void;
    auto Impl_GetDevice() -> void;

   public:
    auto Init() -> void;

   public:
    static auto GetSdlVkInstanceExtensions()
      -> std::tuple<char const* const*, std::size_t>;

   public:
    static auto Make(App& app) -> std::unique_ptr<Renderer>;
  };
}  // namespace lxe
