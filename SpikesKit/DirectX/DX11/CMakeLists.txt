# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

file(GLOB_RECURSE DIRECTX11_INCLUDES CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE DIRECTX11_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_vs_filter("${DIRECTX11_INCLUDES}" "include")
add_vs_filter("${DIRECTX11_SOURCES}" "src")

set(DEPENDED_MODULE_NAME "SpikesKit_Common" "${TS_DIRECTX11_LIBRARIES}" "DirectXTK")

setup_plugin(
    SpikesKit_DX11
    "${DIRECTX11_INCLUDES}"
    "${DIRECTX11_SOURCES}"
    "${DEPENDED_MODULE_NAME}"
    "${PROJECT_SOURCE_DIR}/SpikesKit/DirectX/DX11/include"
    "${PROJECT_SOURCE_DIR}/SpikesKit/DirectX/DX11/SpikesKitDX11PCH.hpp"
)