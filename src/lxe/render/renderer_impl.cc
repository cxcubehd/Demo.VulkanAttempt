#include <format>
#include <ranges>

#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include <lxe/app/app.hh>

#include "renderer.hh"

namespace lxe
{
  auto Renderer::Impl_Init() -> void
  {
    // Init VulkanHpp
    {
      VULKAN_HPP_DEFAULT_DISPATCHER.init();
    }

    // Create instance
    Impl_InitInstance();

    // Create surface
    Impl_InitSurface();

    // Select physical device and get logical device
    Impl_InitDevice();

    // Create swap chain
    Impl_InitSwapchain();

    // Get queues
    Impl_InitGetQueues();

    // Create render pass
    Impl_InitRenderPass();

    // Create graphics pipeline
    Impl_InitGraphicsPipeline();

    // Create frame buffers
    Impl_InitFramebuffers();

    // Create command pool
    Impl_InitCommandPool();

    // Create command buffers
    Impl_InitCommandBuffers();

    // Record command buffers
    Impl_RecordCommandBuffers();

    // Init sync objects
    Impl_InitSync();
  }

  auto Renderer::Impl_InitInstance() -> void
  {
    const auto instanceExt = GetSdlVkInstanceExtensions();

    vkb::InstanceBuilder builder;
    const auto instanceRes =
      builder.set_app_name("lxe")
        .request_validation_layers()
        .use_default_debug_messenger()
        .enable_extensions(std::get<1>(instanceExt), std::get<0>(instanceExt))
        .build();

    if (!instanceRes)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to create instance: {}",
          instanceRes.error().message()
        )
      );

    VbInstance_ = instanceRes.value();
    VkInstance_ = VbInstance_.instance;

    // Init VulkanHpp
    VULKAN_HPP_DEFAULT_DISPATCHER.init(VkInstance_);
  }

  auto Renderer::Impl_InitSurface() -> void
  {
    VkSurfaceKHR surface{};
    if (!SDL_Vulkan_CreateSurface(
          App_->SdlWindow_, VbInstance_.instance, nullptr, &surface
        ))
      throw std::runtime_error("(Renderer) Failed to create surface!");

    VkSurface_ = surface;
  }

  auto Renderer::Impl_InitDevice() -> void
  {
    // Select physical device
    {
      auto phyDeviceSelector = vkb::PhysicalDeviceSelector(VbInstance_);
      auto physicalDeviceRes = phyDeviceSelector.set_surface(VkSurface_)
                                 .set_minimum_version(1, 2)
                                 .require_dedicated_transfer_queue()
                                 .select();

      if (!physicalDeviceRes)
        throw std::runtime_error(
          std::format(
            "(Renderer) Failed to select suitable physical device: {}",
            physicalDeviceRes.error().message()
          )
        );

      VbPhysicalDevice_ = physicalDeviceRes.value();
    }

    // Get logical device
    {
      const auto deviceBuilder = vkb::DeviceBuilder(VbPhysicalDevice_);
      auto deviceRes = deviceBuilder.build();

      if (!deviceRes)
        throw std::runtime_error(
          std::format(
            "(Renderer) Failed to create device: {}",
            deviceRes.error().message()
          )
        );

      VbDevice_ = deviceRes.value();
      VkDevice_ = VbDevice_.device;
    }

    // Init VulkanHpp
    VULKAN_HPP_DEFAULT_DISPATCHER.init(VkDevice_);

    // Get dispatch table
    VbDispatchTable_ = VbDevice_.make_table();
  }

  auto Renderer::Impl_InitSwapchain() -> void
  {
    const auto swapchainBuilder = vkb::SwapchainBuilder(VbDevice_);
    auto swapchainRes = swapchainBuilder.build();

    if (!swapchainRes)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to create swapchain: {}",
          swapchainRes.error().message()
        )
      );

    VbSwapchain_ = swapchainRes.value();
    VkSwapchain_ = VbSwapchain_.swapchain;
  }

  auto Renderer::Impl_InitGetQueues() -> void
  {
    // Get graphics queue
    auto graphicsQueueRes = VbDevice_.get_queue(vkb::QueueType::graphics);

    if (!graphicsQueueRes)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to get graphics queue: {}",
          graphicsQueueRes.error().message()
        )
      );

    VkGraphicsQueue_ = graphicsQueueRes.value();


    // Get present queue
    auto presentQueueRes = VbDevice_.get_queue(vkb::QueueType::present);

    if (!presentQueueRes)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to get present queue: {}",
          presentQueueRes.error().message()
        )
      );

    VkPresentQueue_ = presentQueueRes.value();
  }

  auto Renderer::Impl_InitFramebuffers() -> void
  {
    VkSwapchainImages_ = VkDevice_.getSwapchainImagesKHR(VkSwapchain_);

    auto imageViews = VbSwapchain_.get_image_views().value();
    VkSwapchainImageViews_ = {};
    VkSwapchainImageViews_.reserve(imageViews.size());
    std::ranges::transform(
      imageViews, std::back_inserter(VkSwapchainImageViews_),
      [](const auto& x) { return x; }
    );
    imageViews.clear();

    VkFramebufferCount_ = VkSwapchainImages_.size();
    VkFramebuffers_ = {};
    VkFramebuffers_.resize(VkFramebufferCount_);

    for (const auto i : std::views::iota(0u, VkFramebufferCount_))
    {
      const auto attachments = std::to_array({VkSwapchainImageViews_[i]});

      const auto framebuffer = vk::FramebufferCreateInfo{
        .renderPass = VkRenderPass_,
        .attachmentCount = attachments.size(),
        .pAttachments = attachments.data(),
        .width = VbSwapchain_.extent.width,
        .height = VbSwapchain_.extent.height,
        .layers = 1
      };

      VkFramebuffers_[i] = VkDevice_.createFramebuffer(framebuffer);
    }
  }

  auto Renderer::Impl_InitCommandPool() -> void
  {
    const auto pool = vk::CommandPoolCreateInfo{
      .queueFamilyIndex =
        VbDevice_.get_queue_index(vkb::QueueType::graphics).value()
    };

    VkCommandPool_ = VkDevice_.createCommandPool(pool);
  }

  auto Renderer::Impl_InitCommandBuffers() -> void
  {
    const auto allocInfo = vk::CommandBufferAllocateInfo{
      .commandPool = VkCommandPool_,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = static_cast<std::uint32_t>(VkFramebufferCount_)
    };

    VkCommandBuffers_ = VkDevice_.allocateCommandBuffers(allocInfo);
  }

  auto Renderer::Impl_InitSync() -> void
  {
    VkImageAvailableSemaphores_ = {};
    VkRenderFinishedSemaphores_ = {};
    VkInFlightFences_ = {};
    VkInFlightImages_ = {};

    VkImageAvailableSemaphores_.resize(VkMaxFramesInFlight_);
    VkRenderFinishedSemaphores_.resize(VkMaxFramesInFlight_);
    VkInFlightFences_.resize(VkMaxFramesInFlight_);
    VkInFlightImages_.resize(VkMaxFramesInFlight_);

    constexpr auto semaphoreInfo = vk::SemaphoreCreateInfo{};

    constexpr auto fenceInfo =
      vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled};

    for (const auto i : std::views::iota(0u, VkMaxFramesInFlight_))
    {
      VkImageAvailableSemaphores_[i] = VkDevice_.createSemaphore(semaphoreInfo);
      VkRenderFinishedSemaphores_[i] = VkDevice_.createSemaphore(semaphoreInfo);
      VkInFlightFences_[i] = VkDevice_.createFence(fenceInfo);
    }
  }

  auto Renderer::Impl_CreateShaderModule(std::span<const char> data) const
    -> vk::ShaderModule
  {
    const auto createInfo = vk::ShaderModuleCreateInfo{
      .codeSize = data.size(),
      .pCode = reinterpret_cast<const std::uint32_t*>(data.data())
    };

    return VkDevice_.createShaderModule(createInfo);
  }

  auto Renderer::Impl_RecordCommandBuffers() -> void
  {
    for (const auto i : std::views::iota(0u, VkFramebufferCount_))
    {
      const auto& commandBuffer = VkCommandBuffers_[i];

      commandBuffer.begin(vk::CommandBufferBeginInfo{});

      constexpr auto clearValue = vk::ClearValue{
        .color = vk::ClearColorValue({{0.0f, 0.0f, 1.0f, 1.0f}})
      };

      const auto renderPassBeginInfo = vk::RenderPassBeginInfo{
        .renderPass = VkRenderPass_,
        .framebuffer = VkFramebuffers_[i],
        .renderArea =
          vk::Rect2D{
            .offset = {0, 0},
            .extent = {VbSwapchain_.extent.width, VbSwapchain_.extent.height}
          },
        .clearValueCount = 1,
        .pClearValues = &clearValue
      };

      const auto viewport = vk::Viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(VbSwapchain_.extent.width),
        .height = static_cast<float>(VbSwapchain_.extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
      };

      const auto scissors = vk::Rect2D{
        .offset = {},
        .extent = {VbSwapchain_.extent.width, VbSwapchain_.extent.height}
      };

      commandBuffer.setViewport(0, 1, &viewport);
      commandBuffer.setScissor(0, 1, &scissors);

      commandBuffer.beginRenderPass(
        renderPassBeginInfo, vk::SubpassContents::eInline
      );

      commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, VkPipeline_);

      commandBuffer.draw(3, 1, 0, 0);

      commandBuffer.endRenderPass();

      commandBuffer.end();
    }
  }
}  // namespace lxe
