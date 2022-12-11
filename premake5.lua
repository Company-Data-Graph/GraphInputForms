workspace "DataGraphDataLoadTool"
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	architecture "x86_64"
	staticruntime "on"
	flags "MultiProcessorCompile"
	startproject "Tool"

	configurations
	{
		"Release",
		"Debug"
	}
	
group "Dependencies"
	include  "include/glad"
	include  "include/GLFW"
	include  "include/imgui"
group ""

project "Tool"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	files 
	{
		".editorconfig",
		".clang-format",
		"src/**.hpp",
		"src/**.cpp",
		"include/stb/**.*",
		"include/json/**.*",
		"include/spdlog/include/**.*",
		"include/postgres/include/**.h",
		"include/postgres/interfaces/**.h",
		"include/pgfe/src/**.h",
		"include/pgfe/src/**.hpp"
	}

	includedirs
	{
		"src",
       	"include/glad/include",
       	"include/GLFW/include",
       	"include/imgui",
       	"include/stb",
		"include/spdlog/include/",
		"include/json",
		"include/postgres/include",
		"include/postgres/interfaces",
		"include/pgfe/src"
    }
	
	libdirs {
		"libs/OpenSSL",
		"libs/PostgreSQL"
	}	

	links 
	{ 
		"GLFW",
		"glad",
		"imgui",
		"libcrypto-3-x64.dll",
		"libssl-3-x64.dll",
		"libiconv-2.dll",
		"libintl-9.dll",
		"libpq",
		"libpq.dll",
		"libwinpthread-1.dll",
		"Ws2_32"
	}

	defines 
	{
		"NOMINMAX",
		"SPDLOG_NOEXPECTION",
		"DMITIGR_PGFE_NOT_HEADER_ONLY",
		"UNICODE"
	}

	filter "configurations:Debug"
		defines
		{
			"_DEBUG",
			"SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG"
		}	
		links
		{
			"libs/PostgreSQL/pgfed"
		}
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines
		{
			"NDEBUG",
			"SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF",
			"STBI_NO_FAILURE_STRINGS"
		}
		links
		{
			"libs/PostgreSQL/pgfe"
		}

newaction {
   trigger     = "clean",
   description = "clean the software",
   execute     = function ()
      print("Clean premakefiles...")
	  
	  print("Cleaning Visual studio files")
	  os.rmdir(".vs")
	  os.remove("*.sln")
      os.remove("**.vcxproj*")
	  print("Done")
	  
	  print("Cleaning compiled files")
	  os.rmdir("bin")
	  os.rmdir("obj")
	  print("Done")
	  
	  print("Cleaning makefiles")
	  os.remove("**Makefile")
	  os.remove("**.make")
	  print("Done")
	  
	  print("Cleaning cmake")
	  os.rmdir("**CMakeFiles/")
	  os.remove("*CMakeFiles")
	  os.remove("*CMakeCache.txt")
	  os.remove("**.cmake")
	  print("Done")
	  
      print("Cleaning done.")
   end
}
