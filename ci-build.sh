#!/usr/bin/bash

make -s setup
git submodule update
make -s build WALL=1
make -s shader-clean
make -s clean
