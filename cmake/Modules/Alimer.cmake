# Detect target platform
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
	set(ALIMER_PLATFORM_WINDOWS 1)
	set(ALIMER_PLATFORM_NAME "Win32")
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore" )
	set (WINDOWS_STORE ON CACHE BOOL "" FORCE)
	set(ALIMER_PLATFORM_UWP 1)
	set(ALIMER_PLATFORM_NAME "UWP")
	set(ALIMER_UWP_VERSION_MIN  "10.0.18362.0")
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )
	if (IOS)
		set(ALIMER_PLATFORM_IOS 1)
		set(ALIMER_PLATFORM_NAME "iOS")
	else()
		set(ALIMER_PLATFORM_OSX 1)
		set(ALIMER_PLATFORM_NAME "macOS")
	endif()
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
	set (LINUX ON CACHE BOOL "" FORCE)
	set(ALIMER_PLATFORM_LINUX 1)
	set(ALIMER_PLATFORM_NAME "Linux")
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Android" )
	set(ALIMER_PLATFORM_ANDROID 1)
	set(ALIMER_PLATFORM_NAME "Android")
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" )
	set(ALIMER_PLATFORM_WEB 1)
	set(ALIMER_PLATFORM_NAME "Web")
else()
	message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Define standard configurations
if( CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_CONFIGURATION_TYPES MATCHES "Debug;Dev;Release" )
	set(CMAKE_CONFIGURATION_TYPES "Debug;Dev;Release" CACHE STRING "List of supported configurations." FORCE)

    set(CMAKE_BUILD_TYPE Dev CACHE STRING "The default build type")
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

    # Copy settings
    set(CMAKE_EXE_LINKER_FLAGS_DEV ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_DEV ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
	set(CMAKE_STATIC_LINKER_FLAGS_DEV ${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_DEV ${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS_DEV ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS_DEV ${CMAKE_C_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
	set(CMAKE_CSharp_FLAGS_DEV ${CMAKE_CSharp_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
	set(CMAKE_RC_FLAGS_DEV ${CMAKE_RC_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)

	mark_as_advanced(FORCE CMAKE_EXE_LINKER_FLAGS_DEV)
	mark_as_advanced(FORCE CMAKE_SHARED_LINKER_FLAGS_DEV)
	mark_as_advanced(FORCE CMAKE_STATIC_LINKER_FLAGS_DEV)
	mark_as_advanced(FORCE CMAKE_MODULE_LINKER_FLAGS_DEV)
	mark_as_advanced(FORCE CMAKE_CXX_FLAGS_DEV)
	mark_as_advanced(FORCE CMAKE_C_FLAGS_DEV)
	mark_as_advanced(FORCE CMAKE_CSharp_FLAGS_DEV)
	mark_as_advanced(FORCE CMAKE_RC_FLAGS_DEV)
endif()

# Options
include(CMakeDependentOption)
option(ALIMER_BUILD_SHARED "Build engine as shared library" ON)
option(ALIMER_BUILD_SAMPLES "Build sample projects" ON)
option(ALIMER_ENABLE_RTTI "Enables RTTI" OFF)
option(ALIMER_ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" OFF)
option(ALIMER_PROFILING "Enable performance profiling" ON)
option(ALIMER_THREADING "Enable multithreading" ON)
option(ALIMER_NETWORK "Enable Networking system " ON)
option(ALIMER_PHYSICS "Enable Physics system" ON)
option(ALIMER_BUILD_ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" OFF)

if (IOS OR EMSCRIPTEN)
    set(ALIMER_BUILD_SHARED OFF CACHE BOOL "Always Disable shared library on (IOS, WEB)" FORCE)
elseif(ANDROID)
    set(ALIMER_BUILD_SHARED ON CACHE BOOL "Always enable shared library on (ANDROID)" FORCE)
endif ()

# Graphics backends
if (WIN32)
	option(ALIMER_RHI_D3D12 "RHI Direct3D12 backend" ON)
	
    if (NOT WINDOWS_STORE)
    	option(ALIMER_RHI_VULKAN "RHI Vulkan backend" ON)
	endif ()
elseif(APPLE)
	set(ALIMER_RHI_METAL ON CACHE BOOL "Use Metal Graphics API" FORCE)
else()
	set(ALIMER_RHI_VULKAN ON CACHE BOOL "RHI Vulkan backend" FORCE)
endif ()

if (ANDROID OR IOS OR EMSCRIPTEN)
	set(ALIMER_CSHARP OFF CACHE INTERNAL "Build C# bindings" FORCE)
	set(ALIMER_BUILD_TOOLS OFF CACHE INTERNAL "Disable Build tools" FORCE)
else()
    option(ALIMER_CSHARP "Enable C# bindings" ON)
	option(ALIMER_BUILD_TOOLS "Build tools and editors" ON)
endif()

# Install directories
set (INSTALL_BASE_INCLUDE_DIR include)
set (INSTALL_INCLUDE_DIR ${INSTALL_BASE_INCLUDE_DIR}/Alimer)
set (INSTALL_THIRDPARTY_DIR ${INSTALL_INCLUDE_DIR}/ThirdParty)
set (DEST_SHARE_DIR share)

# Compiler-specific setup
if(MSVC)
	# TODO: manage CLANG MSVC
	if (NOT ALIMER_ENABLE_RTTI)
		string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	endif ()

	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SUBSYSTEM:console /ENTRY:wWinMainCRTStartup")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:windows /ENTRY:wWinMainCRTStartup")
endif ()

function(alimer_print_options)
    message(STATUS "Alimer Engine version ${ALIMER_VERSION}")
    message(STATUS "Build platform: ${ALIMER_PLATFORM_NAME}")

    if (ALIMER_BUILD_SHARED)
        message(STATUS "  Library         SHARED")
    else ()
	    message(STATUS "  Library         STATIC")
    endif ()
    message(STATUS "  Profiling       ${ALIMER_PROFILING}")
    message(STATUS "  Threading       ${ALIMER_THREADING}")
    if (ALIMER_D3D11)
        message(STATUS "  Graphics API:   Direct3D11 (ALIMER_D3D11)")
    endif()
    if (ALIMER_D3D12)
        message(STATUS "  Graphics API:   Direct3D12 (ALIMER_D3D12)")
    endif()
    if (ALIMER_VULKAN)
        message(STATUS "  Graphics API:   Vulkan (ALIMER_VULKAN)")
    endif()
    message(STATUS "  Network         ${ALIMER_NETWORK}")
    message(STATUS "  Physics         ${ALIMER_PHYSICS}")
endfunction()

macro (alimer_define_engine_source_files)
	cmake_parse_arguments(DEFINE_SRC_FILES "NO_INSTALL" "" "" ${ARGN} )

	foreach (path ${DEFINE_SRC_FILES_UNPARSED_ARGUMENTS})
		# Get header files
		file (GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${path}/*.hpp ${path}/*.h ${path}/*.inl)
		list (APPEND HEADER_FILES ${_files})

		# Install them
		file (GLOB INSTALL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${path}/*.hpp ${path}/*.h ${path}/*.inl)

		if (NOT DEFINE_SRC_FILES_NO_INSTALL)
			install (
				DIRECTORY ${path} 
				DESTINATION include
				FILES_MATCHING
				PATTERN "*.h"
				PATTERN "*.hpp"
				PATTERN "Private" EXCLUDE
			)
		endif ()

		# Get source files
		file (GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${path}/*.c ${path}/*.cpp)
		list (APPEND SOURCE_FILES ${_files})

		# Get natvis files for MSVC
		if (MSVC)
			file (GLOB NATVIS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${path}/*.natvis)
			list (APPEND SOURCE_FILES ${NATVIS_FILES})
		endif ()
	endforeach ()
endmacro()

# Groups sources into subfolders.
macro(alimer_group_sources)
	file (GLOB_RECURSE children LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/**)
	foreach (child ${children})
		if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
		  string(REPLACE "/" "\\" groupname "${child}")
		  file (GLOB files LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*)
		  source_group(${groupname} FILES ${files})
		endif ()
	endforeach ()
endmacro()