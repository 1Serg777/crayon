local dev_path = "dev"
local build_path = "build"
local dependency_path = "dependencies"

--Project: crayon
local crayon_include_path = dev_path .. "/crayon/include"
local crayon_src_path = dev_path .. "/crayon/src"

--Project: crayon-lib
local crayon_lib_include_path = dev_path .. "/crayon-lib/include"
local crayon_lib_src_path = dev_path .. "/crayon-lib/src"

workspace ( "crayon" )
   configurations ( { "Debug", "Release" } )
   platforms ( { "x64" } )
   location ( build_path )

   -- filter ( { "system:windows", "action:vs*" } )
   startproject ("crayon")

project ( "crayon-lib" )
   kind ( "StaticLib" )
   language ( "C++" )
   cppdialect ( "C++17" )
   location ( build_path .. "/crayon-lib" )

   targetdir ( build_path .. "/bin/%{cfg.platform}-%{cfg.buildcfg}" )
   objdir ( build_path .. "/bin-int/crayon-lib/%{cfg.platform}-%{cfg.buildcfg}" )

   includedirs {
      crayon_lib_include_path
   }

   files {
      crayon_lib_include_path .. "/**.h",
      crayon_lib_include_path .. "/**.hpp",
      crayon_lib_src_path .. "/**.cpp"
   }

   filter ( "configurations:Debug" )
      defines ( { "DEBUG", "_DEBUG" } )
      symbols ( "On" )

   filter ( "configurations:Release" )
      defines ( { "NDEBUG", "_NDEBUG" } )
      optimize ( "On" )

   filter ( { "system:windows", "action:vs*" } )
      vpaths {
         ["Include/*"] = {
            crayon_lib_include_path .. "/**.h",
            crayon_lib_include_path .. "/**.hpp"
         },
         ["Sources/*"] = {
            crayon_lib_src_path .. "/**.cpp"
         },
      }

project ( "crayon" )
   kind ( "ConsoleApp" )
   language ( "C++" )
   cppdialect ( "C++17" )
   location ( build_path .. "/crayon" )

   targetdir ( build_path .. "/bin/%{cfg.platform}-%{cfg.buildcfg}" )
   objdir ( build_path .. "/bin-int/crayon/%{cfg.platform}-%{cfg.buildcfg}" )

   includedirs {
      crayon_lib_include_path,
      crayon_include_path
   }
   libdirs {
      build_path .. "/bin/%{cfg.platform}-%{cfg.buildcfg}"
   }

   links {
      "crayon-lib",
   }

   files {
      crayon_include_path .. "/**.h",
      crayon_include_path .. "/**.hpp",
      crayon_src_path .. "/**.cpp"
   }

   filter ( "configurations:Debug" )
      defines ( { "DEBUG", "_DEBUG" } )
      symbols ( "On" )

   filter ( "configurations:Release" )
      defines ( { "NDEBUG", "_NDEBUG" } )
      optimize ( "On" )

   filter ( { "system:windows", "action:vs*" } )
      vpaths {
         ["Include/*"] = {
            crayon_include_path .. "/**.h",
            crayon_include_path .. "/**.hpp"
         },
         ["Sources/*"] = {
            crayon_src_path .. "/**.cpp"
         },
      }