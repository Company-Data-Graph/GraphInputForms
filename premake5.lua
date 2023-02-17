require "scripts/functions"

workspace "DataGraphDataLoadTool"
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	projectName = "Tool"
	local wrkDir = path.getabsolute(".")
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")
	architecture "x86_64"
	staticruntime "on"
	flags "MultiProcessorCompile"
	startproject "Tool"

	configurations
	{
		"Release",
		"Debug",
		"Test"
	}
	
group "Dependencies"
	include  "include/glad"
	include  "include/GLFW"
	include  "include/imgui"
	include  "include/gtest"
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
		"include/pgfe/src/**.hpp",
		"include/fmt/src/*.cc",
		"include/fmt/include/**.h"
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
		"include/pgfe/src",
		"include/fmt/include",
		"include/gtest/include"
    }

	links 
	{ 
		"GLFW",
		"glad",
		"imgui",
		"Ws2_32",
		"gtest",
		"libs/PostgreSQL/libintl-9.dll",
		"libs/PostgreSQL/libssl-1_1-x64.dll",
		"libs/PostgreSQL/libwinpthread-1.dll",
		"libs/PostgreSQL/libcrypto-1_1-x64.dll",
		"libs/PostgreSQL/libiconv-2.dll",
		"libs/PostgreSQL/libpq.dll",
		"libs/PostgreSQL/libpq",
	}

	defines 
	{
		"NOMINMAX",
		"SPDLOG_NOEXPECTION",
		"DMITIGR_PGFE_NOT_HEADER_ONLY",
		"UNICODE"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		links "libs/PostgreSQL/pgfed"
		debugdir 'libs/PostgreSQL'
		postbuildcommands (wrkDir .. "/scripts/premake/bin/premake5.exe postBuild --configuration=Debug")
	
		defines
		{
			"_DEBUG",
			"SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG"
		}

	filter "configurations:Release or Test"
		runtime "Release"
		optimize "On"

		links "libs/PostgreSQL/pgfe"
		defines
		{
			"NDEBUG",
			"SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF",
			"STBI_NO_FAILURE_STRINGS"
		}
	
	filter "configurations:Test"
		defines "FORMHANDLER_TESTS"
		postbuildcommands (wrkDir .. "/scripts/premake/bin/premake5.exe postBuild --configuration=Test")

	filter "configurations:Release"
		postbuildcommands (wrkDir .. "/scripts/premake/bin/premake5.exe postBuild --configuration=Release")


require "scripts/actions"
