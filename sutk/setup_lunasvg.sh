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
cmake -S $LUNASVG_REPO_PATH -B $LUNASVG_CMAKE_BUILD_PATH -DCMAKE_INSTALL_PREFIX=/usr/share/liblunasvg
echo "Running make"
make --directory=$LUNASVG_CMAKE_BUILD_PATH install -j4

# NOTE: here we need to use `pkg-config --variable pc_path pkg-config` instead of PKG_CONFIG_PATH, because on linux I've found it to be null
PKG_CONFIG_PC_PATH=`pkg-config --variable pc_path pkg-config | cut -d ':' -f1`
mkdir -p $PKG_CONFIG_PC_PATH

echo "Writing to ${PKG_CONFIG_PC_PATH}/liblunasvg.pc"
echo "prefix=/usr/share/liblunasvg
includedir=\${prefix}/include
libdir=\${prefix}/lib

Name: lunasvg
Description: LunaSVG (pkg config setup by Ravi Prakash Singh)
URL: https://github.com/sammycage/lunasvg.git
Version: 1.0.0
Libs: -L\${libdir} -llunasvg
Cflags: -I\${includedir}" > "${PKG_CONFIG_PC_PATH}/liblunasvg.pc"
