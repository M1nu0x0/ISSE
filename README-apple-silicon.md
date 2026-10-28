# ISSE for Apple Silicon (arm64)

Fork of [ISSE: An Interactive Source Separation Editor](https://isse.sourceforge.net/)
by Nicholas J. Bryan, Gautham J. Mysore and Ge Wang (Adobe Research / Stanford CCRMA),
GPLv3. The original history is preserved; the last upstream commit is 2013-12-15.

This branch adds a native macOS arm64 build. No source behaviour is intended to change.

## What changed

- `CMakeLists.txt`: macOS build replacing the Introjucer-generated Xcode project
  (which is not in the upstream repository). Compiles the same sources as
  `builds/Linux/Makefile`, using the JUCE module `.mm` wrappers.
- Dropped the Intel-only pieces of the original Mac setup: Intel MKL / `libiomp5`
  (post-build copy) and SSE flags. OpenMP is optional via Homebrew `libomp`.
- AU / VST plug-in hosting and QuickTime are disabled (2013 JUCE hosting code
  depends on APIs that no longer exist on arm64 macOS).
- FFTW is linked statically so the `.app` is self-contained.
- Submodules: JUCE now points at `juce-framework/JUCE` (same pinned commit,
  JUCE 3.0.0). The Eigen mirror used upstream no longer exists, so Eigen 3.2
  headers are fetched at build time. The `extras` submodule (SourceForge,
  `git://`) is not needed for the build and was removed.

## Build

CI (`.github/workflows/macos-arm64.yml`) builds on a GitHub-hosted Apple Silicon
runner and uploads `ISSE-macos-arm64.zip`.

Locally (needs Xcode Command Line Tools and Homebrew):

```sh
git submodule update --init sdks/juce
brew install fftw libomp
mkdir -p sdks/eigen && curl -fsSL https://gitlab.com/libeigen/eigen/-/archive/3.2.10/eigen-3.2.10.tar.gz | tar -xz --strip-components=1 -C sdks/eigen
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DISSE_USE_OPENMP=ON
cmake --build build -j
open build/ISSE.app
```

## Running the downloaded app

The app is ad-hoc signed, not notarized. After unzipping:

```sh
xattr -dr com.apple.quarantine ISSE.app
```
