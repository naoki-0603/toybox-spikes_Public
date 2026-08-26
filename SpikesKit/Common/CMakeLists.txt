# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License.
# See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

# CONFIGURE_DEPENDS をつけることで、ファイル追加時に自動でCMakeが再構築される。
file(GLOB_RECURSE COMMON_INCLUDES CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_vs_filter("${COMMON_INCLUDES}" "include")
add_vs_filter("${COMMON_SOURCES}" "src")

set(DEPENDED_MODULE_NAME "TP_IMGUI" "DirectXMath" "Tracy::TracyClient")

set(COMMON_INCLUDE_DIRECTORIES 
    "${THIRD_PARTY_STB_INCLUDES}"
    "${THIRD_PARTY_TINYGLTF_INCLUDES}"
    "${PROJECT_SOURCE_DIR}/SpikesKit"
    "${PROJECT_SOURCE_DIR}/SpikesKit/Common/include"
)

setup_library(
    SpikesKit_Common 
    "${COMMON_INCLUDES}" 
    "${COMMON_SOURCES}" 
    "${DEPENDED_MODULE_NAME}" 
    "${COMMON_INCLUDE_DIRECTORIES}"
    "${PROJECT_SOURCE_DIR}/SpikesKit/SpikesKitPCH.hpp"
)