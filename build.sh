#!/bin/bash
# usage: ./build.sh [gnu|msvc] [Debug|Release]

# check for the compiler argument
if [ -z "$1" ]; then
    echo "Usage: $0 [gnu|msvc] [Debug|Release]"
    exit 1
fi

if [ "$1" == "gnu" ]; then
    export CXX=g++
    QT_DIR="C:/Qt/6.5.3/mingw_64"
elif [ "$1" == "msvc" ]; then
    export CXX=cl
    QT_DIR="C:/Qt/6.5.3/msvc2019_64"
else
    echo "Unknown compiler: $1"
    exit 1
fi
QT_PLUGIN_PATH="$QT_DIR"/plugins

# set the build type (default to Release if not specified)
BUILD_TYPE=${2:-Release}
BUILD_NAME=$1_"${BUILD_TYPE,}"

# build directory setup
BUILD_DIR=build

if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# run CMake to configure the project
cmake -S "../" \
    -B "./$BUILD_NAME" \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_PREFIX_PATH=$QT_DIR \
    -DCMAKE_CXX_COMPILER=$CXX

# build the project
cmake --build "./$BUILD_NAME" --config $BUILD_TYPE

# deploy qt dependencies (shared libs and plugins)
"$QT_DIR"/bin/windeployqt.exe --qmldir ../sources/app ../bin/$BUILD_NAME

echo "Build completed."
