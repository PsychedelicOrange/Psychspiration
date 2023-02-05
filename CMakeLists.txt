cmake_minimum_required(VERSION 3.15)
project(Psychspiration CXX)

set(VCPKG_LIBRARY_LINKAGE static)
include(${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()


# set(vcpkg "./vcpkg/scripts/buildsystems/vcpkg.cmake")
# if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${vcpkg}")
#   set(CMAKE_TOOLCHAIN_FILE "${vcpkg}"
#       CACHE FILEPATH "CMake toolchain file")
#   message(STATUS "vcpkg toolchain found: ${CMAKE_TOOLCHAIN_FILE}")
# endif()

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(gli CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

set(SOURCES
  include/include_psych/Aabb.h
  include/include_psych/Camera.h
  include/include_psych/Culling.h
  include/include_psych/EventHandler.h
  include/include_psych/FileIO.h
  include/include_psych/Frustum.h
  include/include_psych/Hdr.h
  include/include_psych/Light.h
  include/include_psych/Mesh.h
  include/include_psych/Model.h
  include/include_psych/ModelManager.h
  include/include_psych/Object.h
  include/include_psych/Physics.h
  include/include_psych/Scene.h
  include/include_psych/SceneLight.h
  include/include_psych/Settings.h
  include/include_psych/Shader.h
  include/include_psych/Skybox.h
  include/include_psych/State.h
  include/include_psych/Texture.h
  include/include_psych/TextureManager.h
  include/include_extern/TimerQueryAsync.h
  src/src_psych/Aabb.cpp
  src/src_psych/Camera.cpp
  src/src_psych/EventHandler.cpp
  src/src_psych/FileIO.cpp
  src/src_psych/Light.cpp
  src/src_psych/main.cpp
  src/src_psych/Mesh.cpp
  src/src_psych/Model.cpp
  src/src_psych/ModelManager.cpp
  src/src_psych/Object.cpp
  src/src_psych/Physics.cpp
  src/src_psych/Scene.cpp
  src/src_psych/Settings.cpp
  src/src_psych/Shader.cpp
  src/src_psych/Texture.cpp
  src/src_psych/TextureManager.cpp
  src/src_extern/TimerQueryAsyncz.cpp
  src/src_extern/glad.c
  
  )

# add the source files to the executable
# add_executable(Psychspiration ${subdir1Files} ${subdir2Files} ${subdir3Files} ${subdir4Files})
add_executable(Psychspiration ${SOURCES})

set_property(TARGET Psychspiration PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Psychspiration)

set_target_properties(Psychspiration PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Psychspiration PRIVATE include/include_extern/ include/include_psych/ ${BULLET_INCLUDE_DIR})

target_compile_features(Psychspiration PRIVATE cxx_std_17)

target_link_libraries(Psychspiration
  PRIVATE
    glad::glad
    imgui::imgui
    glfw
    glm::glm
    gli
    assimp::assimp
    LinearMath BulletCollision BulletDynamics BulletSoftBody
    nlohmann_json
    zstd
    basisu)

    
