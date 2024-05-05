#!/usr/bin/bash

make -s setup
git submodule update -r
make -s build-debug WALL=1
make -s shader-clean
make -s clean
make -s build-release WALL=1
make -s shader-clean
make -s clean
