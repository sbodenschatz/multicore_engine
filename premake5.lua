solution "multicore_engine_solution"
	configurations{"debug", "release"}
	defines{}
	includedirs{"multicore_engine_core/include"}
	vectorextensions "SSE2"
	startproject "multicore_engine_demo"
	warnings "Extra"
	flags{"FatalWarnings"}
	
	configuration "debug"
		defines{"DEBUG"}
		flags{"Symbols"}
		optimize "Off"
		
	configuration "release"
		defines{"NDEBUG"}
		optimize "Full"
	
	configuration "windows"
		defines{"MULTICORE_ENGINE_WINDOWS"}
		includedirs{"C:/Libs/Boost/include"}
	configuration "not windows"
		defines{"MULTICORE_ENGINE_NOT_WINDOWS"}
		includedirs{}
	
	configuration{"gmake"}
		targetdir "%{prj.location}/bin-gcc/%{cfg.buildcfg}"
		objdir "%{prj.location}/obj-gcc/%{cfg.buildcfg}"
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
		flags {"LinkTimeOptimization","NoIncrementalLink"}
		targetdir "%{prj.location}/bin-vc/%{cfg.buildcfg}"
		objdir "%{prj.location}/obj-vc/%{cfg.buildcfg}"
		
	configuration {"vs2015"}
		libdirs {"C:/Libs/Boost/lib_x64_vc"}
	configuration {"gmake","windows","debug"}
		libdirs {"C:/Libs/Boost/lib_x64_mingw_debug"}
	configuration {"gmake","windows","release"}
		libdirs {"C:/Libs/Boost/lib_x64_mingw_release"}
		
	project "multicore_engine_core"
		kind "StaticLib"
		language "C++"
		location "multicore_engine_core/build"
		files { "multicore_engine_core/include/**.hpp", "multicore_engine_core/src/**.cpp"}

	project "multicore_engine_core_tests"
		kind "ConsoleApp"
		--kind "WindowedApp"
		language "C++"
		location "multicore_engine_core_tests/build"
		files { "multicore_engine_core_tests/include/**.hpp", "multicore_engine_core_tests/src/**.cpp"}
		links {"multicore_engine_core"}
		--removeflags{"Symbols"}
		--optimize "Debug"
		configuration {"gmake"}
			buildoptions "-Wno-deprecated-declarations -Wno-unused-variable"
			links {"boost_unit_test_framework"}
		configuration {"vs2015"}
			debugdir "multicore_engine_core_tests"

	project "multicore_engine_demo"
		kind "ConsoleApp"
		--kind "WindowedApp"
		language "C++"
		location "multicore_engine_demo/build"
		files { "multicore_engine_demo/include/**.hpp", "multicore_engine_demo/src/**.cpp"}
		links {"multicore_engine_core"}
		configuration {"vs2015"}
			debugdir "multicore_engine_demo"
