# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    TRANSPARENT_RENDERING_DEMO_SRC  
    src/TransparentRenderingDemo.hpp
    src/TransparentRenderingDemo.cpp
)

setup_application(TransparentRenderingDemo "${TRANSPARENT_RENDERING_DEMO_SRC}" SpikesKit_DX11)