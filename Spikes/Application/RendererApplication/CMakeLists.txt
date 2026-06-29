# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    RENDERER_APPLICATION_SRC
    src/RendererApplication.hpp
    src/RendererApplication.cpp
)

setup_application(RendererApplication "${RENDERER_APPLICATION_SRC}" SpikesKit_DX11)