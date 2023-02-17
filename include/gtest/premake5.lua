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
        "include",
		"src"
    }
        
	filter "configurations:Debug or Test"
		runtime "Debug"
		symbols "on"
        defines "_DEBUG"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        defines "NDEBUG"
