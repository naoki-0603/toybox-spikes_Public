# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    GRAPHICS_HELPER_TEST_SRC
    src/GraphicsHelperTest.hpp
    src/GraphicsHelperTest.cpp
)

setup_test_application(GraphicsHelperTest "${GRAPHICS_HELPER_TEST_SRC}" SpikesKit_Common)