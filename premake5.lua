solution "multicore_engine_solution"
	configurations{"debug", "release"}
	defines{"GLM_FORCE_RADIANS","GLM_SWIZZLE","GLM_FORCE_SIZE_T_LENGTH","ZLIB_CONST"}
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
		defines{"MULTICORE_ENGINE_WINDOWS",
				---Target windows 7 or higher with windows api headers
				"_WIN32_WINNT=0x0601",
				"WINVER=0x0601"}
	configuration "not windows"
		defines{"MULTICORE_ENGINE_NOT_WINDOWS"}
		includedirs{}
	
	configuration{"gmake"}
		targetsuffix "-gcc-%{cfg.buildcfg}"
		targetdir "%{prj.location}/bin"
		objdir "%{prj.location}/obj-gcc-%{cfg.buildcfg}"
		buildoptions "-std=gnu++14"
		links {"pthread","boost_program_options","boost_filesystem","boost_system","boost_thread"}
		buildoptions "-Wno-unused-parameter -Wno-unused-variable -Wno-deprecated-declarations"

	configuration {"gmake"}
		if _OPTIONS["cc"] == "clang" then
			targetsuffix "-clang-%{cfg.buildcfg}"
			buildoptions "-stdlib=libc++  -Wno-unused-private-field -Wno-unused-local-typedef"
			targetdir "%{prj.location}/bin"
			objdir "%{prj.location}/obj-clang-%{cfg.buildcfg}"
			toolset "clang"
			links "c++"
		end

	configuration {"gmake","linux"}
			includedirs {"/usr/local/include/local-boost"}
			links {"z"}
		
	configuration {"gmake","linux","debug"}
		if _OPTIONS["cc"] == "clang" then
			buildoptions "-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined"
			libdirs {"/usr/local/lib/local-boost/lib-clang-debug"}
			linkoptions {"-rpath /usr/local/lib/local-boost/lib-clang-debug -fsanitize=address -fsanitize=undefined"}
		end
		if _OPTIONS["cc"] == "gcc" then
			libdirs {"/usr/local/lib/local-boost/lib-gcc-debug"}
			linkoptions {"-Wl,-rpath,/usr/local/lib/local-boost/lib-gcc-debug -fsanitize=address -fsanitize=undefined"}
		end


	configuration {"gmake","linux","release"}
		if _OPTIONS["cc"] == "clang" then
			libdirs {"/usr/local/lib/local-boost/lib-clang-release"}
			linkoptions {"-rpath /usr/local/lib/local-boost/lib-clang-release"}
		end
		if _OPTIONS["cc"] == "gcc" then
			libdirs {"/usr/local/lib/local-boost/lib-gcc-release"}
			linkoptions {"-Wl,-rpath,/usr/local/lib/local-boost/lib-gcc-release"}
		end
		
	configuration{"gmake","windows"}
		buildoptions "-isystemC:/Libs/Boost/include -isystemC:/Libs/glm/include -isystemC:/Libs/zlib/include"
		links {"ws2_32","mswsock","zstatic"}
	
	configuration {"vs2015"}
		targetsuffix "-vc-%{cfg.buildcfg}"
		defines{"GLM_FORCE_CXX11"}
		includedirs{"C:/Libs/Boost/include","C:/Libs/glm/include","C:/Libs/zlib/include"}
		architecture "x64"
		flags {"LinkTimeOptimization","NoIncrementalLink","NoMinimalRebuild","MultiProcessorCompile"}
		targetdir "%{prj.location}/bin"
		objdir "%{prj.location}/obj-vc-%{cfg.buildcfg}"
		
	configuration {"vs2015","debug"}
		links {"zlibstaticd"}
	configuration {"vs2015","release"}
		links {"zlibstatic"}

	configuration {"vs2015"}
		libdirs {"C:/Libs/Boost/lib_x64_vc","C:/Libs/zlib/lib_x64_vc"}
	configuration {"gmake","windows","debug"}
		libdirs {"C:/Libs/Boost/lib_x64_mingw_debug","C:/Libs/zlib/lib_x64_mingw_debug"}
	configuration {"gmake","windows","release"}
		libdirs {"C:/Libs/Boost/lib_x64_mingw_release","C:/Libs/zlib/lib_x64_mingw_release"}
		
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
		links { "multicore_engine_core","multicore_engine_parsers"}

		configuration {"windows"}
			links {"mingw32","vulkan-1"}
			libdirs {tostring(os.getenv("VK_SDK_PATH")) .. "/Bin"}
		--	postbuildcommands {
		--		os.getenv("VK_SDK_PATH") .. "\\Bin\\glslangValidator.exe -s -V -o ..\\cube-vert.spv ..\\cube.vert",
		--		os.getenv("VK_SDK_PATH") .. "\\Bin\\glslangValidator.exe -s -V -o ..\\cube-frag.spv ..\\cube.frag"
		--	}

		configuration {"vs2015"}
			debugdir "multicore_engine_demo"
		
		configuration {"gmake","not windows"}
			buildoptions "-std=gnu++1y"
			links {"pthread","tbb","tbbmalloc","vulkan"}
			libdirs {os.findlib("TBB2"),os.findlib("Vulkan")}
			if _OPTIONS["cc"] == "clang" then
				toolset "clang"
				buildoptions "-stdlib=libc++"
				links "c++"
			end

		configuration {"gmake","windows","Debug"}
			libdirs {"C:/Libs/TBB/lib/windows_intel64_gcc_mingw_debug"}
			links{"tbb_debug","tbbmalloc_debug"}
			postbuildcommands {
				"{COPY} C:\\Libs\\TBB\\lib\\windows_intel64_gcc_mingw_debug\\*_debug.dll %{cfg.buildtarget.directory}",
			}
		
		configuration {"gmake","windows","Release"}
			libdirs {"C:/Libs/TBB/lib/windows_intel64_gcc_mingw_release"}   
			links{"tbb","tbbmalloc"}
			postbuildcommands {
				"{COPY} C:\\Libs\\TBB\\lib\\windows_intel64_gcc_mingw_release\\*.dll %{cfg.buildtarget.directory}",
			}
		
		configuration {"vs2015","windows","Debug"}
			libdirs {"C:/Libs/TBB/lib/windows_intel64_vc/Debug"}
			links{"tbb_debug","tbbmalloc_debug"}
			postbuildcommands {
				"{COPY} C:\\Libs\\TBB\\lib\\windows_intel64_vc\\Debug\\*_debug.dll %{cfg.buildtarget.directory}",
			}
		
		configuration {"vs2015","windows","Release"}
			libdirs {"C:/Libs/TBB/lib/windows_intel64_vc/Release"}
			links{"tbb","tbbmalloc"}
			postbuildcommands {
				"{COPY} C:\\Libs\\TBB\\lib\\windows_intel64_vc\\Release\\*.dll %{cfg.buildtarget.directory}",
			}


	project "multicore_engine_pack_file_gen"
		kind "ConsoleApp"
		--kind "WindowedApp"
		language "C++"
		location "multicore_engine_pack_file_gen/build"
		files { "multicore_engine_pack_file_gen/include/**.hpp", "multicore_engine_pack_file_gen/src/**.cpp"}
		links {"multicore_engine_core","multicore_engine_parsers"}
		configuration {"vs2015"}
			debugdir "multicore_engine_pack_file_gen"
			debugargs "-d test.packdesc"

	project "multicore_engine_load_unit_gen"
		kind "ConsoleApp"
		--kind "WindowedApp"
		language "C++"
		location "multicore_engine_load_unit_gen/build"
		files { "multicore_engine_load_unit_gen/include/**.hpp", "multicore_engine_load_unit_gen/src/**.cpp"}
		links {"multicore_engine_core","multicore_engine_parsers"}
		configuration {"vs2015"}
			debugdir "multicore_engine_load_unit_gen"
			debugargs "-d test.lud"

