#! /usr/bin/bash


# example: BUILD_PATH=<my build path> ./<this script>.sh

LUNASVG_GIT_REPO_PATH=https://github.com/sammycage/lunasvg.git
if [ -z $BUILD_PATH ]; then
	BUILD_PATH=./setup_lunasvg.build.dir
fi

LUNASVG_REPO_PATH="${BUILD_PATH}/lunasvg"
LUNASVG_CMAKE_BUILD_PATH="${LUNASVG_REPO_PATH}/build"

if [ -d $LUNASVG_REPO_PATH ]; then
	echo "The repo already seem to be cloned, skipping git clone"
else
	git clone $LUNASVG_GIT_REPO_PATH $LUNASVG_REPO_PATH
fi

if [ -d $LUNASVG_CMAKE_BUILD_PATH ]; then
	cmake --build $LUNASVG_CMAKE_BUILD_PATH --target clean
fi
echo "Running cmake to generate makefile"
cmake -S $LUNASVG_REPO_PATH -B $LUNASVG_CMAKE_BUILD_PATH -DCMAKE_INSTALL_PREFIX=/usr
echo "Running make"
make --directory=$LUNASVG_CMAKE_BUILD_PATH install
