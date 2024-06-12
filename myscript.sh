#!/bin/bash

if [ "$1" == "build" ]; then
    qmake
    make -w
    if [ $? -ne 0 ]; then
        echo "Build failed. Exiting."
        exit 1
    else
        echo "Build success."
    fi
elif [ "$1" == "run" ]; then
    ./myscript.sh build

    if [ "$2" == "terminal" ]; then
        app_dir="/Users/lap15850/HuyDQ6/projects/SampleCallback/SampleCallback.app/Contents/MacOS"

        osascript -e 'tell app "Terminal"
            do script "'"${app_dir}/SampleCallback 8080"'"
        end tell' &

        osascript -e 'tell app "Terminal"
            do script "'"${app_dir}/SampleCallback 8081"'"
        end tell' &
    else
        ./SampleCallback.app/Contents/MacOS/SampleCallback 8080 &
        ./SampleCallback.app/Contents/MacOS/SampleCallback 8081 &
    fi
    

    if [ $? -ne 0 ]; then
        echo "Run failed. Exiting."
        exit 1
    fi
    
elif [ "$1" == "close" ]; then
    pkill -f SampleCallback.app/Contents/MacOS/SampleCallback
    
else
    echo "Please type correct option"
    echo "build: build application"
    echo "run: run 2 application to test"
    echo "run terminal: run 2 application with them terminal"
    echo "close: close 2 test application"
fi
