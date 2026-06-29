# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

set(
    JOB_SYSTEM_TEST_SRC
    src/JobSystemTest.hpp
    src/JobSystemTest.cpp
)

add_executable("RenderJobSystemTest" ${RENDER_JOB_SYSTEM_TEST_SRC})

set_target_properties(RenderJobSystemTest PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:RenderJobSystemTest>"
)