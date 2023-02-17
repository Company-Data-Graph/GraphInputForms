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

	filter "configurations:Debug or Test"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines "NDEBUG"
