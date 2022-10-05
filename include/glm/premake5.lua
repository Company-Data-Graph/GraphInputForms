project "glm"
	kind "StaticLib"
	language "C++"
	
	files
	{
		"**.cpp",
		"**.hpp",
		"**.inl",
	}

	includedirs
    {
        "include"
    }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
