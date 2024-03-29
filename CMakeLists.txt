cmake_minimum_required(VERSION 3.0.0)
project(Playground VERSION 0.1.0)

## set CPACK_PROJECT_NAME
if(DEFINED ENV{CMAKE_PROJECT_NAME})
  message("Use environment project name: " $ENV{CMAKE_PROJECT_NAME})
  set(CPACK_PROJECT_NAME $ENV{CMAKE_PROJECT_NAME})
else()
  set(CPACK_PROJECT_NAME ${PROJECT_NAME})
  set(ENV{CMAKE_PROJECT_NAME} ${CPACK_PROJECT_NAME})
endif()

## set CPACK_PROJECT_VERSION
if(DEFINED ENV{CMAKE_PROJECT_VERSION})
  message("Use environment project version: " $ENV{CMAKE_PROJECT_NAME})
  set(CPACK_PROJECT_VERSION $ENV{CMAKE_PROJECT_VERSION})
else()
  set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
  set(ENV{CMAKE_PROJECT_VERSION} ${CPACK_PROJECT_VERSION})
endif()

if(CMAKE_TOOLCHAIN_FILE)
  MESSAGE(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif()

## set Compiler Flags.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1Z)
if(COMPILER_SUPPORTS_CXX17)
  message("Use std C++17.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX1Z)
  message("Use std C++1z.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

## extra compiler warnings
if ("${CMAKE_C_COMPILER_ID}" MATCHES ".*Clang.*")
	SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wunused-variable -Wswitch-default -Werror")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wunused-but-set-variable -Wunused-variable -Wswitch-default -Werror")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/linux_pc)

## support pkg_check_modules
find_package(PkgConfig REQUIRED)
include(CheckSymbolExists)
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

add_subdirectory(3rdparty/googletest)

# pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
# include_directories(${LIBUSB_INCLUDE_DIRS})
# link_directories(${LIBUSB_LIBRARY_DIRS})
# link_libraries(${LIBUSB_LIBRARIES})

set(PTHREAD_CFLAGS "-D_REENTRANT")
set(PTHREAD_LDFLAGS "-pthread")
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${PTHREAD_CFLAGS} ${PTHREAD_LDFLAGS}")
CHECK_C_SOURCE_COMPILES("
  #include <pthread.h>
  int main(int argc, char** argv) {
    pthread_attr_t type;
    pthread_attr_init(&type);
    return 0;
  }" HAVE_PTHREADS)
message("HAVE_PTHREADS:" ${HAVE_PTHREADS})

add_subdirectory(src)

if(NOT "${SPIKE_EXAMPLES}" STREQUAL "OFF")
message("build spike subdirectory")
add_subdirectory(spike)
endif()

add_subdirectory(test)