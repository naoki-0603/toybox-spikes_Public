// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details.

#ifndef SPIKES_KIT_COMMON_CORE_BASE_HPP_
#define SPIKES_KIT_COMMON_CORE_BASE_HPP_

#define TS_EXPORT_API extern "C" __declspec(dllexport)

#include <type_traits>

#define TS_DEFINE_ENUM_BIT_OPERATORS(Enum) \
	inline Enum operator|(Enum lhs, Enum rhs) \
	{ \
		using underlying = std::underlying_type_t<Enum>; \
		return static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs)); \
	} \
	inline Enum& operator|=(Enum& lhs, Enum rhs) \
	{ \
		using underlying = std::underlying_type_t<Enum>; \
		lhs = static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs)); \
		return lhs; \
	} \
	inline Enum operator&(Enum lhs, Enum rhs) \
	{ \
		using underlying = std::underlying_type_t<Enum>; \
		return static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs)); \
	} \
	inline Enum& operator&=(Enum& lhs, Enum rhs) \
	{ \
		using underlying = std::underlying_type_t<Enum>; \
		lhs = static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs)); \
		return lhs; \
	} \
	inline bool operator==(Enum lhs, Enum rhs) \
	{ \
		using underlying = std::underlying_type_t<Enum>; \
		return static_cast<bool>(static_cast<underlying>(lhs) == static_cast<underlying>(rhs)); \
	} \
	inline bool operator!=(Enum lhs, Enum rhs) \
	{ \
		using underlying = std::underlying_type_t<Enum>; \
		return static_cast<bool>(static_cast<underlying>(lhs) != static_cast<underlying>(rhs)); \
	} \

#endif //! SPIKES_KIT_COMMON_CORE_BASE_HPP_