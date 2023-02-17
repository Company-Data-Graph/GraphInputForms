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
    
    filter "configurations:Debug or Test"
		runtime "Debug"
		symbols "on"
		defines "_DEBUG"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines "NDEBUG"
