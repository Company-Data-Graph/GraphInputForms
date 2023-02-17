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

	filter "configurations:Release or Test"
		runtime "Release"
		optimize "on"
		defines "NDEBUG"
