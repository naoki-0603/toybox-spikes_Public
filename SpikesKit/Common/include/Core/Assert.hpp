// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_ASSERT_HPP_
#define SPIKES_KIT_COMMON_CORE_ASSERT_HPP_

#include <source_location>

#include "../Helper/StringHelper.hpp"

namespace ts
{
	namespace kit
	{
		namespace internal
		{
			void Assert(std::string_view fileName, i32 line, std::string_view message);
		} // namespace internal

	} // namespace kit
} // namespace ts

#ifdef TS_DEBUG || TS_RELEASE
#define TS_ASSERT(Expression, FormattedText, ...) \
	do \
	{ \
		if (!(Expression)) \
		{ \
			const std::source_location location = std::source_location::current(); \
			ts::kit::internal::Assert(location.file_name(), location.line(), ts::kit::Printf(FormattedText,  __VA_ARGS__)); \
		}\
	} while (false)
#else // TS_DEBUG || TS_RELEASE
#define TS_ASSERT(Expression, FormattedText, ...)
#endif // !(TS_DEBUG || TS_RELEASE)
#endif //! SPIKES_KIT_COMMON_CORE_ASSERT_HPP_