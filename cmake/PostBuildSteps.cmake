# Post-build steps configuration

# Copy assets folder to build directory
add_custom_command(
  TARGET ${TARGET}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:${TARGET}>/assets"
  COMMENT "Copying assets folder to build directory")

# Copy all runtime DLLs to the output directory (Windows only)
if(WIN32)
  add_custom_command(
    TARGET ${TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${TARGET}> $<TARGET_FILE_DIR:${TARGET}>
    COMMAND_EXPAND_LISTS
    COMMENT "Copying runtime DLLs to output directory")
endif()
