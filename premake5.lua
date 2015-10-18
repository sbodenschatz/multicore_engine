solution "multicore_engine_solution"
	configurations{"debug", "release"}
	defines{"GLM_FORCE_RADIANS","GLM_SWIZZLE","GLM_FORCE_SIZE_T_LENGTH"}
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
		includedirs{"C:/Libs/Boost/include","C:/Libs/glm/include"}
	configuration "not windows"
		defines{"MULTICORE_ENGINE_NOT_WINDOWS"}
		includedirs{}
	
	configuration{"gmake"}
		targetdir "%{prj.location}/bin-gcc/%{cfg.buildcfg}"
		objdir "%{prj.location}/obj-gcc/%{cfg.buildcfg}"
		buildoptions "-std=gnu++14"
		links {"pthread"}

	configuration {"gmake"}
		if _OPTIONS["cc"] == "clang" then
			buildoptions "-stdlib=libc++"
			targetdir "%{prj.location}/bin-clang/%{cfg.buildcfg}"
			objdir "%{prj.location}/obj-clang/%{cfg.buildcfg}"
			toolset "clang"
			links "c++"
		end

	configuration {"gmake","linux"}
		if _OPTIONS["cc"] == "clang" then
			includedirs {"/usr/local/include/clang-libs"}
		end
		
	configuration {"gmake","linux","debug"}
		if _OPTIONS["cc"] == "clang" then
			buildoptions "-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined"
			libdirs {"/usr/local/lib/clang-libs/lib-debug"}
			linkoptions {"-rpath /usr/local/lib/clang-libs/lib-debug -fsanitize=address -fsanitize=undefined"}
		end

	configuration {"gmake","linux","release"}
		if _OPTIONS["cc"] == "clang" then
			libdirs {"/usr/local/lib/clang-libs/lib-release"}
			linkoptions {"-rpath /usr/local/lib/clang-libs/lib-release"}
		end

	configuration {"vs2015"}
		defines{"GLM_FORCE_CXX11"}
		architecture "x64"
		buildoptions {"/MP"}
		flags {"LinkTimeOptimization","NoIncrementalLink","NoMinimalRebuild"}
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
			buildoptions "-Wno-deprecated-declarations -Wno-unused-variable  -Wno-unused-parameter"
			links {"boost_unit_test_framework"}
		configuration {"gmake","linux"}
			defines {"BOOST_TEST_DYN_LINK"}
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
