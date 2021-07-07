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

target_link_libraries(${TARGET_NAME} PUBLIC
  #stb
  spdlog
)

if (WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE _UNICODE UNICODE)
endif ()
