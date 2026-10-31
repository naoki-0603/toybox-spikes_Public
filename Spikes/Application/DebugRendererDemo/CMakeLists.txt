# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    DEBUG_RENDERING_DEMO_SRC  
    src/DebugRendererDemo.hpp
    src/DebugRendererDemo.cpp
)

setup_application(DebugRendererDemo "${DEBUG_RENDERING_DEMO_SRC}" SpikesKit_DX11)