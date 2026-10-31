// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_RENDERER_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_RENDERER_HPP_

#include "DebugVertex.hpp"

namespace ts
{
	namespace kit
	{
		namespace resource
		{
			class RenderResourceManager;
		} // namespace resource

		namespace graphics
		{
			namespace internal
			{
				static constexpr u32 k_maxVertices = 0xFFFFu * 2u;
			} // namespace internal

			struct DebugLineParameters final
			{
				Vector3 m_begin{};
				Vector3 m_end{};

				Vector4 m_beginColor{ 1.0f, 1.0f, 1.0f, 1.0f };
				Vector4 m_endColor{ 1.0f, 1.0f, 1.0f, 1.0f };
			};

			struct DebugCircleParameters final
			{
				Vector3 m_center{};
				f32 m_radius{ 0.5f };
				Quaternion m_rotation{};
				Vector4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
				u32 m_divisionCount{ 8u };
			};

			struct DebugHalfCircleParameters final
			{
				Vector3 m_center{};
				f32 m_radius{ 0.5f };
				Quaternion m_rotation{};
				Vector4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
				u32 m_divisionCount{ 4u };
			};

			struct DebugBoxParameters final
			{
				Vector3 m_max{};
				Vector3 m_min{};

				Vector4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
			};

			struct DebugSphereParameters final
			{
				Vector3 m_center{};
				f32 m_radius{}; // 単位はメートル

				Vector4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
			};

			struct DebugConeParameters final
			{
				Vector3 m_apex{}; // 頂点
				f32 m_height{ 1.0f };
				f32 m_radius{ 1.0f };

				Quaternion m_rotation{};
				Vector4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
			};

			struct DebugCapsuleParameters final
			{
				Vector3 m_center{};
				f32 m_radius{ 0.5f };
				f32 m_cylinderHeight{ 1.0f };
				Quaternion m_rotation{};
				Vector4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
			};

			struct DebugArrowParameters final
			{
				Vector3 m_start{};
				Vector3 m_direction{ Vector3::Up() }; // 正規化されているものとする
				f32 m_length{ 1.0f };
				f32 m_headSize{ 0.2f }; // 矢じりのサイズ

				Vector4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
			};

			class DebugRenderer final
			{
			public:
				DebugRenderer();
				DebugRenderer(const DebugRenderer&) = delete;
				DebugRenderer(DebugRenderer&&) noexcept = default;
				~DebugRenderer() noexcept = default;
			public:
				[[nodiscard]]
				bool Create(resource::RenderResourceManager* resourceManager);

				[[nodiscard]]
				bool Destroy();

			public:
				void Begin();
				void End(resource::RenderResourceManager* resourceManager);

			public:
				void DrawLine(const DebugLineParameters& parameters);

				// X-Z平面基準の円を描く
				void DrawCircle(const DebugCircleParameters& parameters);

				// X-Y平面基準でかつY+の半円を描く
				void DrawHalfCircle(const DebugHalfCircleParameters& parameters);
				void DrawBox(const DebugBoxParameters& parameters);
				void DrawSphere(const DebugSphereParameters& parameters);
				void DrawCone(const DebugConeParameters& parameters);
				void DrawCapsule(const DebugCapsuleParameters& parameters);
				void DrawArrow(const DebugArrowParameters& parameters);
			public:
				[[nodiscard]]
				BufferResourceHandle GetVertexBufferHandle() const noexcept { return m_vbHandle; }

			private:
				std::vector<DebugVertex> m_vertices;
				u32 m_vertexIndex;

				BufferResourceHandle m_vbHandle;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_RENDERER_HPP_