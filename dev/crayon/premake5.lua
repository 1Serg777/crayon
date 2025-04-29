project ( "crayon" )
    kind       ( "ConsoleApp" )
    language   ( "C++" )
    cppdialect ( "C++17" )
    location   ( build_path .. "/crayon" )
    targetdir  ( build_path .. "/bin/" .. target_dir )
    objdir     ( build_path .. "/bin-int/" .. obj_dir )

    includedirs {
        "%{include_dirs.crayon_lib}",
        "%{include_dirs.crayon}",
    }
    libdirs {
        build_path .. "/bin/" .. target_dir
    }

    links {
        "crayon-lib",
    }

    files {
        "%{include_dirs.crayon}/**.h",
        "%{include_dirs.crayon}/**.hpp",
        "%{src_dirs.crayon}/**.cpp",
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
                "%{include_dirs.crayon}/**.h",
                "%{include_dirs.crayon}/**.hpp"
            },
            ["Sources/*"] = {
                "%{src_dirs.crayon}/**.cpp",
            },
        }