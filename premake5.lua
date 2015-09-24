solution "multicore_engine_solution"
	configurations{"debug", "release"}
	defines{}
	includedirs{"mutlicore_engine_core/include"}
	vectorextensions "SSE2"
	startproject "mutlicore_engine_demo"
	
	configuration{"gmake"}
		flags{"FatalWarnings"}
		warnings "Extra"

	configuration "debug"
		defines{"DEBUG"}
		flags{"Symbols"}
		optimize "Off"
		
	configuration "release"
		defines{"NDEBUG"}
		optimize "Full"
	
	configuration "windows"
		defines{"MULTICORE_ENGINE_WINDOWS"}
		includedirs{}
	configuration "not windows"
		defines{"MULTICORE_ENGINE_NOT_WINDOWS"}
		includedirs{}
	
	configuration{"gmake"}
		targetdir "%{prj.location}/bin-mingw/%{cfg.buildcfg}"
		objdir "%{prj.location}/obj-mingw/%{cfg.buildcfg}"
		buildoptions "-std=gnu++14"

	configuration {"gmake","not windows"}
		if _OPTIONS["cc"] == "clang" then
			toolset "clang"
			buildoptions "-stdlib=libc++"
			links "c++"
		end

	configuration {"vs2015"}
		architecture "x64"
		buildoptions {"/MP"}
		flags {"LinkTimeOptimization"}
		targetdir "%{prj.location}/bin-vc/%{cfg.buildcfg}"
		objdir "%{prj.location}/obj-vc/%{cfg.buildcfg}"
		
	project "mutlicore_engine_core"
		kind "StaticLib"
		language "C++"
		location "mutlicore_engine_core/build"
		files { "mutlicore_engine_core/include/**.hpp", "mutlicore_engine_core/src/**.cpp"}

	project "mutlicore_engine_demo"
		kind "ConsoleApp"
		--kind "WindowedApp"
		language "C++"
		location "mutlicore_engine_demo/build"
		files { "mutlicore_engine_demo/include/**.hpp", "mutlicore_engine_demo/src/**.cpp"}
		links {"mutlicore_engine_core"}
		configuration {"vs2015"}
			debugdir "mutlicore_engine_demo"
