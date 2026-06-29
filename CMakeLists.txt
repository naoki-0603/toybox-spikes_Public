# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License.
# See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

cmake_minimum_required(VERSION 3.20)
project(ToyboxSpikes VERSION 0.1 LANGUAGES CXX)

if (MSVC)
    # 文字コードをutf-8に強制する
    add_compile_options(/utf-8)
endif()

option(ENABLE_TESTING "テストを有効にする" ON)

# モジュール検索パスの追加
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/ThirdParty")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- RUNTIME (DLL / EXE) の出力先 ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Bin/Plugins/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Bin/Plugins/Release")

# --- LIBRARY (Static LIB / DLL Import LIB) の出力先 ---
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Bin/Lib/Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Bin/Lib/Release")

# --- ARCHIVE (Static LIB) の出力先 ---
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Bin/Lib/Debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Bin/Lib/Release")

include(FetchContent)
include(Configuration)
include(GraphicsAPI)
include(SetupThirdPartyLibraries)
include(Setup)
include(Utils)

setup_build_type()
setup_compile_definitions()

setup_directx11()
setup_directx11_xtk()
setup_directx12()
setup_vulkan()

setup_imgui()
setup_google_test()
setup_directx_math()

if (ENABLE_TESTING)
    enable_testing()
endif()

add_subdirectory(SpikesKit)
add_subdirectory(Spikes)