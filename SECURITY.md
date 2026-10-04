# Security Policy

## Supported versions

Only the latest release of Nimble Wood receives security fixes. Please update to the newest version from the [Releases](../../releases) page before reporting an issue.

## Reporting a vulnerability

Please **do not open a public issue** for security problems.

Report privately through GitHub: go to the **Security** tab of this repository, choose **Report a vulnerability**, and describe the problem. Include, where you can:

- what the issue is and which platform(s) it affects (Linux, Windows, macOS, Android, iOS, web)
- the release version or commit
- steps to reproduce, or a proof of concept
- the impact you expect

You can expect an acknowledgement within 7 days and a status update within 30 days. Fixes are published as a new release, and you will be credited in the release notes unless you prefer to stay anonymous.

## Scope

In scope:

- the game code in `src/`
- the build files and workflows (`CMakeLists.txt`, `android/`, `.github/workflows/`), for example anything that could let a malicious change reach the published release files or the GitHub Pages site
- the packaged downloads from the Releases page

Out of scope:

- vulnerabilities in [SDL](https://github.com/libsdl-org/SDL) itself, please report those to the SDL project; if one affects Nimble Wood, a note here is welcome so the pinned version can be updated
- vulnerabilities in your operating system, browser, or toolchain
- issues that need a modified or already compromised device
- social engineering, or denial-of-service by simply running the game with unreasonable settings

## Verifying downloads

Every release includes a `SHA256SUMS.txt` file. Check a download before running it:

```bash
sha256sum -c SHA256SUMS.txt --ignore-missing
```

Release binaries are not code-signed: Windows and macOS may show an "unknown publisher" warning, the Android APK is signed with a debug key, and the iOS `.ipa` is unsigned. Only download releases from this repository's Releases page.
