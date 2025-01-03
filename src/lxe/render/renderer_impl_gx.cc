#include <array>
#include <format>

#include <vulkan/vulkan.hpp>

#include <lxe_assets/shaders.hh>

#include "renderer.hh"

namespace lxe
{
  auto Renderer::Impl_InitRenderPass() -> void
  {
    const auto colorAttachment = vk::AttachmentDescription{
      .format = static_cast<vk::Format>(VbSwapchain_.image_format),
      .samples = vk::SampleCountFlagBits::e1,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::ePresentSrcKHR
    };

    constexpr auto colorAttachmentRef = vk::AttachmentReference{
      .attachment = 0, .layout = vk::ImageLayout::eColorAttachmentOptimal
    };

    const auto subpass = vk::SubpassDescription{
      .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentRef
    };

    constexpr auto dependency = vk::SubpassDependency{
      .srcSubpass = vk::SubpassExternal,
      .dstSubpass = 0,
      .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
      .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
      .srcAccessMask = vk::AccessFlagBits::eNone,
      .dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
        vk::AccessFlagBits::eColorAttachmentWrite
    };

    const auto renderPassInfo = vk::RenderPassCreateInfo{
      .attachmentCount = 1,
      .pAttachments = &colorAttachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency
    };

    VkRenderPass_ = VkDevice_.createRenderPass(renderPassInfo);
    // if (VbDispatchTable_.createRenderPass(&createInfo, nullptr,
    //       reinterpret_cast<VkRenderPass*>(&VkRenderPass_)) != VK_SUCCESS)
    //   throw std::runtime_error("(Renderer) Failed to create render pass!");
  }

  auto Renderer::Impl_InitGraphicsPipeline() -> void
  {
    // TODO: load SPIR-V shaders
    // vertex
    // fragment

    const auto shaderModuleVertex = Impl_CreateShaderModule(assets::Asset_Shader_Vertex_Spv());
    const auto shaderModuleFragment = Impl_CreateShaderModule(assets::Asset_Shader_Fragment_Spv());

    const auto shaderStageVertex = vk::PipelineShaderStageCreateInfo{
      .stage = vk::ShaderStageFlagBits::eVertex,
      .module = shaderModuleVertex,
      .pName = "main"
    };

    const auto shaderStageFragment = vk::PipelineShaderStageCreateInfo{
      .stage = vk::ShaderStageFlagBits::eFragment,
      .module = shaderModuleFragment,
      .pName = "main"
    };

    const auto shaderStages = std::to_array({
      shaderStageVertex,
      shaderStageFragment,
    });

    constexpr auto vertexInputStage = vk::PipelineVertexInputStateCreateInfo{
      .vertexBindingDescriptionCount = 0, .vertexAttributeDescriptionCount = 0
    };

    constexpr auto inputAssemblyStage =
      vk::PipelineInputAssemblyStateCreateInfo{
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = false
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
      .offset = {0, 0},
      .extent = {VbSwapchain_.extent.width, VbSwapchain_.extent.height}
    };

    const auto viewportState = vk::PipelineViewportStateCreateInfo{
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissors
    };

    constexpr auto rasterizationState =
      vk::PipelineRasterizationStateCreateInfo{
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = false,
        .depthBiasConstantFactor = {},
        .depthBiasClamp = {},
        .depthBiasSlopeFactor = {},
        .lineWidth = 1.0f
      };

    constexpr auto multisampleState = vk::PipelineMultisampleStateCreateInfo{
      .rasterizationSamples = vk::SampleCountFlagBits::e1,
      .sampleShadingEnable = false
    };

    constexpr auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{
      .blendEnable = false,
      .srcColorBlendFactor = {},
      .dstColorBlendFactor = {},
      .colorBlendOp = {},
      .srcAlphaBlendFactor = {},
      .dstAlphaBlendFactor = {},
      .alphaBlendOp = {},
      .colorWriteMask = vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA
    };

    const auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{
      .logicOpEnable = false,
      .logicOp = vk::LogicOp::eCopy,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment
    };

    constexpr auto layout = vk::PipelineLayoutCreateInfo();

    VkPipelineLayout_ = VkDevice_.createPipelineLayout(layout);

    constexpr auto dynamicStates =
      std::to_array({vk::DynamicState::eViewport, vk::DynamicState::eScissor});

    const auto dynamicState = vk::PipelineDynamicStateCreateInfo{
      .dynamicStateCount = dynamicStates.size(),
      .pDynamicStates = dynamicStates.data()
    };

    const auto pipeline = vk::GraphicsPipelineCreateInfo{
      .stageCount = shaderStages.size(),
      .pStages = shaderStages.data(),
      .pVertexInputState = &vertexInputStage,
      .pInputAssemblyState = &inputAssemblyStage,
      .pTessellationState = {},
      .pViewportState = &viewportState,
      .pRasterizationState = &rasterizationState,
      .pMultisampleState = &multisampleState,
      .pDepthStencilState = {},
      .pColorBlendState = &colorBlendState,
      .pDynamicState = &dynamicState,
      .layout = VkPipelineLayout_,
      .renderPass = VkRenderPass_,
      .subpass = 0,
      .basePipelineHandle = {},
      .basePipelineIndex = {}
    };

    const auto pipelineResult =
      VkDevice_.createGraphicsPipeline(nullptr, pipeline);

    if (pipelineResult.result != vk::Result::eSuccess)
      throw std::runtime_error(
        std::format(
          "(Renderer) Failed to create graphics pipeline: {}",
          static_cast<int>(pipelineResult.result)
        )
      );

    VkPipeline_ = pipelineResult.value;

    // TODO: Destroy shader module
  }
}  // namespace lxe
