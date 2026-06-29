// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_LOG_HPP_
#define SPIKES_KIT_COMMON_CORE_LOG_HPP_

// reference
// https://an-embedded-engineer.hateblo.jp/entry/2020/08/29/230546

#include "../Helper/StringHelper.hpp"

namespace ts
{
	namespace kit
	{
		namespace internal
		{
			const std::string k_consoleTextColorReset = "\x1b[0m";
			const std::string k_consoleTextColorRed = "\x1b[31m";
			const std::string k_consoleTextColorGreen = "\x1b[32m";
			const std::string k_consoleTextColorYellow = "\x1b[33m";
			const std::string k_consoleTextColorBlue = "\x1b[34m";
			const std::string k_consoleTextColorWhite = "\x1b[37m";
		} // namespace internal
	} // namespace kit
} // namespace ts

#define TS_DEBUG_LOG(Log, ...) \
	do \
	{ \
		const std::string formattedText = ts::kit::Printf(Log, __VA_ARGS__); \
		const std::string log = ts::kit::internal::k_consoleTextColorGreen + "[Debug]: " + formattedText + ts::kit::internal::k_consoleTextColorReset; \
		std::println("{}", log); \
	} while (false)

#define TS_INFO_LOG(Log, ...) \
	do \
	{ \
		const std::string formattedText = ts::kit::Printf(Log, __VA_ARGS__); \
		const std::string log = ts::kit::internal::k_consoleTextColorGreen + "[Info]: " + formattedText + ts::kit::internal::k_consoleTextColorReset; \
		std::println("{}", log); \
	} while (false)

#define TS_WARNING_LOG(Log, ...)  \
	do \
	{ \
		const std::string formattedText = ts::kit::Printf(Log, __VA_ARGS__); \
		const std::string log = ts::kit::internal::k_consoleTextColorYellow + "[Warning]: " + formattedText + ts::kit::internal::k_consoleTextColorReset; \
		std::println("{}", log); \
	} while (false)

#define TS_ERROR_LOG(Log, ...)  \
	do \
	{ \
		const std::string formattedText = ts::kit::Printf(Log, __VA_ARGS__); \
		const std::string log = ts::kit::internal::k_consoleTextColorRed + "[Error]: " + formattedText + ts::kit::internal::k_consoleTextColorReset; \
		std::println("{}", log); \
	} while (false)

#define TS_FATAL_LOG(Log, ...) \
	do \
	{ \
		const std::string formattedText = ts::kit::Printf(Log, __VA_ARGS__); \
		const std::string log = ts::kit::internal::k_consoleTextColorRed + "[Fatal]: " + formattedText + ts::kit::internal::k_consoleTextColorReset; \
		std::println("{}", log); \
		abort(); \
	} while (false)

#endif //! SPIKES_KIT_COMMON_CORE_LOG_HPP_