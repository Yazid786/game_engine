#!/bin/bash

set -e

mkdir -p ./build

time {
	clang++ ./bin/build.cpp -o build/game -O0 -g3 -std=c++11 -pedantic -lX11 -lXrandr -lGL
	clang++ ./game/game.cpp -o ./build/game_temp.so -fPIC -shared
	mv ./build/game_temp.so ./game.so
}

