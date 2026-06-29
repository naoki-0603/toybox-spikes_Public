# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    SLOT_MAP_TEST_SRC
    src/SlotMap.hpp
    src/SlotMap.cpp
)

setup_test_application(SlotMap "${SLOT_MAP_TEST_SRC}" SpikesKit_Common)