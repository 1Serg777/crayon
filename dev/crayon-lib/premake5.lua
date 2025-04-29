project ( "crayon-lib" )
    kind       ( "StaticLib" )
    language   ( "C++" )
    cppdialect ( "C++17" )
    location   ( build_path .. "/crayon-lib" )
    targetdir  ( build_path .. "/bin/" .. target_dir )
    objdir     ( build_path .. "/bin-int/" .. obj_dir )

    includedirs {
        "%{include_dirs.crayon_lib}",
    }

    files {
        "%{include_dirs.crayon_lib}/**.h",
        "%{include_dirs.crayon_lib}/**.hpp",
        "%{src_dirs.crayon_lib}/**.cpp",
    }

    filter ( "configurations:Debug" )
        defines ( { "DEBUG", "_DEBUG" } )
        runtime ( "Debug" )
        symbols ( "On" )

    filter ( "configurations:Release" )
        defines  ( { "NDEBUG", "_NDEBUG" } )
        runtime  ( "Release" )
        optimize ( "On" )

    filter ( { "system:windows", "action:vs*" } )
        vpaths {
            ["Include/*"] = {
                "%{include_dirs.crayon_lib}/**.h",
                "%{include_dirs.crayon_lib}/**.hpp"
            },
            ["Sources/*"] = {
                "%{src_dirs.crayon_lib}/**.cpp",
            },
        }