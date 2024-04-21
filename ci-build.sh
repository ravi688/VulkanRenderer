#!/usr/bin/bash

make -s setup
git submodule update
make -s build
make -s shader-clean
make -s clean
