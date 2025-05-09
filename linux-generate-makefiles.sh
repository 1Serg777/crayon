#!/bin/sh

script_path="$(dirname "$0")"
script_path="$(cd "$premake_exe_path"; pwd)"

premake_exe_path="${script_path}/tools/premake/premake5"

os_premake_flag="--os=linux"

ide_action="gmake"

# 1. Script's directory:
# echo "Script executed from: ${PWD}"
# echo $premake_exe_path

# 2. Execute script
# echo  $os_premake_flag $ide_action
# echo $premake_path $os_premake_flag $ide_action
# exec $premake_exe_path $os_premake_flag $ide_action
$premake_exe_path $os_premake_flag $ide_action