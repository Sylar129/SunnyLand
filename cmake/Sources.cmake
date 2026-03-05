# Source files configuration

set(TARGET_SOURCES
    src/engine/component/component.h
    src/engine/core/config.cpp
    src/engine/core/config.h
    src/engine/core/context.cpp
    src/engine/core/context.h
    src/engine/core/game_app.cpp
    src/engine/core/game_app.h
    src/engine/core/time.cpp
    src/engine/core/time.h
    src/engine/input/input_manager.cpp
    src/engine/input/input_manager.h
    src/engine/object/game_object.cpp
    src/engine/object/game_object.h
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

set(TARGET_INCLUDE_DIRS
    src
)
