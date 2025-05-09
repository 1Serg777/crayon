#!/bin/sh

script_path="$(dirname "$0")"
script_path="$(cd "$premake_exe_path"; pwd)"

make -C "${script_path}/build"