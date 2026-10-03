cmake_minimum_required(VERSION 3.16)

project(NimbleWood VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Statically link the MSVC runtime so the .exe runs without the VC++ redistributable.
# Must be set before SDL is added so SDL uses the same runtime.
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ---------------------------------------------------------------------------
# SDL3
#   Android : SDL3 comes from the SDL .aar (prefab) added by the Gradle project in android/.
#   default : use the system SDL3 if installed (pacman -S sdl3, apt, brew, ...)
#             and fall back to building the pinned version below if not found.
#   ON      : always build SDL3 from source and link it statically (used by CI,
#             produces self-contained binaries). iOS and web always do this.
# ---------------------------------------------------------------------------
option(NIMBLE_BUNDLE_SDL "Build SDL3 from source and link statically" OFF)
set(NIMBLE_SDL_TAG "release-3.4.16" CACHE STRING "SDL3 git tag used when bundling")

if(ANDROID)
  find_package(SDL3 REQUIRED CONFIG)
elseif(NOT NIMBLE_BUNDLE_SDL AND NOT IOS AND NOT EMSCRIPTEN)
  find_package(SDL3 3.2 CONFIG QUIET)
endif()

if(NOT SDL3_FOUND)
  message(STATUS "Building SDL3 (${NIMBLE_SDL_TAG}) from source, static")
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
  message(STATUS "Using SDL3 ${SDL3_VERSION}")
endif()

# ---------------------------------------------------------------------------
# Game
# ---------------------------------------------------------------------------
if(ANDROID)
  # SDLActivity loads libmain.so, so the game must be a shared library named "main".
  add_library(NimbleWood SHARED src/main.cpp)
  set_target_properties(NimbleWood PROPERTIES OUTPUT_NAME main)
else()
  add_executable(NimbleWood src/main.cpp)
endif()

target_compile_definitions(NimbleWood PRIVATE NIMBLE_VERSION="${PROJECT_VERSION}")
target_link_libraries(NimbleWood PRIVATE SDL3::SDL3)

if(MSVC)
  target_compile_options(NimbleWood PRIVATE /W4)
else()
  target_compile_options(NimbleWood PRIVATE -Wall -Wextra)
endif()

if(WIN32)
  # GUI app: no console window (SDL_main.h provides WinMain).
  set_target_properties(NimbleWood PROPERTIES WIN32_EXECUTABLE ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Fewer runtime dependencies on the target distro.
  target_link_options(NimbleWood PRIVATE -static-libstdc++ -static-libgcc)
elseif(IOS)
  set_target_properties(NimbleWood PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Info.plist.in"
    MACOSX_BUNDLE_GUI_IDENTIFIER "dev.nimblewood.game"
    MACOSX_BUNDLE_BUNDLE_NAME "Nimble Wood"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "dev.nimblewood.game"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
  )
elseif(EMSCRIPTEN)
  # Produces index.html + index.js + index.wasm
  set_target_properties(NimbleWood PROPERTIES OUTPUT_NAME index SUFFIX ".html")
  target_link_options(NimbleWood PRIVATE -sALLOW_MEMORY_GROWTH=1)
endif()

# ---------------------------------------------------------------------------
# Install layout (used by CI to stage release archives; Android is packaged by Gradle,
# iOS by CI from the Xcode build output)
#   <prefix>/NimbleWood(.exe) or index.html/js/wasm, assets/, README.md, LICENSE
# ---------------------------------------------------------------------------
if(NOT ANDROID)
  install(TARGETS NimbleWood RUNTIME DESTINATION . BUNDLE DESTINATION .)
  install(DIRECTORY assets/ DESTINATION assets)
  install(FILES README.md LICENSE DESTINATION .)
  if(EMSCRIPTEN)
    install(FILES
      "$<TARGET_FILE_DIR:NimbleWood>/index.js"
      "$<TARGET_FILE_DIR:NimbleWood>/index.wasm"
      DESTINATION .)
  endif()
endif()
