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

## support pkg_check_modules
find_package(PkgConfig REQUIRED)

# pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
# include_directories(${LIBUSB_INCLUDE_DIRS})
# link_directories(${LIBUSB_LIBRARY_DIRS})
# link_libraries(${LIBUSB_LIBRARIES})

include_directories(src/example)
include_directories(include)

add_executable( ${CPACK_PROJECT_NAME}
  src/main.c

  src/example/example.cpp
)

# Cannot check this.
#find_library(libpthread REQUIRE)
#target_link_libraries(Start_from_zero pthread)

if(NOT "${SPIKE_EXAMPLES}" STREQUAL "OFF")
  message("build spike subdirectory")
  add_subdirectory(spike)
endif()