# Post-build steps configuration

# Copy assets folder to build directory
add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${TARGET}>/assets"
    COMMENT "Copying assets folder to build directory"
)
