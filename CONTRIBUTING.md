# Contributing to Nimble Wood

Thanks for your interest in Nimble Wood! Bug reports, fixes and ideas are welcome.

## Reporting bugs and suggesting features

Open an [issue](../../issues) and include:

- what you expected and what happened instead
- the platform (Linux, Windows, macOS, Android, iOS or web) and the release version or commit
- steps to reproduce, and any console output

For security problems, do **not** open an issue. See [SECURITY.md](SECURITY.md).

## Setting up

Follow [Build locally](README.md#build-locally) in the README. In short, you need a C++17 compiler, CMake 3.16+ and SDL3 (or let CMake build it with `-DNIMBLE_BUNDLE_SDL=ON`). Mobile and web builds are described in [Build for mobile and web](README.md#build-for-mobile-and-web).

## Making a change

1. Fork the repository and create a branch from the default branch.
2. Make your change, keeping it focused: one fix or feature per pull request.
3. Build and run the game on at least one platform. Mention in the pull request which platforms you tested; if you could not test one that your change affects, say so.
4. Open a pull request describing what changed and why. Link the issue it closes, if any.

For anything large (new systems, new dependencies, big refactors), please open an issue first so we can agree on the approach before you spend time on it.

## Guidelines

- **Keep it light.** The project aims to stay lightweight, with only the code and dependencies it needs. Avoid adding libraries unless there is a clear reason.
- **C++17**, no compiler extensions. The build uses `-Wall -Wextra` (`/W4` on MSVC), so please leave your change free of new warnings.
- **Stay cross-platform.** Code must work on desktop, Android, iOS and the web. Use SDL's APIs rather than platform-specific calls, and guard anything unavoidable with SDL's platform macros (for example `SDL_PLATFORM_EMSCRIPTEN`). The entry points are SDL's main callbacks in `src/main.cpp`, which is what makes this possible, so don't replace them with a plain `main()` loop.
- **Match the existing style:** 4-space indentation, braces on the same line, short functions, comments only where the code isn't obvious.
- **Don't commit build output, large binaries, or secrets.** Game assets go in `assets/`; only add assets you have the right to distribute under the project's license.
- **SDL version:** it is pinned in one place, `NIMBLE_SDL_TAG` in `CMakeLists.txt`. Updating it is welcome as its own pull request.

## Commit messages

Use a short, imperative summary line (for example `Clamp player to screen edges`), with details in the body if needed.

## Releases and CI

Releases are built by [`release.yml`](.github/workflows/release.yml) when a maintainer pushes a `v*` tag. It does not run on pull requests, so please build locally before submitting. Maintainers can trigger a full multi-platform build from the Actions tab to check a change across platforms.

## License

By contributing, you agree that your contribution is licensed under the project's [MIT License](LICENSE).
