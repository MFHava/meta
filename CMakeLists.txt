
#          Copyright Michael Florian Hava.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.26)
project(Meta)

find_package(Boost REQUIRED COMPONENTS preprocessor)

add_library(meta INTERFACE)
	target_compile_features(meta INTERFACE cxx_std_20)
	target_include_directories(meta INTERFACE "inc")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		target_compile_options(meta INTERFACE -Wall -Wextra -Wpedantic -Wconversion)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(meta INTERFACE
			# warning flags
			/W4 # enable all sensible warnings
			/wd4003 # ignore not enough parameters for function macro
			/wd4099 # ignore mixing struct and class keyword for same type
			/wd4456 # ignore local shadowing
			/wd4457 # ignore function parameter shadowing
			/wd4458 # ignore class member shadowing
			/wd4459 # ignore global shadowing
			/we4700 # treat use of uninitialized variables as compile error
			# conformance flags
			/Zc:__cplusplus # update __cplusplus according to standard (stays at C++98 otherwise)
			/Zc:preprocessor # enable standard conformant preprocessor
			# miscellaneous flags
			/MP # parallel compilation
			/JMC # improve debugging support by only stepping into own code
			/bigobj # use "big object files" to circumvent class of errors that are related to heavy template or machine generated code
		)
	endif()
	target_link_libraries(meta INTERFACE Boost::preprocessor)

option(META_BUILD_TESTS "Build tests" OFF)
if(META_BUILD_TESTS)
	add_executable(test-meta)
		file(GLOB_RECURSE META CONFIGURE_DEPENDS "inc/*")
			source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc FILES ${META})
			target_sources(test-meta PRIVATE ${META}) #including META sources here to have them included in IDEs
		file(GLOB_RECURSE META CONFIGURE_DEPENDS "test/*")
			source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/test FILES ${META})
			target_sources(test-meta PRIVATE ${META})
		target_link_libraries(test-meta PRIVATE meta)
endif()


option(META_BUILD_DOCUMENTATION "Build documentation" OFF)
if(META_BUILD_DOCUMENTATION)
	find_package(Doxygen 1.9.1)

	set(DOXYGEN_PROJECT_NAME "C++ Metadata")
	set(DOXYGEN_PROJECT_BRIEF "Header-only library for C++ compile-time metadata")
	set(DOXYGEN_OUTPUT_DIRECTORY "docs")
	set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
	set(DOXYGEN_SHOW_USED_FILES NO)
	set(DOXYGEN_SHOW_FILES NO)
	set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
	set(DOXYGEN_FILE_PATTERNS "*.hpp")
	set(DOXYGEN_EXCLUDE_SYMBOLS "meta::internal*")
	set(DOXYGEN_GENERATE_TREEVIEW YES)
	set(DOXYGEN_MACRO_EXPANSION YES)
	set(DOXYGEN_COLLABORATION_GRAPH NO)
	doxygen_add_docs(Documentation "inc;docs")
endif()
