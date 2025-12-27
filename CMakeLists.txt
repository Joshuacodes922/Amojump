cmake_minimum_required(VERSION 3.8)
project(VulkanEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



# Enable MSVC hot reload
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Find Vulkan
find_package(Vulkan REQUIRED)

set(SLANG_ROOT "C:/Users/JoshuaS/source/repos/Vulkan Engine Cmake Build/dependencies/slang-2025.23.1-windows-x86_64")
set(SLANGC "${SLANG_ROOT}/bin/slangc.exe")

# Add your source files
add_executable(VulkanEngine VulkanEngine.cpp
main.cpp 
swapchain.cpp
pipeline.cpp
buffer.cpp
descriptor.cpp
texture.cpp
sampler.cpp
gameobject.cpp
TextureManager.cpp
meshManager.cpp
Scene.cpp
collisions.cpp
inputManager.cpp
proceduralGeneration.cpp
camera.cpp
scoreManager.cpp
)

# Include directories
target_include_directories(VulkanEngine PRIVATE ${Vulkan_INCLUDE_DIRS} dependencies)
target_include_directories(VulkanEngine PRIVATE dependencies/glm-master)
target_include_directories(VulkanEngine PRIVATE ${SLANG_ROOT}/include)
target_include_directories(VulkanEngine PRIVATE dependencies/nlohmann)
target_include_directories(VulkanEngine PRIVATE external/stb)

add_library(VulkanCppModule)
add_library(Vulkan::cppm ALIAS VulkanCppModule)

target_compile_definitions(VulkanCppModule PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_BINARY_DIR})



target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")

target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)

set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

target_sources(VulkanCppModule
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${Vulkan_INCLUDE_DIR}"
        FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
)

# Add GLFW as a subdirectory
add_subdirectory(dependencies/glfw-3.4)
target_link_libraries(VulkanEngine PRIVATE glfw ${Vulkan_LIBRARIES} ${SLANG_ROOT}/lib/slang.lib)

# -----------------------------
# Shader compilation function
# -----------------------------
# Copy all shaders to the build directory
file(GLOB SHADERS "shaders/*.spv")
foreach(SHADER ${SHADERS})
    add_custom_command(TARGET VulkanEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER} $<TARGET_FILE_DIR:VulkanEngine>/shaders)
endforeach()


add_custom_target(Shaders ALL DEPENDS ${SPIRV_SHADERS})

add_dependencies(VulkanEngine Shaders)

# Use C++20
set_property(TARGET VulkanEngine PROPERTY CXX_STANDARD 20)
