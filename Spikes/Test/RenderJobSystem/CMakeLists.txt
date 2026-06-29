# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    RENDER_JOB_SYSTEM_TEST_SRC
    src/RenderJobSystemTest.hpp
    src/RenderJobSystemTest.cpp
)

setup_test_application(RenderJobSystem "${RENDER_JOB_SYSTEM_TEST_SRC}" SpikesKit_Common)