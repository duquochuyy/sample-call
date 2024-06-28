#!/bin/bash

find_project_root() {
    local dir="$1"
    while [ "$dir" != "/" ]; do
        if [ -f "$dir/config.json" ]; then
            echo "$dir"
            return
        fi
        dir=$(dirname "$dir")
    done
    echo "Error: Project root not found" >&2
    exit 1
}

current_dir=$(pwd)

project_root=$(find_project_root "$current_dir")

if [ "$1" == "build" ]; then
    mkdir -p "$project_root/build/qt_mac"
    cd "$project_root/build/qt_mac"
    qmake "$project_root/projects/QT_MAC/SampleCallback.pro" -spec macx-clang CONFIG+=debug CONFIG+=qml_debug QMAKE_APPLE_DEVICE_ARCHS="arm64" && /usr/bin/make qmake_all
    make -w
    if [ $? -ne 0 ]; then
        echo "Build failed. Exiting."
        exit 1
    else
        echo "Build success."
    fi
elif [ "$1" == "run" ]; then
    ./qt_mac.sh build
    
    ./qt_mac.sh close

    if [ "$2" == "terminal" ]; then
        app_dir="$project_root/build/qt_mac/SampleCallback.app/Contents/MacOS"

        osascript -e 'tell app "Terminal"
            do script "'"${app_dir}/SampleCallback 8080"'"
        end tell' &

        osascript -e 'tell app "Terminal"
            do script "'"${app_dir}/SampleCallback 8081"'"
        end tell' &
    else
        "$project_root/build/qt_mac/SampleCallback.app/Contents/MacOS/SampleCallback" 8080 &
        "$project_root/build/qt_mac/SampleCallback.app/Contents/MacOS/SampleCallback" 8081 &
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
