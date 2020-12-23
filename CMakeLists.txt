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

# message("Executable Name: " ${CPACK_PROJECT_NAME})
message("Executable Name: " $ENV{CMAKE_PROJECT_NAME})

include_directories(
  src/example
)

add_executable( ${CPACK_PROJECT_NAME}
  src/main.c

  src/example/example.cpp
)

# Cannot check this.
#find_library(libpthread REQUIRE)
#target_link_libraries(Start_from_zero pthread)
