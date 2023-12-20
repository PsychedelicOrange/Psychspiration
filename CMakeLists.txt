cmake_minimum_required(VERSION 3.15)
project(Psychspiration CXX C)

# set(VCPKG_LIBRARY_LINKAGE static)
# include(${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()


# set(vcpkg "./vcpkg/scripts/buildsystems/vcpkg.cmake")
# if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${vcpkg}")
#   set(CMAKE_TOOLCHAIN_FILE "${vcpkg}"
#       CACHE FILEPATH "CMake toolchain file")
#   message(STATUS "vcpkg toolchain found: ${CMAKE_TOOLCHAIN_FILE}")
# endif()


set(SOURCES
include/include_extern/glm/glm.hpp
include/include_extern/glad/glad.h
include/include_extern/KHR/khrplatform.h
include/include_psych/Aabb.h
include/include_psych/ASSIMPimporter.h
include/include_psych/BatchStatic.h
include/include_psych/Camera.h
include/include_psych/ConsoleDebug.h
include/include_psych/constants.h
include/include_psych/Controls.h
include/include_psych/Culling.h
include/include_psych/CullShape.h
include/include_psych/DebugDrawPhysics.h
include/include_psych/enums.h
include/include_psych/EventHandler.h
include/include_psych/Events.h
include/include_psych/FileIO.h
include/include_psych/Frustum.h
include/include_psych/GLI_load.h
include/include_psych/gui.h
include/include_psych/Hdr.h
include/include_psych/HullBuilder.h
include/include_psych/Hull.h
include/include_psych/Light.h
include/include_psych/Material.h
include/include_psych/Physics.h
include/include_psych/Player.h
include/include_psych/RawInput.h
include/include_psych/RCamera.h
include/include_psych/RCameraObject.h
include/include_psych/Renderer.h
include/include_psych/RenderState.h
include/include_psych/RInput.h
include/include_psych/RLightObjects.h
include/include_psych/RLights.h
include/include_psych/RLightTypes.h
include/include_psych/RMaterialBuilder.h
include/include_psych/RMeshBuilder.h
include/include_psych/RMesh.h
include/include_psych/RModelBuilder.h
include/include_psych/RModel.h
include/include_psych/RModelManager.h
include/include_psych/RObject.h
include/include_psych/R_Scene.h
include/include_psych/RSettings.h
include/include_psych/RTextureBuilder.h
include/include_psych/RTextureDirector.h
include/include_psych/RTexture.h
include/include_psych/RTextureManager.h
include/include_psych/RWindow.h
include/include_psych/SceneLight.h
include/include_psych/SettingsManager.h
include/include_psych/ShaderBuilder.h
include/include_psych/Shader.h
include/include_psych/Skybox.h
include/include_psych/State.h
include/include_psych/STBI_load.h
include/include_psych/Texture.h
include/include_psych/Buff.h

src/src_psych/Buff.cpp
src/src_psych/Aabb.cpp
src/src_psych/Camera.cpp
src/src_psych/ConsoleDebug.cpp
src/src_psych/EventHandler.cpp
src/src_psych/FileIO.cpp
src/src_psych/gui.cpp
src/src_psych/HullBuilder.cpp
src/src_psych/Light.cpp
# src/src_psych/main.cpp
# src/src_psych/Mesh.cpp
# src/src_psych/Model.cpp
# src/src_psych/ModelManager.cpp
# src/src_psych/Object.cpp
src/src_psych/Physics.cpp
src/src_psych/Player.cpp
src/src_psych/RawInput.cpp
src/src_psych/RCameraObject.cpp
src/src_psych/Renderer.cpp
src/src_psych/RInput.cpp
src/src_psych/Rmain.cpp
src/src_psych/RModelManager.cpp
src/src_psych/R_Scene.cpp
src/src_psych/RSettings.cpp
src/src_psych/RTextureBuilder.cpp
src/src_psych/RTextureManager.cpp
src/src_psych/RWindow.cpp
# src/src_psych/Scene.cpp
src/src_psych/SceneLight.cpp
src/src_psych/SettingsManager.cpp
src/src_psych/Shader.cpp
src/src_psych/stbi_defination.cpp
src/src_psych/Texture.cpp
# src/src_psych/TextureManager.cpp
src/src_extern/glad.c
src/src_extern/imgui.cpp
src/src_extern/imgui_demo.cpp
src/src_extern/imgui_draw.cpp
src/src_extern/imgui_stdlib.cpp
src/src_extern/imgui_tables.cpp
src/src_extern/imgui_widgets.cpp
src/src_extern/implot.cpp
src/src_extern/implot_demo.cpp
src/src_extern/implot_items.cpp
src/src_extern/imgui_impl_glfw.cpp
src/src_extern/imgui_impl_opengl3.cpp
  )

# add the source files to the executable
# add_executable(Psychspiration ${subdir1Files} ${subdir2Files} ${subdir3Files} ${subdir4Files})

add_executable(Psychspiration ${SOURCES})

set_property(TARGET Psychspiration PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Psychspiration)

set_target_properties(Psychspiration PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Psychspiration PRIVATE include/include_psych/ include/include_extern/ include/include_extern/imgui/ include/include_extern/imgui/backends/ /usr/local/include/bullet/ )

target_compile_features(Psychspiration PRIVATE cxx_std_17)

target_link_libraries(Psychspiration
  PRIVATE
#    glad
#    imgui
    dl
    glfw3
#    glm
#    gli
    BulletDynamics
    BulletCollision
    assimp
    LinearMath BulletCollision BulletDynamics BulletSoftBody
#    nlohmann_json
    zstd
    X11)

    
