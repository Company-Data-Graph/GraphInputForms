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

	filter  "configurations:Debug or Release"
		removefiles { "imgui_te*.h", "imgui_te*.cpp", "imgui_capture_tool.h", "imgui_capture_tool.cpp", "thirdparty/**.*" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release or Test"
		runtime "Release"
		optimize "on"
		defines "NDEBUG"

	filter "configurations:Test"
		defines "IMGUI_ENABLE_TEST_ENGINE"

