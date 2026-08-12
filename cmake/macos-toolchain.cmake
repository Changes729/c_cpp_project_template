# osxcross macOS cross-compilation toolchain for Linux
# Usage: cmake .. -DCMAKE_TOOLCHAIN_FILE=cmake/macos-toolchain.cmake

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ── compiler ──────────────────────────────────────────────────────────
set(CMAKE_C_COMPILER   /usr/local/osx-ndk-x86/bin/o64-clang)
set(CMAKE_CXX_COMPILER /usr/local/osx-ndk-x86/bin/o64-clang++)

# ── sysroot (osxcross provides this) ──────────────────────────────────
set(CMAKE_OSX_SYSROOT /usr/local/osx-ndk-x86/SDK/MacOSX10.11.sdk)

# ── flags ─────────────────────────────────────────────────────────────
set(CMAKE_C_FLAGS   "-Wall -Werror" CACHE STRING "")
set(CMAKE_CXX_FLAGS "-Wall -Werror" CACHE STRING "")

# ── find utilities on host, not target ─────────────────────────────────
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
