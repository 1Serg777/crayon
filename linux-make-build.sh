#!/bin/sh

script_path="$(dirname "$0")"
# or
script_path="$(cd "$premake_exe_path"; pwd)"

make -C "${script_path}/build"