# Nimble Wood

A 2D platformer written in C++17 with [SDL3](https://libsdl.org). Currently an empty, working skeleton: a resizable window, a fixed-timestep game loop and a square you can move with **A/D**, **←/→**, or by touching/clicking the left or right half of the screen. **Esc** quits on desktop.

Builds for **Linux (x64, arm64)**, **Windows (x64)**, **macOS (universal: Apple Silicon + Intel)**, **Android**, **iOS** and the **web** (WebAssembly).

## Project layout

```
.
├── CMakeLists.txt                 # build definition (all platforms)
├── src/main.cpp                   # game entry point (SDL main callbacks)
├── assets/                        # game assets (packaged into releases)
├── android/                       # Gradle project that wraps the CMake build into an APK
├── platform/ios/Info.plist.in     # iOS app metadata
├── .github/workflows/release.yml  # build + publish on tag
├── LICENSE
└── README.md
```

## Requirements

- A C++17 compiler (GCC, Clang or MSVC)
- CMake 3.16 or newer
- Git (only needed when SDL3 is built from source)
- SDL3 3.2 or newer, **optional**: if it is not installed, CMake downloads and builds a pinned version automatically (see [Build options](#build-options))

## Build locally

### Arch Linux

```bash
sudo pacman -S --needed base-devel cmake ninja git sdl3

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/NimbleWood
```

### Debian / Ubuntu

The distro SDL3 package may be missing or too old, so build SDL3 from source (needs the dev packages below):

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

### Fedora

```bash
sudo dnf install cmake ninja-build gcc-c++ git SDL3-devel
cmake -S . -B build -G Ninja && cmake --build build && ./build/NimbleWood
```

### macOS

```bash
brew install cmake ninja sdl3
cmake -S . -B build -G Ninja && cmake --build build && ./build/NimbleWood
```

### Windows (Visual Studio 2022 or newer, with the "Desktop development with C++" workload)

```powershell
cmake -S . -B build -DNIMBLE_BUNDLE_SDL=ON
cmake --build build --config Release
.\build\Release\NimbleWood.exe
```

## Build for mobile and web

CI builds all of these automatically (see [Releasing](#releasing)); the steps below are for building them yourself.

### Web (WebAssembly)

Install [Emscripten](https://emscripten.org/docs/getting_started/downloads.html), then:

```bash
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
cd build-web && python3 -m http.server 8080   # open http://localhost:8080/index.html
```

The output is `index.html`, `index.js` and `index.wasm`; upload the three files to any static host (GitHub Pages, itch.io HTML5, Netlify, ...). Browsers refuse to load `.wasm` from `file://`, so always test through a local server.

### Android

Requirements: Android SDK with NDK and CMake (easiest through Android Studio), JDK 17, and Gradle 8.7.

1. Download `SDL3-devel-<version>-android.zip` from the [SDL releases](https://github.com/libsdl-org/SDL/releases) (use the version in `NIMBLE_SDL_TAG`), extract the `.aar` inside and save it as `android/app/libs/SDL3.aar`.
2. Build and install on a connected device or emulator:

```bash
cd android
gradle wrapper --gradle-version 8.7     # one time, creates ./gradlew (commit it if you like)
./gradlew installDebug                  # or assembleRelease -> app/build/outputs/apk/release/
```

You can also open the `android/` folder in Android Studio. The Android build uses the prebuilt SDL from the `.aar`, not the source build.

### iOS (needs a Mac with Xcode)

```bash
cmake -S . -B build-ios -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DNIMBLE_BUNDLE_SDL=ON
open build-ios/NimbleWood.xcodeproj
```

In Xcode select the `NimbleWood` target, set your **Team** under *Signing & Capabilities*, pick a device or simulator and run. For the simulator use `-DCMAKE_OSX_SYSROOT=iphonesimulator` instead.

## Build options

| Option | Default | Meaning |
|---|---|---|
| `NIMBLE_BUNDLE_SDL` | `OFF` | `ON` always builds SDL3 from source and links it statically (self-contained binary). `OFF` uses the installed SDL3 and falls back to building it if none is found. |
| `NIMBLE_SDL_TAG` | `release-3.4.16` | SDL git tag used when SDL is built from source. |
| `CMAKE_BUILD_TYPE` | `Release` | `Debug` or `Release` (single-config generators). |

Example: `cmake -S . -B build -DNIMBLE_BUNDLE_SDL=ON -DNIMBLE_SDL_TAG=release-3.x.y`

## Releasing

Releases are built by [`release.yml`](.github/workflows/release.yml) whenever a tag starting with `v` is pushed:

```bash
git tag v0.0.1
git push origin v0.0.1
```

The workflow then:

1. Builds seven artifacts in parallel:

   | Artifact | Runner | Output |
   |---|---|---|
   | `linux-x64` | `ubuntu-24.04` | `.tar.gz` |
   | `linux-arm64` | `ubuntu-24.04-arm` | `.tar.gz` |
   | `windows-x64` | `windows-latest` (MSVC, static runtime) | `.zip` |
   | `macos-universal` | `macos-latest` (arm64 + x86_64, macOS 11+) | `.tar.gz` |
   | `web` | `ubuntu-24.04` + Emscripten | `.zip` (`index.html/js/wasm`) |
   | `android` | `ubuntu-24.04` + Gradle 8.7, JDK 17, SDL `.aar` | `.apk` (arm64-v8a, armeabi-v7a, x86_64) |
   | `ios` | `macos-latest` + Xcode | `-ios-unsigned.ipa` (arm64, iOS 13+) |

2. Packages desktop and web builds with `assets/`, `README.md` and `LICENSE`.
3. Creates a GitHub Release for the tag with all files, a `SHA256SUMS.txt` and auto-generated notes. If any platform fails to build, nothing is published. Tags containing a `-` (for example `v0.2.0-beta.1`) are marked as pre-releases.

You can also run the workflow manually from the **Actions** tab (**Run workflow**). That builds every platform and uploads the archives as workflow artifacts without creating a release, which is handy for testing.

Notes:

- `ubuntu-24.04-arm` runners are free for **public** repositories only. For a private repo, remove that matrix entry.
- Linux binaries are built on Ubuntu 24.04, so they need glibc 2.39 or newer (Ubuntu 24.04+, Fedora 40+, current Arch). SDL loads X11, Wayland, audio, etc. at runtime, so no SDL packages are needed to run the game.
- **Android**: the APK is signed with the debug key, so it installs for testing (`adb install` or tap the file after allowing "unknown sources") but cannot go to the Play Store. For a store release, add your own keystore to `android/app/build.gradle` and build an `.aab` (`bundleRelease`).
- **iOS**: the `.ipa` is **unsigned** because signing needs an Apple Developer account. It proves the iOS build works; to run it, re-sign it with a tool such as Sideloadly or AltStore, or build from Xcode with your own team (see above). To publish through TestFlight or the App Store, add signing certificates and a provisioning profile as repository secrets and extend the `build-ios` job.
- **Web**: unzip and host the three files, or try it locally with `python3 -m http.server`.
- Downloaded macOS and Windows binaries are unsigned. macOS: run `xattr -dr com.apple.quarantine NimbleWood-*` once after extracting. Windows: SmartScreen may show a warning, choose "More info" then "Run anyway".

## Updating SDL

Change the default of `NIMBLE_SDL_TAG` in `CMakeLists.txt` to a newer `release-3.x.y` tag from the [SDL releases](https://github.com/libsdl-org/SDL/releases). Even minor numbers (3.2.x, 3.4.x) are stable releases. CI also reads this value to pick the matching Android `.aar`, so it is the only place to change.

## Roadmap

Tilemap loading, player physics, sprites and audio. For those, add `SDL3_image` / `SDL3_mixer` the same way SDL3 is added in `CMakeLists.txt`. Assets are packaged for desktop, web-hosting and Android already; for iOS they still need to be added to the app bundle (`MACOSX_PACKAGE_LOCATION`) and for the web they can be embedded with Emscripten's `--preload-file assets`.

## License

MIT, see [LICENSE](LICENSE).
