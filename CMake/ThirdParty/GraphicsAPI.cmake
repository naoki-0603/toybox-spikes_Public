# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

function(setup_directx11)
    set(
    TS_DIRECTX11_LIBRARIES 
    d3d11.lib dxgi.lib D3DCompiler.lib
    CACHE STRING "DirectX11 Libraries" FORCE 
)
endfunction()

function(setup_directx11_xtk)
    FetchContent_Declare(
        DirectX11XTK
        GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
        GIT_TAG 5a8f5d01cc1328e6451b588617c9985fa5a2a8ab # may2026
    )

    FetchContent_MakeAvailable(DirectX11XTK)
endfunction()

function(setup_directx12)
    set(
        TS_DIRECTX12_LIBRARIES
        d3d12.lib dxgi.lib dxguid.lib 
        CACHE STRING "DirectX12 Libraries" FORCE
    )
endfunction()

function(setup_vulkan)
    find_package(Vulkan REQUIRED)
endfunction()