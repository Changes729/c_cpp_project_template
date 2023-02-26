# this is required
SET(CMAKE_SYSTEM_NAME Windows)

# specify the cross compiler
if(APPLE)
  SET(CMAKE_C_COMPILER      /opt/homebrew/bin/x86_64-w64-mingw32-gcc)
  SET(CMAKE_CXX_COMPILER    /opt/homebrew/bin/x86_64-w64-mingw32-g++)
  SET(CMAKE_FIND_ROOT_PATH  /opt/homebrew/Cellar/mingw-w64/10.0.0_4/toolchain-x86_64/x86_64-w64-mingw32/)
else(LINUX)
  SET(CMAKE_C_COMPILER      /usr/bin/x86_64-w64-mingw32-gcc)
  SET(CMAKE_CXX_COMPILER    /usr/bin/x86_64-w64-mingw32-g++)
  SET(CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32/)
endif()

# search for programs in the build host directories (not necessary)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")
