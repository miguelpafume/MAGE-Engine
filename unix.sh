#!/bin/bash

build=false;
run=false;
windows=false;

while [ ! $# -eq 0 ]
do  
    case "$1" in
        -b | --build)
            build=true
            shift
            ;;
        -r | --run)
            run=true
            shift
            ;;
        -w | --windows)
            windows=true
            shift
            ;;
    esac
done

if [ $build = true ]
then
    mkdir -p build
    
    if [ $windows = true ]
    then
        cmake -B ./build -G "MinGW Makefiles"
        cmake --build ./build --parallel
    else
        cmake -B ./build
        make -C ./build
    fi
fi

if [ $run = true ]
then
    if [ $windows = true ]
    then
        ./build/mage_engine.exe
    else
        echo
        ./build/mage_engine
    fi
fi

# base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-cmake