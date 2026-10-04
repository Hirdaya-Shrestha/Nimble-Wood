cmake_minimum_required(VERSION 3.16)

project(NimbleWood VERSION 0.0.2 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# MSVC runtime
# ---------------------------------------------------------------------------
# Statically link the MSVC runtime so the .exe runs without the
# VC++ redistributable.
#
# Must be set before SDL is added so SDL uses the same runtime.
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY
      "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ---------------------------------------------------------------------------
# SDL3
#
# Android : SDL3 comes from the SDL .aar (prefab) added by the Gradle
#           project in android/.
#
# Default : use system SDL3 if installed and fall back to building
#           the pinned version from source.
#
# ON      : always build SDL3 from source and link it statically.
#           Used by CI. iOS and web always do this.
# ---------------------------------------------------------------------------
option(
  NIMBLE_BUNDLE_SDL
  "Build SDL3 from source and link statically"
  OFF
)

set(
  NIMBLE_SDL_TAG
  "release-3.4.18"
  CACHE STRING
  "SDL3 git tag used when bundling"
)


set(
  NIMBLE_SDL_IMAGE_TAG
  "release-3.4.6"
  CACHE STRING
  "SDL3_image git tag used when bundling"
)

if(ANDROID)
  find_package(SDL3 REQUIRED CONFIG)
  find_package(SDL3_image REQUIRED CONFIG)
elseif(NOT NIMBLE_BUNDLE_SDL AND NOT IOS AND NOT EMSCRIPTEN)
  find_package(SDL3 3.2 CONFIG QUIET)
  if(SDL3_FOUND)
    find_package(SDL3_image 3.2 CONFIG QUIET)
  endif()
endif()

# ---------------------------------------------------------------------------
# Build SDL3 from source when required
# ---------------------------------------------------------------------------
if(NOT SDL3_FOUND)

  message(
    STATUS
    "Building SDL3 (${NIMBLE_SDL_TAG}) from source, static"
  )

  include(FetchContent)

  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_TESTS OFF CACHE BOOL "" FORCE)
  set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        ${NIMBLE_SDL_TAG}
    GIT_SHALLOW    TRUE
  )

  FetchContent_MakeAvailable(SDL3)

else()

  message(
    STATUS
    "Using SDL3 ${SDL3_VERSION}"
  )

endif()


if(NOT SDL3_image_FOUND)
  message(
    STATUS
    "Building SDL3_image (${NIMBLE_SDL_IMAGE_TAG}) from source, static"
  )
  include(FetchContent)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(SDL3IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
  set(SDL3IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
  # PNG and JPEG use the built-in stb_image backend (no extra libraries needed).
  # The heavy optional formats are disabled; enable them here if you need them.
  set(SDLIMAGE_VENDORED OFF CACHE BOOL "" FORCE)
  set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
  set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
  set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
  set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG        ${NIMBLE_SDL_IMAGE_TAG}
    GIT_SHALLOW    TRUE
  )
  FetchContent_MakeAvailable(SDL3_image)
else()
  message(STATUS "Using SDL3_image ${SDL3_image_VERSION}")
endif()

# ---------------------------------------------------------------------------
# Nimble Wood sources
# ---------------------------------------------------------------------------
set(NIMBLE_SOURCES

  # SDL entry point
  src/main.cpp

  # Core
  src/core/App.cpp

  # Entities
  src/entities/Player.cpp

  # Input
  src/input/Input.cpp

  # Utils
  src/utils/DrawCircle.cpp

  # Rendering
  src/rendering/Renderer.cpp
  src/rendering/Splash.cpp

)

# ---------------------------------------------------------------------------
# Game target
# ---------------------------------------------------------------------------
if(ANDROID)

  # SDLActivity loads libmain.so, so the game must be a shared library
  # named "main".
  add_library(
    NimbleWood
    SHARED
    ${NIMBLE_SOURCES}
  )

  set_target_properties(
    NimbleWood
    PROPERTIES
      OUTPUT_NAME main
  )

elseif(WIN32)

  enable_language(RC)

  add_executable(
    NimbleWood
    ${NIMBLE_SOURCES}
    platform/windows/app.rc
  )

  target_include_directories(
    NimbleWood
    PRIVATE
      platform/windows
  )

else()

  add_executable(
    NimbleWood
    ${NIMBLE_SOURCES}
  )

endif()

# ---------------------------------------------------------------------------
# Include directories
# ---------------------------------------------------------------------------
#
# This allows includes such as:
#
#   #include "core/App.hpp"
#   #include "entities/Player.hpp"
#   #include "input/Input.hpp"
#   #include "rendering/Renderer.hpp"
#
target_include_directories(
  NimbleWood
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------------------------------------------------------------------------
# Compile definitions
# ---------------------------------------------------------------------------
target_compile_definitions(
  NimbleWood
  PRIVATE
    NIMBLE_VERSION="${PROJECT_VERSION}"
)

# ---------------------------------------------------------------------------
# Libraries
# ---------------------------------------------------------------------------
target_link_libraries(
  NimbleWood
  PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------
if(MSVC)

  target_compile_options(
    NimbleWood
    PRIVATE
      /W4
  )

else()

  target_compile_options(
    NimbleWood
    PRIVATE
      -Wall
      -Wextra
  )

endif()

# ---------------------------------------------------------------------------
# Platform-specific configuration
# ---------------------------------------------------------------------------

if(WIN32)

  # GUI application: no console window.
  # SDL_main.h provides the Windows entry point.
  set_target_properties(
    NimbleWood
    PROPERTIES
      WIN32_EXECUTABLE ON
  )

  if(MINGW)

    # Avoid requiring libstdc++, libgcc and winpthread DLLs
    # next to the executable.
    target_link_options(
      NimbleWood
      PRIVATE
        -static
    )

  endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

  # Reduce runtime dependencies on the target Linux distribution.
  target_link_options(
    NimbleWood
    PRIVATE
      -static-libstdc++
      -static-libgcc
  )

elseif(IOS)

  set_target_properties(
    NimbleWood
    PROPERTIES

      MACOSX_BUNDLE TRUE

      MACOSX_BUNDLE_INFO_PLIST
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Info.plist.in"

      MACOSX_BUNDLE_GUI_IDENTIFIER
        "dev.nimblewood.game"

      MACOSX_BUNDLE_BUNDLE_NAME
        "Nimble Wood"

      MACOSX_BUNDLE_BUNDLE_VERSION
        "${PROJECT_VERSION}"

      MACOSX_BUNDLE_SHORT_VERSION_STRING
        "${PROJECT_VERSION}"

      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER
        "dev.nimblewood.game"

      XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY
        "1,2"

      XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME
        "AppIcon"
  )

  # App icon asset catalog.
  set(
    NIMBLE_IOS_ICONS
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Assets.xcassets"
  )

  target_sources(
    NimbleWood
    PRIVATE
      "${NIMBLE_IOS_ICONS}"
  )

  set_source_files_properties(
    "${NIMBLE_IOS_ICONS}"
    PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources
  )

elseif(APPLE)

  # macOS application bundle.
  set(
    NIMBLE_MAC_ICON
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/nimblewood.icns"
  )

  target_sources(
    NimbleWood
    PRIVATE
      "${NIMBLE_MAC_ICON}"
  )

  set_source_files_properties(
    "${NIMBLE_MAC_ICON}"
    PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources
  )

  set_target_properties(
    NimbleWood
    PROPERTIES

      MACOSX_BUNDLE TRUE

      MACOSX_BUNDLE_INFO_PLIST
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/Info.plist.in"

      MACOSX_BUNDLE_ICON_FILE
        "nimblewood.icns"

      MACOSX_BUNDLE_GUI_IDENTIFIER
        "dev.nimblewood.game"

      MACOSX_BUNDLE_BUNDLE_NAME
        "Nimble Wood"

      MACOSX_BUNDLE_BUNDLE_VERSION
        "${PROJECT_VERSION}"

      MACOSX_BUNDLE_SHORT_VERSION_STRING
        "${PROJECT_VERSION}"
  )

elseif(EMSCRIPTEN)

  # Produces:
  #
  #   index.html
  #   index.js
  #   index.wasm
  #   favicon.png
  #
  set_target_properties(
    NimbleWood
    PROPERTIES
      OUTPUT_NAME index
      SUFFIX ".html"
  )

  target_link_options(
    NimbleWood
    PRIVATE
      -sALLOW_MEMORY_GROWTH=1
  )

  add_custom_command(
    TARGET NimbleWood
    POST_BUILD

    COMMAND
      ${CMAKE_COMMAND}
      -DHTML=$<TARGET_FILE:NimbleWood>
      -P
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/web_postprocess.cmake"

    COMMAND
      ${CMAKE_COMMAND}
      -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/platform/web/favicon.png"
      $<TARGET_FILE_DIR:NimbleWood>/favicon.png
  )

endif()

# ---------------------------------------------------------------------------
# Install layout
#
# Used by CI to stage release archives:
#
#   <prefix>/
#   ├── NimbleWood(.exe)
#   ├── index.html
#   ├── index.js
#   ├── index.wasm
#   ├── assets/
#   ├── README.md
#   └── LICENSE
#
# Android is packaged by Gradle.
# iOS is packaged by CI from the Xcode build output.
# ---------------------------------------------------------------------------
if(NOT ANDROID)

  install(
    TARGETS NimbleWood
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .
  )

  install(
    DIRECTORY assets/
    DESTINATION assets
  )

  install(
    FILES
      README.md
      LICENSE
    DESTINATION .
  )

  if(EMSCRIPTEN)

    install(
      FILES
        "$<TARGET_FILE_DIR:NimbleWood>/index.js"
        "$<TARGET_FILE_DIR:NimbleWood>/index.wasm"
        "$<TARGET_FILE_DIR:NimbleWood>/favicon.png"
      DESTINATION .
    )

  endif()

endif()
