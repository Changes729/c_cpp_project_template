#!/bin/bash

# export CMAKE_PROJECT_NAME="output"

SCRIPT_DIR=$(dirname $(readlink -f "$0"))
MAIN_DIR=$SCRIPT_DIR/..

debug() {
  cd $MAIN_DIR/build
  cmake .. -DCMAKE_BUILD_TYPE=DEBUG && cmake --build .
}

clean() {
  if [ -f ./build/CMakeCache.txt ]; then
    rm ./build/CMakeCache.txt
  fi
}

build() {
  cd $MAIN_DIR/build
  cmake .. && cmake --build .
}

run() {
  if [ -d $MAIN_DIR/build/bin ]; then
    cd $MAIN_DIR/build/bin
  else
    build
  fi

  if [ ! -f $CMAKE_PROJECT_NAME ]; then
    build
  fi

  ./$CMAKE_PROJECT_NAME
}

## Start from here. #####################################
input=$1

if [ ! -d ./build ]; then
  mkdir build
fi

case "$input" in
"--debug")
  debug
  ;;
"--clean" | "-c")
  clean
  ;;
"--build" | "-b")
  build
  ;;
"--rebuild" | "-re")
  clean && build
  ;;
"--run" | "-r")
  run
  ;;
*)
  clean && build && run
  ;;
esac
