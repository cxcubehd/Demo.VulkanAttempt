# battery embed
CPMAddPackage(
        NAME b_embed
        GITHUB_REPOSITORY batterycenter/embed
        GIT_TAG v1.2.19
        GIT_SHALLOW
        OPTIONS
        "B_PRODUCTION_MODE ON"
)

# glm
CPMAddPackage("gh:g-truc/glm#1.0.3")
target_link_libraries(app PUBLIC glm::glm)

# JoltPhysics
CPMAddPackage(
        NAME JoltPhysics
        GITHUB_REPOSITORY jrouwe/JoltPhysics
        GIT_TAG v5.5.0
        SOURCE_SUBDIR Build
        OPTIONS
        "TARGET_UNIT_TESTS OFF"
        "TARGET_HELLO_WORLD OFF"
        "TARGET_PERFORMANCE_TEST OFF"
        "TARGET_SAMPLES OFF"
        "TARGET_VIEWER OFF"
        "INTERPROCEDURAL_OPTIMIZATION OFF"
)
target_link_libraries(app PUBLIC Jolt)

# SDL3
CPMAddPackage(
        NAME SDL
        GITHUB_REPOSITORY libsdl-org/SDL
        GIT_TAG release-3.4.2
        GIT_SHALLOW
        OPTIONS
        "SDL_TEST_LIBRARY OFF"
        "SDL_STATIC ON"
)
target_link_libraries(app PUBLIC SDL3::SDL3-static)

# Vulkan
CPMAddPackage(
        NAME Vulkan
        GITHUB_REPOSITORY KhronosGroup/Vulkan-Hpp
        GIT_TAG v1.4.346
        GIT_SHALLOW
        OPTIONS
        "VULKAN_HPP_NO_CONSTRUCTORS OFF"
        "VULKAN_HPP_ENABLE_CPP20_MODULES ON"
        "VULKAN_HPP_CPP20_MODULE_DYNAMIC_DISPATCHER ON"
)
target_compile_definitions(VulkanHppModule PUBLIC VULKAN_HPP_NO_CONSTRUCTORS)
target_link_libraries(app PUBLIC VulkanHppModule)

# vk-bootstrap
if (Vulkan_ADDED)
    set(VK_BOOTSTRAP_VULKAN_HEADER_DIR "${Vulkan_SOURCE_DIR}/Vulkan-Headers/include")
endif ()

CPMAddPackage(
        NAME vk-bootstrap
        GITHUB_REPOSITORY charles-lunarg/vk-bootstrap
        GIT_TAG v1.4.345
        GIT_SHALLOW
)
target_link_libraries(app PUBLIC vk-bootstrap::vk-bootstrap)

# VulkanMemoryAllocator
CPMAddPackage(
        NAME VulkanMemoryAllocator
        GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG v3.3.0
        GIT_SHALLOW
)
