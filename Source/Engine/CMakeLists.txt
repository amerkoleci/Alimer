# Define target name
set (TARGET_NAME Alimer)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/AlimerConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/AlimerConfig.h)

# Add root-level files
file (GLOB HEADER_FILES *.h)
file (GLOB SOURCE_FILES *.c *.cpp)
source_group("" FILES ${HEADER_FILES})
source_group("" FILES ${SOURCE_FILES})

alimer_define_engine_source_files(
  Core
  RHI
  Assets
)

# Include platform files.
alimer_define_engine_source_files(Platform/Private NO_INSTALL)
alimer_define_engine_source_files(Platform/${ALIMER_PLATFORM_NAME} NO_INSTALL)

alimer_group_sources()

if (ALIMER_BUILD_SHARED)
  add_library(${TARGET_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES} ${CMAKE_CURRENT_BINARY_DIR}/AlimerConfig.h)
  #set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

  target_compile_definitions(${TARGET_NAME}
    PRIVATE -DALIMER_EXPORTS=1
  	PUBLIC -DALIMER_IMPORTS=1
  )
else()
  add_library(${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${CMAKE_CURRENT_BINARY_DIR}/AlimerConfig.h)

  target_compile_definitions (${TARGET_NAME} PUBLIC -DALIMER_STATIC)
endif ()

target_include_directories(${TARGET_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/ImGui
  $<INSTALL_INTERFACE:${INSTALL_BASE_INCLUDE_DIR}>
  $<INSTALL_INTERFACE:${INSTALL_THIRDPARTY_DIR}>
)

if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
  # Warnings as Errors Enabled
  target_compile_options(${TARGET_NAME} PRIVATE /W3 /WX)

  # Multiprocessor compiler
  target_compile_options(${TARGET_NAME} PRIVATE /MP)

  target_compile_options(${TARGET_NAME} PRIVATE /fp:fast)
  target_compile_options(${TARGET_NAME} PRIVATE /permissive- /JMC- /Zc:__cplusplus)

  if(ALIMER_BUILD_ENABLE_CODE_ANALYSIS)
    target_compile_options(${TARGET_NAME} PRIVATE /analyze)
  endif()

  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
    target_compile_options(${TARGET_NAME} PRIVATE /Zc:preprocessor /wd5104 /wd5105)
  endif()
else()
  target_compile_options(${TARGET_NAME} PUBLIC
    -fno-exceptions
	$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  )

  # Warnings as Errors Enabled
  target_compile_options(${TARGET_NAME} PRIVATE -Werror)
endif()

target_link_libraries(${TARGET_NAME} PUBLIC
  #stb
  spdlog
)

if (ALIMER_RHI_D3D12)
  target_link_libraries(${TARGET_NAME} PRIVATE D3D12MemoryAllocator)
endif ()

if (ALIMER_RHI_VULKAN)
  target_link_libraries(${TARGET_NAME} PRIVATE volk vma spirv-reflect)
endif()

if (WIN32)
  target_compile_definitions(${TARGET_NAME} PRIVATE _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS)
elseif (WINDOWS_STORE)
  target_link_libraries(${TARGET_NAME} PRIVATE d3d12 dxgi)
endif ()
