-- Common paths
build_path        = "%{wks.location}"
dev_path          = "%{wks.location}/../dev"
tools_path        = "%{wks.location}/../tools"

target_dir = "%{cfg.buildcfg}-%{cfg.architecture}-%{cfg.system}"
obj_dir    = "%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}-%{cfg.system}"

-------------------------
-- include directories --
-------------------------

include_dirs = {}

-- dev projects include directories
include_dirs["crayon_lib"] = dev_path .. "/crayon-lib/include"
include_dirs["crayon"] = dev_path .. "/crayon/include"

-----------------------------
-- source code directories --
-----------------------------
src_dirs = {}

-- dev projects source code directories
src_dirs["crayon_lib"] = dev_path .. "/crayon-lib/src"
src_dirs["crayon"] = dev_path .. "/crayon/src"

-------------------------
-- library directories --
-------------------------
lib_dirs = {}