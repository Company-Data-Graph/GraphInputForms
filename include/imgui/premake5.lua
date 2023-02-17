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

	filter  "configurations:not Test"
		removefiles { "imgui_te*.h", "imgui_te*.cpp", "imgui_capture_tool.h", "imgui_capture_tool.cpp", "thirdparty/**.*" }

	filter "configurations:Debug or Test"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines "NDEBUG"

	filter "configurations:Test"
		defines "IMGUI_ENABLE_TEST_ENGINE"

