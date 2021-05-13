#!/bin/bash

SCRIPT_DIR=$(dirname $(readlink -f "$0"))
TEST_DIR=$SCRIPT_DIR/../build/bin/test

if [ -d $TEST_DIR ]; then
  for exe in $(ls $TEST_DIR); do
    valgrind --leak-check=yes $TEST_DIR/$exe
  done
fi
