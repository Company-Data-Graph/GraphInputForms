project "glad"
    kind "StaticLib"
    language "C"

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
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
