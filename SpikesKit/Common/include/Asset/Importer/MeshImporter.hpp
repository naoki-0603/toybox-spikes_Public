// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_IMPORTER_MESH_IMPORTER_HPP_
#define SPIKES_KIT_COMMON_ASSET_IMPORTER_MESH_IMPORTER_HPP_

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			struct GLTFMeshImporterResult;

			struct MeshImportConfig final
			{
				fs::path m_path{};

				bool m_convertToLeftHanded{};
			};

			struct MeshImporterResult final
			{
				enum class AlphaMode : u32
				{
					Opaque,
					Mask,
					Blend,

					Max
				};

				struct Texture final
				{
					std::string m_name{};
					std::string m_uri{};
					u64 m_uriHash{};
				};

				struct Sampler final
				{
					enum class MagFilter
					{
						Nearest,
						Linear,

						Max
					};

					enum class MinFilter
					{
						Nearest,
						Linear,
						NearestMipMapNearest,
						LinearMipMapNearest,
						NearestMipMapLinear,
						LinearMipMapLinear,

						Max
					};

					enum class Wrap
					{
						ClampToEdge,
						MirroredReapeat,
						Repeat,

						Max
					};
				public:
					std::string m_name{};
					MagFilter m_magFilter{};
					MinFilter m_minFilter{};
					Wrap m_wrapU{};
					Wrap m_wrapV{};
				};

				struct Material final
				{
					std::string m_name{};
					AlphaMode m_alphaMode{ AlphaMode::Opaque };
					f32 m_alphaCutoff{};
					bool m_doubleSided{};

					Vector4 m_baseColorFactor{};
					f32 m_metallicFactor{};
					f32 m_roughnessFactor{};

					f32 m_normalScale{};
					f32 m_occlusionStrength{};
					Vector3 m_emissiveFactor{};

					Texture m_baseColorTexture{};
					Texture m_metallicRoughnessTexture{};
					Texture m_normalTexture{};
					Texture m_occlusionTexture{};
					Texture m_emissiveTexture{};

					Sampler m_baseColorTextureSampler{};
					Sampler m_metallicRoughnessTextureSampler{};
					Sampler m_normalTextureSampler{};
					Sampler m_occlusionTextureSampler{};
					Sampler m_emissiveTextureSampler{};

					i32 m_baseColorTextureTexcoord{ -1 };
					i32 m_metallicRoughnessTextureTexcoord{ -1 };
					i32 m_normalTextureTexcoord{ -1 };
					i32 m_occlusionTextureTexcoord{ -1 };
					i32 m_emissiveTextureTexcoord{ -1 };
				};

				struct Mesh final
				{
					enum class Mode
					{
						Triangles
					};

					struct SubMesh final
					{
						i32 m_material{ -1 };
						Mode m_mode{};

						std::vector<Vector3> m_positions{};
						std::vector<Vector3> m_normals{};
						std::vector<Vector2> m_texcoords0{};
						std::vector<Vector2> m_texcoords1{};

						std::vector<u8> m_indicesU8{};
						std::vector<u16> m_indicesU16{};
						std::vector<u32> m_indicesU32{};
					};
				public:
					std::string m_name{};
					std::vector<SubMesh> m_subMeshes{};

					Matrix m_world{};
				};
			public:
				std::vector<Material> m_materials{};
				std::vector<Mesh> m_meshes{};

				bool m_succeeded{ false };
			};

			class MeshImporter final
			{
			public:
				MeshImporter() = default;
				MeshImporter(const MeshImporter&) = delete;
				MeshImporter(MeshImporter&&) noexcept = default;
				~MeshImporter() noexcept = default;

			public:
				[[nodiscard]]
				MeshImporterResult Import(const MeshImportConfig& config);

			private:
				[[nodiscard]]
				MeshImporterResult TranslateFromGLTF(GLTFMeshImporterResult& result);
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_IMPORTER_MESH_IMPORTER_HPP_