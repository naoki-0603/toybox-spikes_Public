# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

function(setup_imgui)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG 6d910d5487d11ca567b61c7824b0c78c569d62f0 # v1.92.5
    )

    FetchContent_MakeAvailable(imgui)

    set(
        IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )

    add_library(
        TP_IMGUI STATIC ${IMGUI_SOURCES}
    )

    target_include_directories(
        TP_IMGUI PUBLIC ${imgui_SOURCE_DIR}
    )

    target_link_libraries(
        TP_IMGUI PUBLIC Vulkan::Vulkan
    )
endfunction()

function(setup_google_test)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 # release-1.17.0
    )

    FetchContent_MakeAvailable(googletest)
endfunction()

function(setup_directx_math)
    FetchContent_Declare(
        DirectXMath
        GIT_REPOSITORY https://github.com/microsoft/DirectXMath.git
        GIT_TAG 56206b1916646b29b37e1eac56e96e2008d6c71f # Windows SDK (10.0.28000)
    )
    
    FetchContent_MakeAvailable(DirectXMath)
endfunction()