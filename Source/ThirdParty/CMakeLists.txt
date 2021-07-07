cmake_minimum_required(VERSION 3.19)

project(alimer_third_party LANGUAGES C CXX)

# spdlog
add_subdirectory(spdlog EXCLUDE_FROM_ALL)
set_property(TARGET spdlog PROPERTY FOLDER "ThirdParty")

if (ALIMER_RHI_D3D12)
    add_library(DirectX-Headers INTERFACE)
    set(DIRECTX_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DirectX-Headers/include)
    target_sources(DirectX-Headers INTERFACE 
        ${DIRECTX_HEADERS_INCLUDE_DIR}/directx/d3d12.h
        ${DIRECTX_HEADERS_INCLUDE_DIR}/directx/d3dx12.h
        ${DIRECTX_HEADERS_INCLUDE_DIR}/directx/d3d12shader.h
    )
    target_include_directories(DirectX-Headers INTERFACE ${DIRECTX_HEADERS_INCLUDE_DIR})

    # D3D12MemoryAllocator
    set(D3D12MA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/D3D12MemoryAllocator/src")
    set(D3D12MA_FILES
        "${D3D12MA_DIR}/D3D12MemAlloc.cpp"
        "${D3D12MA_DIR}/D3D12MemAlloc.h"
        "${D3D12MA_DIR}/D3D12MemAlloc.natvis"
    )
 
    add_library(D3D12MemoryAllocator STATIC ${D3D12MA_FILES})
    target_include_directories(D3D12MemoryAllocator PUBLIC ${D3D12MA_DIR})
    target_link_libraries(D3D12MemoryAllocator DirectX-Headers)
    set_property(TARGET D3D12MemoryAllocator PROPERTY FOLDER "ThirdParty")
endif ()
