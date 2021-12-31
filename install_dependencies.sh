#!/bin/bash

git clone https://github.com/assimp/assimp.git
cd assimp
git checkout v5.0.1
cmake CMakeLists.txt
make -j4