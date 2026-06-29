# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License.
# See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

function(setup_build_type)
    if(NOT CMAKE_CONFIGURATION_TYPES)
        set(
            CMAKE_CONFIGURATION_TYPES
            "Debug;Release;Dist"
            CACHE STRING "" FORCE
        )
    endif()
endfunction()

function(setup_compile_definitions)
    # プロジェクト全体で使用できるマクロの定義
    add_compile_definitions(TS_USE_CXX23)
    add_compile_definitions(
        $<$<CONFIG:Debug>:TS_DEBUG>
        $<$<CONFIG:Release>:TS_RELEASE>
        $<$<CONFIG:Dist>:TS_DIST>
    )
endfunction()

function(setup_third_party_libraries)
    
endfunction()