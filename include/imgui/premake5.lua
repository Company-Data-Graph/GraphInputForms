project "imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	files
	{
		"*.cpp",
		"*.h"
	}

	includedirs
	{
		"../GLFW/include",
		"../glad/include"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
