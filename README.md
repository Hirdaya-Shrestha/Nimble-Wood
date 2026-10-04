<p align="center">
    <img src="https://raw.githubusercontent.com/Hirdaya-Shrestha/Nimble-Wood/main/assets/logo.png" height="150" width="150" alt="Nimble-Wood">
</p>
<h1 align="center">Nimble Wood</h1>

<p align="center">
  A cross-platform 2D platformer written in C++17 with <a href="https://libsdl.org">SDL3</a>.<br>
  One codebase for Linux, Windows, macOS, Android, iOS and the web.
</p>

<p align="center">
  <a href="https://github.com/Hirdaya-Shrestha/Nimble-Wood/actions/workflows/release.yml"><img alt="Release workflow" src="https://github.com/Hirdaya-Shrestha/Nimble-Wood/actions/workflows/release.yml/badge.svg"></a>
  <a href="https://github.com/Hirdaya-Shrestha/Nimble-Wood/releases/latest"><img alt="Latest release" src="https://img.shields.io/github/v/release/Hirdaya-Shrestha/Nimble-Wood"></a>
  <a href="LICENSE"><img alt="License: MIT" src="https://img.shields.io/badge/license-MIT-blue.svg"></a>
</p>

---

## Overview

Nimble Wood is at an early stage: the repository is a clean, working foundation with a resizable window, a fixed-timestep game loop and a controllable placeholder character. Everything needed to build and ship for every platform is already in place, so game development can focus on gameplay.

**Highlights**

- Small, dependency-light codebase: C++17, CMake and SDL3 only
- A single entry point built on SDL's main callbacks, shared by desktop, mobile and web
- Self-contained release builds (SDL3 linked statically)
- Tag-triggered CI that builds and publishes every platform

## Platforms

| Platform | Architectures | Release artifact |
|---|---|---|
| Linux | x86-64, arm64 | `.tar.gz` |
| Windows | x86-64 | `.zip` |
| macOS | Universal (Apple Silicon + Intel), macOS 11+ | `.tar.gz` containing `NimbleWood.app` |
| Android | arm64-v8a, armeabi-v7a, x86_64 (API 21+) | `.apk` |
| iOS | arm64 (iOS 13+) | `.ipa` (unsigned) |
| Web | WebAssembly | `.zip` (`index.html`, `index.js`, `index.wasm`) |

## Download

Pre-built packages for every platform are on the [Releases](../../releases) page, together with a `SHA256SUMS.txt` file for verifying downloads.

> [!NOTE]
> Binaries are not code-signed. Windows SmartScreen and macOS Gatekeeper may warn on first launch, and the iOS package must be re-signed before it can be installed. See [Platform notes](#platform-notes).

## Controls

| Input | Action |
|---|---|
| `A` / `D` or `←` / `→` | Move |
| Touch or click, left / right half of the screen | Move left / right |
| `Esc` | Quit (desktop) |

## Building from source

### Requirements

- A C++17 compiler (GCC, Clang or MSVC)
- [CMake](https://cmake.org) 3.16 or newer
- Git (only when SDL3 is built from source)
- SDL3 and SDL3_image 3.2 or newer, **optional**: if they are not installed, CMake downloads and builds pinned versions automatically (PNG and JPEG are supported out of the box)

### Linux

<details open>
<summary><b>Arch Linux</b></summary>

```bash
sudo pacman -S --needed base-devel cmake ninja git sdl3 sdl3_image

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/NimbleWood
```
</details>

<details>
<summary><b>Debian / Ubuntu</b></summary>

Distribution SDL3 packages may be missing or too old, so build SDL3 from source. It needs the development packages below.

```bash
sudo apt-get install -y build-essential cmake ninja-build pkg-config git \
  libasound2-dev libpulse-dev libaudio-dev libfribidi-dev libjack-dev libsndio-dev \
  libx11-dev libxext-dev \
  libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
  libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
  libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev \
  libpipewire-0.3-dev libwayland-dev libwayland-bin wayland-protocols \
  libdecor-0-dev liburing-dev

cmake -S . -B build -G Ninja -DNIMBLE_BUNDLE_SDL=ON
cmake --build build
./build/NimbleWood
```
</details>

<details>
<summary><b>Fedora</b></summary>

```bash
sudo dnf install cmake ninja-build gcc-c++ git SDL3-devel SDL3_image-devel

cmake -S . -B build -G Ninja
cmake --build build
./build/NimbleWood
```
</details>

### macOS

```bash
brew install cmake ninja sdl3 sdl3_image

cmake -S . -B build -G Ninja
cmake --build build
open build/NimbleWood.app
```

### Windows

Choose one of the two toolchains.

<details open>
<summary><b>Option A: Visual Studio (MSVC)</b></summary>

Install Visual Studio 2022 or newer with the **Desktop development with C++** workload. If Visual Studio is already installed, open the *Visual Studio Installer*, choose *Modify* and tick that workload. From a terminal, the same can be done with:

```powershell
winget install --id Microsoft.VisualStudio.2022.Community --override "--wait --passive --add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended"
winget install --id Git.Git
```

Then open **Developer PowerShell for VS** from the Start menu and run:

```powershell
cmake -S . -B build -DNIMBLE_BUNDLE_SDL=ON
cmake --build build --config Release
.\build\Release\NimbleWood.exe
```

This is the same toolchain the release builds use.
</details>

<details>
<summary><b>Option B: MSYS2 (MinGW-w64), command line only</b></summary>

Install [MSYS2](https://www.msys2.org/), open the **MSYS2 UCRT64** shell and run:

```bash
pacman -S --needed git mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja \
  mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-sdl3 mingw-w64-ucrt-x86_64-sdl3-image

cmake -S . -B build -G Ninja
cmake --build build
./build/NimbleWood.exe
```

This uses MSYS2's libraries, so `SDL3.dll` and `SDL3_image.dll` must be found: run from the UCRT64 shell, or copy both from `/ucrt64/bin/` next to the `.exe`. For a standalone `.exe`, add `-DNIMBLE_BUNDLE_SDL=ON` to the first `cmake` command.
</details>

### Web (WebAssembly)

Install [Emscripten](https://emscripten.org/docs/getting_started/downloads.html), then:

```bash
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
cd build-web && python3 -m http.server 8080    # open http://localhost:8080/index.html
```

Browsers refuse to load `.wasm` files from `file://`, so always test through a local server.

### Android

Requirements: Android SDK with NDK and CMake (easiest through Android Studio), JDK 17 and Gradle 8.7.

1. Download two archives, extract the `.aar` inside each and save them in `android/app/libs/`:

   | Download | Save as |
   |---|---|
   | `SDL3-devel-<version>-android.zip` from the [SDL releases](https://github.com/libsdl-org/SDL/releases) (version = `NIMBLE_SDL_TAG`) | `SDL3.aar` |
   | `SDL3_image-devel-<version>-android.zip` from the [SDL_image releases](https://github.com/libsdl-org/SDL_image/releases) (version = `NIMBLE_SDL_IMAGE_TAG`) | `SDL3_image.aar` |

2. Build and install on a device or emulator:

```bash
cd android
gradle wrapper --gradle-version 8.7     # one time, creates ./gradlew
./gradlew installDebug                  # or: ./gradlew assembleRelease
```

The `android/` folder can also be opened directly in Android Studio. The Android build uses the prebuilt SDL libraries from the `.aar` files rather than building them from source.

### iOS

Requires a Mac with Xcode.

```bash
cmake -S . -B build-ios -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DNIMBLE_BUNDLE_SDL=ON
open build-ios/NimbleWood.xcodeproj
```

In Xcode, select the `NimbleWood` target, set your **Team** under *Signing & Capabilities*, choose a device or simulator and run. For the simulator, use `-DCMAKE_OSX_SYSROOT=iphonesimulator`.

### Build options

| Option | Default | Description |
|---|---|---|
| `NIMBLE_BUNDLE_SDL` | `OFF` | `ON` always builds SDL3 and SDL3_image from source and links them statically, producing a self-contained binary. `OFF` uses installed libraries and builds only the missing ones. iOS and web always build them. |
| `NIMBLE_SDL_TAG` | `release-3.4.18` | SDL git tag used when SDL3 is built from source. Also selects the Android `.aar` in CI. |
| `NIMBLE_SDL_IMAGE_TAG` | `release-3.4.6` | SDL_image git tag used when SDL3_image is built from source. Also selects the Android `.aar` in CI. |
| `CMAKE_BUILD_TYPE` | `Release` | `Debug` or `Release` (single-configuration generators). |

## Project structure

```
.
├── CMakeLists.txt                  Build definition for all platforms
├── src/main.cpp                    Game entry point (SDL main callbacks)
├── assets/                         Game assets, packaged into releases
├── branding/icon.png               Source image for all app icons
├── tools/make_icons.py             Generates every platform icon from branding/icon.png
├── android/                        Gradle project wrapping the CMake build into an APK
├── platform/                       Per-platform files: icons, Info.plist templates, Windows resource
└── .github/workflows/
    ├── release.yml                 Build and publish all platforms on tag
    └── pages.yml                   Publish the web build to GitHub Pages
```

## Icons and branding

All app icons are generated from one image, `branding/icon.png` (square PNG, 1024 x 1024 px or larger; transparency is fine).

```bash
pip install pillow                     # Arch: sudo pacman -S python-pillow
python3 tools/make_icons.py            # or: python3 tools/make_icons.py path/to/logo.png
```

Commit the generated files. The script writes:

| Platform | Generated file | Used by |
|---|---|---|
| Windows | `platform/windows/nimblewood.ico` | embedded in the `.exe` through `platform/windows/app.rc` |
| macOS | `platform/macos/nimblewood.icns` | `NimbleWood.app` bundle |
| iOS | `platform/ios/Assets.xcassets` | app icon asset catalog (1024 px, opaque: transparent areas are filled with a dark green) |
| Android | `android/app/src/main/res/mipmap-*/ic_launcher.png` | launcher icon (`android:icon` in the manifest) |
| Web | `platform/web/favicon.png` | browser tab icon, linked from `index.html` after the build |
| Linux, window icon | `src/icon.h` | 64 x 64 icon embedded in the game and set with `SDL_SetWindowIcon` |

Linux executables cannot carry an icon inside the file. The window icon above covers the window and, on most desktops, the taskbar; a file-manager icon would need a `.desktop` file, which is not included yet.

## Releasing

[`release.yml`](.github/workflows/release.yml) runs when a tag starting with `v` is pushed:

```bash
git tag v0.1.0
git push origin v0.1.0
```

It builds every platform in parallel, creates a GitHub Release with all packages, a `SHA256SUMS.txt` and generated release notes, and marks tags containing a `-` (for example `v0.2.0-beta.1`) as pre-releases. If any platform fails to build, nothing is published.

| Artifact | Runner |
|---|---|
| `linux-x64` | `ubuntu-24.04` |
| `linux-arm64` | `ubuntu-24.04-arm` |
| `windows-x64` | `windows-latest` (MSVC, static runtime) |
| `macos-universal` | `macos-latest` (arm64 + x86_64) |
| `web` | `ubuntu-24.04` with Emscripten |
| `android` | `ubuntu-24.04` with Gradle 8.7, JDK 17 and the SDL `.aar` |
| `ios` | `macos-latest` with Xcode |

The workflow can also be started manually from the **Actions** tab (**Run workflow**). That builds every platform and uploads the packages as workflow artifacts without creating a release, which is useful for testing.

To update SDL or SDL_image, change the defaults of `NIMBLE_SDL_TAG` and `NIMBLE_SDL_IMAGE_TAG` in `CMakeLists.txt` to newer `release-3.x.y` tags from the [SDL](https://github.com/libsdl-org/SDL/releases) and [SDL_image](https://github.com/libsdl-org/SDL_image/releases) releases. They are the only place to change; CI reads them to select the matching Android `.aar` files. Even minor versions (3.2.x, 3.4.x) are stable releases.

### Platform notes

- **Linux** binaries are built on Ubuntu 24.04 and need glibc 2.39 or newer. SDL loads X11, Wayland and audio libraries at runtime, so no SDL packages are needed to run the game. The `linux-arm64` runner is free for public repositories only; for a private repository, remove that matrix entry.
- **Windows**: SmartScreen may show a warning. Choose *More info*, then *Run anyway*.
- **macOS**: the package contains `NimbleWood.app`. After extracting, run `xattr -dr com.apple.quarantine NimbleWood.app` once to clear the Gatekeeper quarantine flag, then open it.
- **Android**: the APK is signed with the debug key, so it installs for testing but cannot be published to a store. For a store release, add your own keystore to `android/app/build.gradle` and build an `.aab` with `bundleRelease`.
- **iOS**: the `.ipa` is unsigned because signing requires an Apple Developer account. Re-sign it with a tool such as Sideloadly or AltStore, or build from Xcode with your own team. For TestFlight or App Store delivery, add signing certificates and a provisioning profile as repository secrets and extend the `build-ios` job.
- **Web**: unzip and host the three files, or publish them on GitHub Pages as described below.

## Roadmap

- Tilemap loading and level format
- Player physics (gravity, jumping, collision)
- Sprites and animation (SDL3_image is already linked; load images with `IMG_LoadTexture`)
- Audio (`SDL3_mixer`)
- Asset packaging for iOS (app bundle resources) and the web (Emscripten `--preload-file`), so images can be loaded on those platforms

Additional SDL libraries can be added the same way SDL3_image is added in `CMakeLists.txt`.

## Contributing

Contributions are welcome. Please read [CONTRIBUTING.md](CONTRIBUTING.md) first. To report a security issue, follow [SECURITY.md](SECURITY.md) and do not open a public issue.

## License

Released under the [MIT License](LICENSE).
