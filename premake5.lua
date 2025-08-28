-- include "dependencies.lua"

workspace("crayon")
   architecture  ("x86_64")
   platforms     ("x64")
   configurations({"Debug", "Release"})
   location      ("build" )
   startproject  ("crayon")

include "dependencies.lua"

-- include ( dev_path .. "/crayon-lib" )
-- include ( dev_path .. "/crayon" )

include("dev/crayon-lib")
include("dev/crayon")