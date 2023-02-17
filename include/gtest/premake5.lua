project "gtest"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	files
	{
		"src/*.*",
		"include/**.h"
	}

    includedirs
    {
        "include"
    }
        
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
        defines "_DEBUG"

	filter "configurations:Release or Test"
		runtime "Release"
		optimize "on"
        defines "NDEBUG"
