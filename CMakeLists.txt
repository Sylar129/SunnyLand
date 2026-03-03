cmake_minimum_required(VERSION 3.16.0)
project(SunnyLand VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set(TARGET ${PROJECT_NAME})

add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/SDL)

set(SDLIMAGE_VENDORED OFF)
add_subdirectory(third_party/SDL_image)

set(SDLMIXER_VENDORED OFF)
set(SDLMIXER_VORBIS_STB OFF)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
add_subdirectory(third_party/SDL_mixer)

add_subdirectory(third_party/SDL_ttf)
add_subdirectory(third_party/glm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(third_party/json)

include_directories(src)

add_executable(${TARGET}
                src/engine/core/game_app.cpp
                src/engine/core/game_app.h
                src/engine/core/time.cpp
                src/engine/core/time.h
                src/engine/render/camera.cpp
                src/engine/render/camera.h
                src/engine/render/renderer.cpp
                src/engine/render/renderer.h
                src/engine/render/sprite.h
                src/engine/resource/audio_manager.cpp
                src/engine/resource/audio_manager.h
                src/engine/resource/font_manager.cpp
                src/engine/resource/font_manager.h
                src/engine/resource/resource_manager.cpp
                src/engine/resource/resource_manager.h
                src/engine/resource/texture_manager.cpp
                src/engine/resource/texture_manager.h
                src/engine/utils/math.h
                src/log.cpp
                src/log.h
                src/main.cpp
)

target_link_libraries(${TARGET}
                        PRIVATE
                        spdlog::spdlog
                        SDL3::SDL3
                        SDL3_image::SDL3_image
                        SDL3_mixer::SDL3_mixer
                        SDL3_ttf::SDL3_ttf
                        glm::glm
                        nlohmann_json::nlohmann_json
)