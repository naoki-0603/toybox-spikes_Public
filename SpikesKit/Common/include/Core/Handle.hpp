// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_HANDLE_HPP_
#define SPIKES_KIT_COMMON_CORE_HANDLE_HPP_

namespace ts
{
	namespace kit
	{
		struct Handle
		{
			using ID = u64;
		public:
			[[nodiscard]]
			static Handle Create(u32 generation, u32 index) noexcept
			{
				Handle handle;

				// 00000000 00000000 00000000 00000000 generation (0 ～ 31)
				// 00000000 00000000 00000000 00000000 index (32 ～ 63)
				handle.m_id = (static_cast<u64>(generation) << 32) | static_cast<u64>(index);
				return handle;
			}
		public:
			[[nodiscard]]
			u32 GetIndex() const noexcept { return static_cast<u32>(m_id) & 0xFFFFFFFF; }

			[[nodiscard]]
			u32 GetGeneration() const noexcept { return static_cast<u32>(m_id >> 32) & 0xFFFFFFFF; }

			bool IsValid() const noexcept { return m_id != k_invalidID; }
			bool operator==(const Handle& rhs) const { return (m_id == rhs.m_id); }
			bool operator!=(const Handle& rhs) const { return (m_id != rhs.m_id); }
		protected:
			Handle() = default;
			Handle(ID id) : m_id(id) {}

		private:
			ID m_id;

		private:
			static constexpr ID k_invalidID = 0u;
		};

		template <class T>
		struct TypedHandle final : Handle
		{
			TypedHandle() = default;
			explicit TypedHandle(u64 id) : Handle(id) {}
			explicit TypedHandle(const Handle& baseHandle) : Handle(baseHandle) {}

			[[nodiscard]]
			static TypedHandle<T> Create(u32 generation, u32 index) noexcept
			{
				return TypedHandle<T>(Handle::Create(generation, index));
			}
		};

		// GPU用リソースハンドル
		struct ShaderResourceTag {};
		struct MeshResourceTag {};
		struct PipelineStateTag {};
		struct BufferTag {};
		struct ShaderBindingsLayoutTag {};
		struct TextureTag {};
		struct TextureViewTag {};
		struct SamplerTag {};

		// GPU用リソースハンドルのエイリアス
		using ShaderResourceHandle = TypedHandle<ShaderResourceTag>;
		using MeshResourceHandle = TypedHandle<MeshResourceTag>;
		using PipelineStateResourceHandle = TypedHandle<PipelineStateTag>;
		using BufferResourceHandle = TypedHandle<BufferTag>;
		using ShaderBindingsLayoutResourceHandle = TypedHandle<ShaderBindingsLayoutTag>;
		using TextureResourceHandle = TypedHandle<TextureTag>;
		using TextureViewResourceHandle = TypedHandle<TextureViewTag>;
		using SamplerResourceHandle = TypedHandle<SamplerTag>;

		namespace asset
		{
			struct ShaderAsset;
			struct TextureAsset;
			struct MaterialAsset;
			struct StaticMeshAsset;
		} // namespace asset

		using ShaderAssetHandle = TypedHandle<asset::ShaderAsset>;
		using TextureAssetHandle = TypedHandle<asset::TextureAsset>;
		using MaterialAssetHandle = TypedHandle<asset::MaterialAsset>;
		using StaticMeshAssetHandle = TypedHandle<asset::StaticMeshAsset>;
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_HANDLE_HPP_