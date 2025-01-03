# battery embed
CPMAddPackage("gh:batterycenter/embed@1.2.19")


# glm
find_package(glm CONFIG REQUIRED)
target_link_libraries(app PUBLIC glm::glm)

# SDL3
CPMAddPackage(
        NAME SDL
        URL https://github.com/libsdl-org/SDL/archive/b2178857dcd6c4ba230266c92da36ef288ba1837.zip
        OPTIONS
        "SDL_TEST_LIBRARY OFF"
        "SDL_STATIC ON"
)
target_link_libraries(app PUBLIC SDL3::SDL3-static)

# Vulkan
CPMAddPackage(
        NAME Vulkan
        GITHUB_REPOSITORY KhronosGroup/Vulkan-Hpp
        GIT_TAG v1.4.303
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
message("Headers: ${VK_BOOTSTRAP_VULKAN_HEADER_DIR}")

CPMAddPackage("gh:charles-lunarg/vk-bootstrap#2e7c8e8323c0fe7efc96dc241610fcba35d8f530")
target_link_libraries(app PUBLIC vk-bootstrap::vk-bootstrap)

# VulkanMemoryAllocator
CPMAddPackage("gh:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator#3bab6924988e5f19bf36586a496156cf72f70d9f")
#target_link_libraries(app PUBLIC GPUOpen::VulkanMemoryAllocator)