// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/Assert.hpp"
#include "Core/Log.hpp"

namespace ts
{
	namespace kit
	{
		namespace internal
		{
			void Assert(std::string_view fileName, i32 line, std::string_view message)
			{
				const std::string fullMessage = "Message: " +
					std::string(message) +
					" FileName: " +
					std::string(fileName) +
					" LineNo: " +
					std::to_string(line);

				TS_FATAL_LOG("{}", fullMessage);
			}
		} // namespace internal

	} // namespace kit
} // namespace ts
