# CMake version
cmake_minimum_required (VERSION 3.19)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds are not supported. Please choose a different binary directory.")
	return()
endif()

# Configure CMake global variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Project declaration
project (Alimer)

# Set CMake modules search path
if(CMAKE_MODULE_PATH)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
else ()
    set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
endif()

# Include Engine cmake utilities and options
include(Alimer)

# Build version
set (ALIMER_VERSION_MAJOR 0)
set (ALIMER_VERSION_MINOR 1)
set (ALIMER_VERSION_PATCH 0)
set (ALIMER_VERSION ${ALIMER_VERSION_MAJOR}.${ALIMER_VERSION_MINOR}.${ALIMER_VERSION_PATCH})
set (ALIMER_VERSION_ALIAS "WIP")

execute_process(COMMAND git rev-list --count HEAD
    OUTPUT_VARIABLE ALIMER_REVISION_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if ("${ALIMER_REVISION_ID}" STREQUAL "")
    # We don't have any source control so just hard-code all of this information to empty
    set (ALIMER_REVISION_ID 0)
    set (ALIMER_SHORT_CHANGE_SET 0)
    set (ALIMER_CHANGE_SET 0)
    set (ALIMER_CHANGE_SET_DATE "")
else ()
    execute_process(COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE ALIMER_SHORT_CHANGE_SET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND git rev-parse HEAD
        OUTPUT_VARIABLE ALIMER_CHANGE_SET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND git log -1 --abbrev=12 --date=format:%Y-%m-%d --pretty=format:%cd
        OUTPUT_VARIABLE ALIMER_CHANGE_SET_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
        
    message("GIT Revision ID: ${ALIMER_REVISION_ID}")
    message("GIT Short ChangeSet: ${ALIMER_SHORT_CHANGE_SET}")
    message("GIT ChangeSet: ${ALIMER_CHANGE_SET}")
    message("GIT ChangeSet Date: ${ALIMER_CHANGE_SET_DATE}")
endif ()

# Root and build dir.
set (ALIMER_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (ALIMER_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/ThirdParty)
mark_as_advanced(ALIMER_ROOT_DIR)
mark_as_advanced(ALIMER_THIRD_PARTY_DIR)

# Configure CMake global variables
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

if (NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

# Source
add_subdirectory(Source)

# Print current build configuration
alimer_print_options()

# Install files
# README.md and license
install (FILES
	${CMAKE_CURRENT_SOURCE_DIR}/README.md
	${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
	DESTINATION .
)

# Install CMake modules and toolchains provided by and for Urho3D
install (DIRECTORY ${CMAKE_SOURCE_DIR}/cmake/Modules/ DESTINATION ${DEST_SHARE_DIR}/cmake/Modules/)

# Install data files
file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT ANDROID)
	foreach(dir Assets)
		install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir} DESTINATION bin/)
	endforeach()

	# dxil and dxcompiler
	if(WIN32 AND NOT (WINDOWS_STORE))
		set(DXIL_LIBRARY "${ALIMER_THIRD_PARTY_DIR}/dxc/bin/win-x64/dxil.dll")
		set(DXCOMPILER_LIBRARY "${ALIMER_THIRD_PARTY_DIR}/dxc/bin/win-x64/dxcompiler.dll")

		foreach(config ${CMAKE_CONFIGURATION_TYPES})
			file (COPY ${DXIL_LIBRARY} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${config})
			file (COPY ${DXCOMPILER_LIBRARY} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${config})
		endforeach()

		install (FILES
			${DXIL_LIBRARY}
			${DXCOMPILER_LIBRARY}
			DESTINATION ${DEST_BIN_DIR_CONFIG}
		)
	endif ()
endif ()
