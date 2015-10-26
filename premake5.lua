solution "multicore_engine_solution"
	configurations{"debug", "release"}
	defines{"GLM_FORCE_RADIANS","GLM_SWIZZLE","GLM_FORCE_SIZE_T_LENGTH"}
	includedirs{"multicore_engine_core/include","multicore_engine_parsers/include"}
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
	configuration "not windows"
		defines{"MULTICORE_ENGINE_NOT_WINDOWS"}
		includedirs{}
	
	configuration{"gmake"}
		targetdir "%{prj.location}/bin-gcc/%{cfg.buildcfg}"
		objdir "%{prj.location}/obj-gcc/%{cfg.buildcfg}"
		buildoptions "-std=gnu++14"
		links {"pthread"}
		buildoptions "-Wno-unused-parameter"

	configuration {"gmake"}
		if _OPTIONS["cc"] == "clang" then
			buildoptions "-stdlib=libc++  -Wno-unused-private-field"
			targetdir "%{prj.location}/bin-clang/%{cfg.buildcfg}"
			objdir "%{prj.location}/obj-clang/%{cfg.buildcfg}"
			toolset "clang"
			links "c++"
		end

	configuration {"gmake","linux"}
			includedirs {"/usr/local/include/local-boost"}
		
	configuration {"gmake","linux","debug"}
		if _OPTIONS["cc"] == "clang" then
			buildoptions "-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined"
			libdirs {"/usr/local/lib/local-boost/lib-clang-debug"}
			linkoptions {"-rpath /usr/local/lib/local-boost/lib-clang-debug -fsanitize=address -fsanitize=undefined"}
		end
		if _OPTIONS["cc"] == "gcc" then
			libdirs {"/usr/local/lib/local-boost/lib-gcc-debug"}
			linkoptions {"-rpath /usr/local/lib/local-boost/lib-gcc-debug -fsanitize=address -fsanitize=undefined"}
		end


	configuration {"gmake","linux","release"}
		if _OPTIONS["cc"] == "clang" then
			libdirs {"/usr/local/lib/local-boost/lib-clang-release"}
			linkoptions {"-rpath /usr/local/lib/local-boost/lib-clang-release"}
		end
		if _OPTIONS["cc"] == "gcc" then
			libdirs {"/usr/local/lib/local-boost/lib-gcc-release"}
			linkoptions {"-rpath /usr/local/lib/local-boost/lib-gcc-release"}
		end
		
	configuration{"gmake","windows"}
		buildoptions "-isystemC:/Libs/Boost/include -isystemC:/Libs/glm/include"
	
	configuration {"vs2015"}
		defines{"GLM_FORCE_CXX11"}
		includedirs{"C:/Libs/Boost/include","C:/Libs/glm/include"}
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
		
	project "multicore_engine_parsers"
		kind "StaticLib"
		language "C++"
		location "multicore_engine_parsers/build"
		files {"multicore_engine_parsers/src/**.cpp"}

		configuration{"gmake"}
			buildoptions "-Wno-sign-compare"
		
		configuration{"gmake","windows"}
			-- disable debug settings for the parser because 32-bit-hosted MinGW used on windows runs into OOM or other size restrictions when compiling the parser
			removeflags{"Symbols"}
			optimize "Debug"

	project "multicore_engine_core"
		kind "StaticLib"
		language "C++"
		location "multicore_engine_core/build"
		links {"multicore_engine_parsers"}
		files {"multicore_engine_core/include/**.hpp", "multicore_engine_core/src/**.cpp"}

	project "multicore_engine_core_tests"
		kind "ConsoleApp"
		--kind "WindowedApp"
		language "C++"
		location "multicore_engine_core_tests/build"
		files { "multicore_engine_core_tests/include/**.hpp", "multicore_engine_core_tests/src/**.cpp"}
		links {"multicore_engine_core","multicore_engine_parsers"}
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
		links {"multicore_engine_core","multicore_engine_parsers"}
		configuration {"vs2015"}
			debugdir "multicore_engine_demo"
