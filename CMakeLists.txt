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

add_subdirectory(vendored/spdlog)
add_subdirectory(vendored/SDL)

set(SDLIMAGE_VENDORED OFF)
add_subdirectory(vendored/SDL_image)

set(SDLMIXER_VENDORED OFF)
set(SDLMIXER_VORBIS_STB OFF)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
add_subdirectory(vendored/SDL_mixer)

add_subdirectory(vendored/SDL_ttf)
add_subdirectory(vendored/glm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendored/json)

include_directories(src)

add_executable(${TARGET}
                src/engine/core/game_app.cpp
                src/engine/core/game_app.h
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