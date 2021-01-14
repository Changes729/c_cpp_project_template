cmake_minimum_required(VERSION 3.0.0)
project(Playground VERSION 0.1.0)

if(DEFINED ENV{CMAKE_PROJECT_NAME})
  set(CPACK_PROJECT_NAME $ENV{CMAKE_PROJECT_NAME})
else()
  set(CPACK_PROJECT_NAME ${PROJECT_NAME})
  set(ENV{CMAKE_PROJECT_NAME} ${CPACK_PROJECT_NAME})
endif()

if(DEFINED ENV{CMAKE_PROJECT_VERSION})
  set(CPACK_PROJECT_VERSION $ENV{CMAKE_PROJECT_VERSION})
else()
  set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
  set(ENV{CMAKE_PROJECT_VERSION} ${CPACK_PROJECT_VERSION})
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1Z)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX1Z)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

# message("Executable Name: " ${CPACK_PROJECT_NAME})
message("Executable Name: " $ENV{CMAKE_PROJECT_NAME})

include_directories(
  src/example/

  include/
)

add_executable( ${CPACK_PROJECT_NAME}
  src/main.c

  src/example/example.cpp
)

# Cannot check this.
#find_library(libpthread REQUIRE)
#target_link_libraries(Start_from_zero pthread)
