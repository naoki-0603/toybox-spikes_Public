// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_IMPORTER_MESH_FORMAT_GLTF_MESH_IMPORTER_HPP_
#define SPIKES_KIT_COMMON_ASSET_IMPORTER_MESH_FORMAT_GLTF_MESH_IMPORTER_HPP_

#include <variant>

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			struct GLTFMeshImporterConfig final
			{
			public:
				fs::path m_path{};

				bool m_convertToLeftHanded{};
			};

			struct GLTFMeshImporterResources final
			{
				struct Scene final
				{
					std::string m_name{};
				};

				struct Node final
				{
					std::string m_name{};
					Vector3 m_translation{};
					Vector3 m_scale{};
					Quaternion m_rotation{};

					Matrix m_local{};
					Matrix m_world{};
					bool m_hasMatrix{};

					i32 m_parent{ -1 };

					i32 m_mesh{ -1 };
				};

				struct Mesh final
				{
					struct SubMesh final
					{
						i32 m_material{ -1 };
						i32 m_mode{ -1 };

						std::vector<Vector3> m_positions{};
						std::vector<Vector3> m_normals{};
						std::vector<Vector2> m_texcoords0{};
						std::vector<Vector2> m_texcoords1{};
						
						std::variant<
							std::vector<u8>, std::vector<u16>, std::vector<u32>
						> m_indices{};
					};
				public:
					std::string m_name{};

					Matrix m_worldTransform{};

					std::vector<SubMesh> m_subMeshes{};
				};

				struct Texture final
				{
					std::string m_name{};

					i32 m_sampler{ -1 };
					i32 m_imageSource{ -1 };
				};

				struct Image final
				{
					std::string m_name{};
					std::string m_uri{};
				};

				struct Sampler final
				{
					std::string m_name{};

					i32 m_minFilter{};
					i32 m_magFilter{};
					i32 m_wrapS{};
					i32 m_wrapT{};
				};

				struct Material final
				{
					std::string m_name{};

					std::string m_alphaMode{ "OPAQUE" };
					f32 m_alphaCutoff{ 0.5f };
					bool m_doubleSided{};

					Vector4 m_baseColorFactor{ 1, 1, 1, 1 };
					f32 m_metallicFactor{ 1.0f };
					f32 m_roughnessFactor{ 1.0f };

					i32 m_baseColorTextureIndex{ -1 };
					i32 m_baseColorTextureTexcoordIndex{ -1 };

					i32 m_metallicRoughnessTextureIndex{ -1 };
					i32 m_metallicRoughnessTextureTexcoordIndex{ -1 };

					f32 m_normalScale{ 1.0f };
					i32 m_normalTextureIndex{ -1 };
					i32 m_normalTextureTexcoordIndex{ -1 };

					f32 m_occlusionStrength{ 1.0f };
					i32 m_occlusionTextureIndex{ -1 };
					i32 m_occlusionTextureTexcoordIndex{ -1 };

					Vector3 m_emissiveFactor{};
					i32 m_emissiveTextureIndex{ -1 };
					i32 m_emissiveTextureTexcoordIndex{ -1 };
				};

			public:
				Scene m_scene{};

				std::vector<Node> m_nodes{};
				std::vector<Mesh> m_originalMeshes{};
				std::vector<Texture> m_textures{};
				std::vector<Image> m_images{};
				std::vector<Sampler> m_samplers{};
				std::vector<Material> m_materials{};
				std::vector<Mesh> m_meshes{};
			};

			struct GLTFMeshImporterResult final
			{
			public:
				GLTFMeshImporterResources m_resources{};

				bool m_succeeded{ false };
			};

			class GLTFMeshImporter final
			{
			public:
				GLTFMeshImporter();
				GLTFMeshImporter(const GLTFMeshImporter&) = delete;
				GLTFMeshImporter(GLTFMeshImporter&&) noexcept = default;
				~GLTFMeshImporter() noexcept = default;

			public:
				[[nodiscard]]
				GLTFMeshImporterResult Import(const GLTFMeshImporterConfig& config);
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_IMPORTER_MESH_FORMAT_GLTF_IMPORTER_HPP_