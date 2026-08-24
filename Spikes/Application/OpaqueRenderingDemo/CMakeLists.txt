# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    OPAQUE_RENDERING_DEMO_SRC
    src/OpaqueRenderingDemo.hpp
    src/OpaqueRenderingDemo.cpp
)

setup_application(OpaqueRenderingDemo "${OPAQUE_RENDERING_DEMO_SRC}" SpikesKit_DX11)