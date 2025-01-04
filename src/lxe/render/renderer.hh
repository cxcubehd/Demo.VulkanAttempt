#pragma once

#include <memory>
#include <span>
#include <tuple>
#include <vector>

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
    vk::Instance VkInstance_;

    vk::SurfaceKHR VkSurface_;

    vkb::PhysicalDevice VbPhysicalDevice_;
    vkb::Device VbDevice_;
    vk::Device VkDevice_;
    vkb::DispatchTable VbDispatchTable_;

    vkb::Swapchain VbSwapchain_;
    vk::SwapchainKHR VkSwapchain_;

    vk::Queue VkGraphicsQueue_;
    vk::Queue VkPresentQueue_;

    vk::RenderPass VkRenderPass_;

    vk::PipelineLayout VkPipelineLayout_;
    vk::Pipeline VkPipeline_;

    std::vector<vk::Image> VkSwapchainImages_;
    std::vector<vk::ImageView> VkSwapchainImageViews_;

    std::size_t VkFramebufferCount_{};
    std::vector<vk::Framebuffer> VkFramebuffers_;

    vk::CommandPool VkCommandPool_;
    std::vector<vk::CommandBuffer> VkCommandBuffers_;

    std::vector<vk::Semaphore> VkImageAvailableSemaphores_;
    std::vector<vk::Semaphore> VkRenderFinishedSemaphores_;
    std::vector<vk::Fence> VkInFlightFences_;
    std::vector<vk::Fence> VkInFlightImages_;

    std::size_t VkCurrentFrame_{};

    static constexpr std::size_t VkMaxFramesInFlight_ = 2;

    bool VkShouldResize_{};

   private:
    auto Impl_Init() -> void;

    auto Impl_InitInstance() -> void;
    auto Impl_InitSurface() -> void;
    auto Impl_InitDevice() -> void;
    auto Impl_InitSwapchain() -> void;
    auto Impl_InitGetQueues() -> void;

    auto Impl_InitRenderPass() -> void;
    auto Impl_InitGraphicsPipeline() -> void;

    auto Impl_InitFramebuffers() -> void;
    auto Impl_InitCommandPool() -> void;
    auto Impl_InitCommandBuffers() -> void;
    auto Impl_InitSync() -> void;

    [[nodiscard]] auto Impl_CreateShaderModule(std::span<const char> data) const
      -> vk::ShaderModule;

    auto Impl_RecordCommandBuffers() -> void;

    auto Impl_Render() -> void;

    auto Impl_MaybeResize() -> void;

    auto Impl_RecreateSwapchain() -> void;

   public:
    auto Init() -> void;

   public:
    static auto GetSdlVkInstanceExtensions()
      -> std::tuple<char const* const*, std::size_t>;

   public:
    static auto Make(App& app) -> std::unique_ptr<Renderer>;
  };
}  // namespace lxe
