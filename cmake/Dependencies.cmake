# Third-party dependencies configuration

# spdlog - logging library
add_subdirectory(third_party/spdlog)

# SDL3 - main graphics library
add_subdirectory(third_party/SDL)

# SDL3_image - image loading support
set(SDLIMAGE_VENDORED OFF)
add_subdirectory(third_party/SDL_image)

# SDL3_mixer - audio mixing library
set(SDLMIXER_VENDORED OFF)
set(SDLMIXER_VORBIS_STB OFF)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
add_subdirectory(third_party/SDL_mixer)

# SDL3_ttf - text rendering library
add_subdirectory(third_party/SDL_ttf)

# GLM - math library for graphics
add_subdirectory(third_party/glm)

# nlohmann_json - JSON library
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
add_subdirectory(third_party/json)
