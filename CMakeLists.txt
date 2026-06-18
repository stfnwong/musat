cmake_minimum_required(VERSION 3.21)

# ==== Project ==== #
project(
    smips,
    VERSION 0.1.0
	DESCRIPTION "A DPLL SAT Solver"
    LANGUAGES CXX
)


# ==== Global settings ==== #
# These settings apply to all targets 
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)    # -std=c++20 rather than -std=gnu++20

# Put executables and libraries in predictable places 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Export compile commands for clangd, vim, etc
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ==== Build type 
# If no build type is specified then default to Release 
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Build type not set, defauling to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE, CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel"
    )
endif()

# ==== Compiler warnings 
# Define a reusable interface target for warning flags 
# Targets opt-in by linking against this
add_library(sat_warnings INTERFACE)

target_compile_options(sat_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /WX
    >
)    
# ==== Sanitizers
add_library(sat_sanitizers INTERFACE)
target_compile_options(sat_sanitizers INTERFACE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    >
)

target_link_options(sat_sanitizers INTERFACE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
        -fsanitize=address,undefined
    >
)

# ==== Library logic 
add_library(solver src/solver.cpp)

target_include_directories(solver
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(solver PUBLIC cxx_std_20)

target_compile_options(solver
	PRIVATE
	$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_executable(sat apps/main.cpp)
target_link_libraries(sat PRIVATE solver)

if(PROJECT_IS_TOP_LEVEL) 
	include(CTest)    # for BUILD_TESTING and enable_testing()

	if(BUILD_TESTING)
		include(FetchContent)
		FetchContent_Declare(
			googletest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG        v1.15.2
		)

		# Don't let gtest clobber linker settings on windows
		set(gtest_forced_shared_crt ON CACHE BOOL "" FORCE)
		FetchContent_MakeAvailable(googletest)

		add_executable(solver_tests test/test_solver.cpp)
		target_link_libraries(solver_tests
			PRIVATE
				solver
				GTest::gtest_main
		)

		include(GoogleTest)
		gtest_discover_tests(solver_tests)
	endif()
endif()



# ==== Subdirectories
#add_subdirectory(src)
#
## Only build if this is the top-level project (not a sub-dep)
#if(PROJECT_IS_TOP_LEVEL)
#    enable_testing()
#    add_subdirectory(tests)
#endif()
