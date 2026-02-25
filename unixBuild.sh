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
        cmake -B ./build -G "Visual Studio 18 2026"
        echo "Solution file Written to: ./build/<name>.slnx"
        # DOESN'T WORK
        # 'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\devenv.exe' .\build\template.slnx /Build
    else
        cmake -B ./build -G "MinGW Makefiles"
        make -C ./build
    fi
fi
    
if [ $run = true ]
then
    echo
    echo "WIP - UNIX RUN"
fi