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
action="$1"
codec="h264"
if [[ "$2" == "codec:h264" ]]; then
    codec="h264"
elif [[ "$2" == "codec:av1" ]]; then
    codec="av1"
fi

build_app() {
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
}

run_app() {
    build_app
    close_app

    "$project_root/build/qt_mac/SampleCallback.app/Contents/MacOS/SampleCallback" 8080 "$codec" &
    "$project_root/build/qt_mac/SampleCallback.app/Contents/MacOS/SampleCallback" 8081 "$codec" &

    if [ $? -ne 0 ]; then
        echo "Run failed. Exiting."
        exit 1
    fi
}

run_app_terminal() {
    build_app
    close_app

    app_dir="$project_root/build/qt_mac/SampleCallback.app/Contents/MacOS"

    osascript -e 'tell app "Terminal"
        do script "'"${app_dir}/SampleCallback 8080 $codec"'"
    end tell' &

    osascript -e 'tell app "Terminal"
        do script "'"${app_dir}/SampleCallback 8081 $codec"'"
    end tell' &

    if [ $? -ne 0 ]; then
        echo "Run failed. Exiting."
        exit 1
    fi
}

close_app() {
    pkill -f SampleCallback.app/Contents/MacOS/SampleCallback
}

case "$action" in
    build)
        build_app
        ;;
    run)
        run_app
        ;;
    "run:terminal")
        run_app_terminal
        ;;
    close)
        close_app
        ;;
    *)
        echo "Please type correct option"
        echo "build: build application"
        echo "run: run 2 application to test"
        echo "run:terminal: run 2 application with them terminal"
        echo "close: close 2 test application"
        ;;
esac 
