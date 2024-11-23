#! /usr/bin/bash


# example: BUILD_PATH=<my build path> ./<this script>.sh

PLUTOVG_GIT_REPO_PATH=https://github.com/ravi688/plutovg.git
if [ -z $BUILD_PATH ]; then
	BUILD_PATH=./setup_plutovg.build.dir
fi

PLUTOVG_REPO_PATH="${BUILD_PATH}/plutovg"
PLUTOVG_CMAKE_BUILD_PATH="${PLUTOVG_REPO_PATH}/build"

if [ -d $PLUTOVG_REPO_PATH ]; then
	echo "The repo already seem to be cloned, skipping git clone"
else
	git clone $PLUTOVG_GIT_REPO_PATH $PLUTOVG_REPO_PATH
fi

if [ -d $PLUTOVG_CMAKE_BUILD_PATH ]; then
	cmake --build $PLUTOVG_CMAKE_BUILD_PATH --target clean
fi
echo "Running cmake to generate makefile"
cmake -S $PLUTOVG_REPO_PATH -B $PLUTOVG_CMAKE_BUILD_PATH -DCMAKE_INSTALL_PREFIX=/usr/share/libplutovg
echo "Running make"
make --directory=$PLUTOVG_CMAKE_BUILD_PATH install -j4

# NOTE: here we need to use `pkg-config --variable pc_path pkg-config` instead of PKG_CONFIG_PATH, because on linux I've found it to be null
PKG_CONFIG_PC_PATH=`pkg-config --variable pc_path pkg-config | cut -d ':' -f1`
mkdir -p $PKG_CONFIG_PC_PATH

echo "Writing to ${PKG_CONFIG_PC_PATH}/libplutovg.pc"
echo "prefix=/usr/share/libplutovg
includedir=\${prefix}/include
libdir=\${prefix}/lib

Name: plutovg
Description: PlutoVg (pkg config setup by Ravi Prakash Singh)
URL: https://github.com/sammycage/plutovg.git
Version: 1.0.0
Libs: -L\${libdir} -lplutovg
Cflags: -I\${includedir}" > "${PKG_CONFIG_PC_PATH}/libplutovg.pc"
