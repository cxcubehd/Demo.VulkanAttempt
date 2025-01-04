#include <array>
#include <format>

#include <vulkan/vulkan.hpp>

#include "renderer.hh"

namespace lxe
{
  auto Renderer::Impl_Render() -> void
  {
    const auto i = VkCurrentFrame_;

    if (VkDevice_.waitForFences(
          1, &VkInFlightFences_[i], true,
          std::numeric_limits<std::uint64_t>::max()
        ) != vk::Result::eSuccess)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to wait for fences: {}",
          static_cast<int>(VkDevice_.getFenceStatus(VkInFlightFences_[i]))
        )
      );

    std::uint32_t imageIndex{};

    const auto imageAcquireRes = VkDevice_.acquireNextImageKHR(
      VkSwapchain_, std::numeric_limits<std::uint64_t>::max(),
      VkImageAvailableSemaphores_[i], nullptr, &imageIndex
    );

    if (imageAcquireRes == vk::Result::eErrorOutOfDateKHR)
    {
      VkShouldResize_ = true;
      return;
    }

    if (imageAcquireRes != vk::Result::eSuccess &&
        imageAcquireRes != vk::Result::eSuboptimalKHR)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to acquire next image: {}",
          static_cast<int>(imageAcquireRes)
        )
      );

    if (VkInFlightImages_[imageIndex])
    {
      if (VkDevice_.waitForFences(
            1, &VkInFlightImages_[imageIndex], true,
            std::numeric_limits<std::uint64_t>::max()
          ) != vk::Result::eSuccess)
        throw std::runtime_error(
          "(Renderer) Failed to wait for in-flight image"
        );
    }
    VkInFlightImages_[imageIndex] = VkInFlightFences_[i];

    const auto waitSemaphores = std::to_array({VkImageAvailableSemaphores_[i]});
    const auto waitStages = std::to_array<vk::PipelineStageFlags>(
      {vk::PipelineStageFlagBits::eColorAttachmentOutput}
    );
    const auto signalSemaphores =
      std::to_array({VkRenderFinishedSemaphores_[i]});

    const auto submitInfo = vk::SubmitInfo{
      .waitSemaphoreCount = waitSemaphores.size(),
      .pWaitSemaphores = waitSemaphores.data(),
      .pWaitDstStageMask = waitStages.data(),
      .commandBufferCount = 1,
      .pCommandBuffers = &VkCommandBuffers_[imageIndex],
      .signalSemaphoreCount = signalSemaphores.size(),
      .pSignalSemaphores = signalSemaphores.data()
    };

    if (VkDevice_.resetFences(1, &VkInFlightFences_[i]) != vk::Result::eSuccess)
      throw std::runtime_error("(Renderer) Failed to reset in-flight fence");

    VkGraphicsQueue_.submit(submitInfo, VkInFlightFences_[i]);

    const auto presentInfo = vk::PresentInfoKHR{
      .waitSemaphoreCount = signalSemaphores.size(),
      .pWaitSemaphores = signalSemaphores.data(),
      .swapchainCount = 1,
      .pSwapchains = &VkSwapchain_,
      .pImageIndices = &imageIndex
    };

    const auto presentRes = VkPresentQueue_.presentKHR(presentInfo);

    if (presentRes == vk::Result::eErrorOutOfDateKHR ||
        presentRes == vk::Result::eSuboptimalKHR)
    {
      VkShouldResize_ = true;
      return;
    }

    if (presentRes != vk::Result::eSuccess)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to present image: {}",
          static_cast<int>(presentRes)
        )
      );

    VkCurrentFrame_ = (VkCurrentFrame_ + 1) % VkMaxFramesInFlight_;
  }
}  // namespace lxe
