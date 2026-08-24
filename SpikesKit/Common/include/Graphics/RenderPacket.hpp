// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_RENDER_PACKET_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_RENDER_PACKET_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct RenderPacket final
			{
				BufferResourceHandle m_vertexBufferHandle{};
				BufferResourceHandle m_indexBufferHandle{};
				
				u32 m_indexCount{};
				u32 m_startIndexLocation{};
				i32 m_baseVertexLocation{};

				BufferResourceHandle m_materialConstantBufferHandle{};

				ShaderResourceHandle m_vertexShaderHandle{};
				ShaderResourceHandle m_pixelShaderHandle{};

				TextureResourceHandle m_baseColorTextureHandle{};
				TextureResourceHandle m_metallicRoughnessTextureHandle{};
				TextureResourceHandle m_normalTextureHandle{};
				TextureResourceHandle m_occlusionTextureHandle{};
				TextureResourceHandle m_emissiveTextureHandle{};

				SamplerResourceHandle m_baseColorSamplerHandle{};
				SamplerResourceHandle m_metallicRoughnessSamplerHandle{};
				SamplerResourceHandle m_normalSamplerHandle{};
				SamplerResourceHandle m_occlusionSamplerHandle{};
				SamplerResourceHandle m_emissiveSamplerHandle{};

				Matrix m_worldTransform{};
			};

			struct RenderPacketsBundle final
			{
				std::vector<RenderPacket> m_opaqueRenderPackets{};
				std::vector<RenderPacket> m_maskRenderPackets{};
				std::vector<RenderPacket> m_transparentRenderPackets{};
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_RENDER_PACKET_HPP_