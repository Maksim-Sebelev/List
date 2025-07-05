#!/bin/bash

source "$HOME/.bash_functions" || exit 1

build_dir="build/"

cmake -B ${build_dir}      || exit 1
cmake --build ${build_dir} || exit 1
bash_cd ${build_dir}       || exit 1
