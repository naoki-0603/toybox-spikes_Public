# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License.

# プラグインをセットアップする
function(setup_plugin plugin_name header_files source_files dependent_module_names include_directory_paths pch_include_path)
    add_library(${plugin_name} SHARED ${header_files} ${source_files})

    target_link_libraries(${plugin_name} PUBLIC ${dependent_module_names})
    target_include_directories(${plugin_name} PUBLIC ${include_directory_paths})

    if (pch_include_path)
        target_precompile_headers(${plugin_name} PUBLIC ${pch_include_path})
    endif()
endfunction()

# ライブラリをセットアップする
function(setup_library library_name library_headers library_sources dependent_module_names include_directory_paths pch_include_path)
    add_library(${library_name} STATIC ${library_headers} ${library_sources})

    target_include_directories(${library_name} PUBLIC ${include_directory_paths})
    target_link_libraries(${library_name} PUBLIC ${dependent_module_names})

    if (pch_include_path)
        target_precompile_headers(${library_name} PUBLIC ${pch_include_path})
    endif()
endfunction()

# テストアプリケーションをセットアップする
function(setup_test_application test_app_name test_app_sources dependent_module_names)
    add_executable(${test_app_name} ${test_app_sources}) 
    
    set_target_properties(${test_app_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Spikes/Test/${test_app_name}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Spikes/Test/${test_app_name}/Release"
    )

    target_link_libraries(${test_app_name} PRIVATE ${dependent_module_names} GTest::gtest GTest::gtest_main)
    add_test(NAME ${test_app_name} COMMAND ${test_app_name})
endfunction()

# アプリケーションをセットアップする
function(setup_application app_name app_sources dependent_module_names)
    add_executable(${app_name} ${app_sources})
    
    set_target_properties(${app_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Spikes/Application/${app_name}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Spikes/Application/${app_name}/Release"
    )

    target_link_libraries(${app_name} PRIVATE ${dependent_module_names})
endfunction()