#!/usr/bin/env bash

mkdir -p build
cd build

cmake ..
make || { echo "Make failed with code $?"; exit $?; }

if [[ "$1" = "-r" ]]; then
	shift
	cd ..
	./build/dot-vm "$@"
fi

