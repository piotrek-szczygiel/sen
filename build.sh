#!/bin/bash

opts=-g

code="$(dirname "$(readlink -f "$0")")"
pushd "$code"
mkdir -p out
pushd out
g++ $opts "$code/src/main.cpp" -o sen
popd
popd
