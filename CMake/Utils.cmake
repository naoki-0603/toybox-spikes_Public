# SPDX-License-Identifier: MIT
# Copyright (c) 2024-2026 naoki
# Licensed under the MIT License. See the LICENSE file in the project root,
# or visit https://opensource.org/licenses/MIT for details.

# Visual Studioでのみ有効
function(add_vs_filter files filter_name)
    foreach(file ${files})
        source_group(${filter_name} FILES ${file})
    endforeach()
endfunction()