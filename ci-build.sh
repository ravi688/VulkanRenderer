#!/usr/bin/bash

git submodule update -r
make -s setup
make -s init
make -s build-debug WALL=1
make -s shader-clean
make -s clean
make -s build-release WALL=1
make -s shader-clean
make -s clean
