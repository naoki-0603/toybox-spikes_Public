// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Asset/Importer/MeshFormat/GLTFMeshImporter.hpp"

#define TINYGLTF3_ENABLE_FS
#define TINYGLTF3_IMPLEMENTATION
#include <tiny_gltf_v3.h>

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			namespace
			{
				template <class Type>
				std::vector<Type> ReadData(const u8* startData, u64 count, u32 byteStride)
				{
					// ByteStrideが0なら隙間なく詰まっているため型のサイズを使用する
					const u32 stride = byteStride > 0u ? byteStride : sizeof(Type);

					if (byteStride == 0u)
					{
						std::vector<Type> data = {};
						data.resize(count);

						std::memcpy(data.data(), startData, stride * count);

						return data;
					}

					std::vector<Type> data = {};
					data.reserve(count);

					const u8* currentData = startData;
					for (u64 i = 0u; i < count; ++i)
					{
						auto type = reinterpret_cast<const Type*>(currentData);

						data.push_back(*type);

						currentData += stride;
					}

					return data;
				}

				[[nodiscard]]
				bool FetchNodes(
					const tg3_model& model, 
					std::vector<GLTFMeshImporterResources::Node>& nodes,
					bool convertToLeftHanded
				)
				{
					nodes.reserve(model.nodes_count);
					for (u32 i = 0u; i < model.nodes_count; ++i)
					{
						const tg3_node& node = model.nodes[i];
						const std::string nodeName = { node.name.data, node.name.len };

						GLTFMeshImporterResources::Node element = {};
						element.m_name = nodeName;

						Matrix localTransform = {};

						const bool hasMatrix = (node.has_matrix == 1);
						if (hasMatrix)
						{
							Matrix local = Matrix(
								node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
								node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
								node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
								node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]
							);

							// 左手座標系に変換する
							if (convertToLeftHanded)
							{
								local._13 *= -1.0f;
								local._23 *= -1.0f;
								local._31 *= -1.0f;
								local._32 *= -1.0f;
								local._34 *= -1.0f;
								local._43 *= -1.0f;
							}

							localTransform = local;
							element.m_hasMatrix = true;
						}
						else
						{
							// スケール
							Vector3 scale = {};
							{
								scale.x = static_cast<f32>(node.scale[0]);
								scale.y = static_cast<f32>(node.scale[1]);
								scale.z = static_cast<f32>(node.scale[2]);

								// スケールが入っていない場合は1.0fを各要素に代入する
								if (scale.IsApproxEqual(Vector3::Zero()))
								{
									scale = Vector3(1.0f, 1.0f, 1.0f);
								}
							}

							// スケーリング行列
							const Matrix S = Matrix::Scaling(scale);

							// 回転
							Quaternion rotation = {};
							{
								rotation.x = static_cast<f32>(node.rotation[0]);
								rotation.y = static_cast<f32>(node.rotation[1]);
								rotation.z = static_cast<f32>(node.rotation[2]);
								rotation.w = static_cast<f32>(node.rotation[3]);

								if (convertToLeftHanded)
								{
									rotation.z *= -1.0f;
									rotation.w *= -1.0f;
								}
							}

							// 回転行列
							Matrix R = {};
							rotation.ToMatrix(R);

							// 位置
							Vector3 translation = {};
							{
								translation.x = static_cast<f32>(node.translation[0]);
								translation.y = static_cast<f32>(node.translation[1]);
								translation.z = static_cast<f32>(node.translation[2]);

								if (convertToLeftHanded)
								{
									// Z軸を反転させる
									translation.z *= -1.0f;
								}
							}

							// トランスレーション行列
							const Matrix T = Matrix::Translation(translation);

							element.m_translation = translation;
							element.m_scale = scale;
							element.m_rotation = rotation;

							localTransform = S * R * T;
						}
						
						element.m_mesh = node.mesh;
						element.m_local = localTransform;

						nodes.push_back(std::move(element));
					}

					return true;
				}

				[[nodiscard]]
				bool FetchNodesParentRelations(
					const tg3_model& model,
					std::vector<GLTFMeshImporterResources::Node>& nodes
				)
				{
					const u32 nodeCount = static_cast<u32>(nodes.size());
					for (u32 i = 0; i < nodeCount; ++i)
					{
						const tg3_node& node = model.nodes[i];

						for (u32 j = 0; j < node.children_count; ++j)
						{
							nodes[node.children[j]].m_parent = i;
						}
					}

					return true;
				}

				Matrix ComputeNodesWorldRecursively(
					GLTFMeshImporterResources::Node& node,
					std::vector<GLTFMeshImporterResources::Node>& nodes,
					i32 parent
				)
				{
					if (parent == -1)
					{
						return node.m_local;
					}

					node.m_world = node.m_local * ComputeNodesWorldRecursively(
						nodes[parent],
						nodes,
						nodes[parent].m_parent
					);

					return node.m_world;
				}

				// TODO(naoki): O(N^2)になっているのでルートから計算していく形に変更する
				[[nodiscard]]
				bool ComputeNodesWorld(std::vector<GLTFMeshImporterResources::Node>& nodes)
				{
					const u32 nodeCount = static_cast<u32>(nodes.size());

					for (u32 i = 0u; i < nodeCount; ++i)
					{
						nodes[i].m_world = ComputeNodesWorldRecursively(
							nodes[i],
							nodes,
							nodes[i].m_parent
						);
					}

					return true;
				}

				[[nodiscard]]
				bool FetchMeshes(
					const tg3_model& model,
					std::vector<GLTFMeshImporterResources::Mesh>& meshes,
					bool convertToLeftHanded
				)
				{
					meshes.resize(model.meshes_count);
					for (u32 i = 0u; i < model.meshes_count; ++i)
					{
						const tg3_mesh& mesh = model.meshes[i];
						const auto meshName = std::string(mesh.name.data, mesh.name.len);
						
						meshes[i].m_name = meshName;
						meshes[i].m_subMeshes.resize(mesh.primitives_count);
						
						// Vertex data.
						for (u32 j = 0u; j < mesh.primitives_count; ++j)
						{
							const tg3_primitive& primitive = mesh.primitives[j];

							GLTFMeshImporterResources::Mesh::SubMesh& subMesh = meshes[i].m_subMeshes[j];
							subMesh.m_material = primitive.material;
							subMesh.m_mode = primitive.mode;

							for (u32 ai = 0u; ai < primitive.attributes_count; ++ai)
							{
								const std::string attributeName = std::string(
									primitive.attributes[ai].key.data,
									primitive.attributes[ai].key.len
								);
								const i32 accessorIndex = primitive.attributes[ai].value;

								const tg3_accessor& accessor = model.accessors[accessorIndex];
								const tg3_buffer_view& bufferView = model.buffer_views[accessor.buffer_view];
								const tg3_buffer& buffer = model.buffers[bufferView.buffer];

								const u8* startData = buffer.data.data + accessor.byte_offset + bufferView.byte_offset;

								if (attributeName == "POSITION")
								{
									TS_ASSERT(
										accessor.component_type == TG3_COMPONENT_TYPE_FLOAT &&
										accessor.type == TG3_TYPE_VEC3,
										"未対応"
									);

									std::vector<Vector3> positions = ReadData<Vector3>(
										startData, accessor.count, bufferView.byte_stride
									);

									if (convertToLeftHanded)
									{
										for (auto& position : positions)
										{
											position.z *= -1.0f;
										}
									}

									subMesh.m_positions = std::move(positions);
								}
								else if (attributeName == "NORMAL")
								{
									TS_ASSERT(
										accessor.component_type == TG3_COMPONENT_TYPE_FLOAT &&
										accessor.type == TG3_TYPE_VEC3,
										"未対応"
									);

									std::vector<Vector3> normals = ReadData<Vector3>(
										startData, accessor.count, bufferView.byte_stride
									);

									if (convertToLeftHanded)
									{
										for (auto& normal : normals)
										{
											normal.z *= -1.0f;
										}
									}

									subMesh.m_normals = std::move(normals);
								}
								else if (attributeName == "TANGENT")
								{
									TS_DEBUG_LOG("TODO: 属性TANGENTの実装");
									// TODO(naoki): 属性TANGENTの実装
									// 左手座標系に変換する際にTANGENTのZ成分に対しても-1.0fを書ける
								}
								else if (attributeName == "TEXCOORD_0")
								{
									TS_ASSERT(
										accessor.component_type == TG3_COMPONENT_TYPE_FLOAT &&
										accessor.type == TG3_TYPE_VEC2,
										"未対応"
									);

									std::vector<Vector2> texcoords = ReadData<Vector2>(
										startData, accessor.count, bufferView.byte_stride
									);

									subMesh.m_texcoords0 = std::move(texcoords);
								}
								else if (attributeName == "TEXCOORD_1")
								{
									TS_ASSERT(
										accessor.component_type == TG3_COMPONENT_TYPE_FLOAT &&
										accessor.type == TG3_TYPE_VEC2,
										"未対応"
									);

									std::vector<Vector2> texcoords = ReadData<Vector2>(
										startData, accessor.count, bufferView.byte_stride
									);

									subMesh.m_texcoords1 = std::move(texcoords);
								}
								else
								{
									TS_WARNING_LOG("対応していない属性です。{}", attributeName);
								}
							}

							// Index data
							if (primitive.indices != -1)
							{
								const tg3_accessor& accessor = model.accessors[primitive.indices];
								const tg3_buffer_view& bufferView = model.buffer_views[accessor.buffer_view];
								const tg3_buffer& buffer = model.buffers[bufferView.buffer];

								const u8* startData = buffer.data.data +
									accessor.byte_offset +
									bufferView.byte_offset;

								std::variant<std::vector<u8>, std::vector<u16>, std::vector<u32>> indices = {};
								
								// R8_UINTの場合はGPUが対応していないのでU16にキャストする
								if (accessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_BYTE)
								{
									std::vector<u8> indicesU8 = ReadData<u8>(startData, accessor.count, bufferView.byte_stride);

									if (convertToLeftHanded)
									{
										// 2番目と3番目をスワップする
										for (u64 indicesIndex = 0u; indicesIndex < indicesU8.size(); indicesIndex += 3u)
										{
											std::swap(indicesU8[indicesIndex + 1u], indicesU8[indicesIndex + 2u]);
										}
									}

									std::vector<u16> convertedIndicesU16 = {};
									convertedIndicesU16.resize(indicesU8.size());
									for (u64 indicesIndex = 0u; indicesIndex < indicesU8.size(); ++indicesIndex)
									{
										convertedIndicesU16[indicesIndex] = static_cast<u16>(indicesU8[indicesIndex]);
									}

									indices = convertedIndicesU16;
								}
								else if (accessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_SHORT)
								{
									std::vector<u16> indicesU16 = ReadData<u16>(startData, accessor.count, bufferView.byte_stride);

									if (convertToLeftHanded)
									{
										// 2番目と3番目をスワップする
										for (u64 indicesIndex = 0u; indicesIndex < indicesU16.size(); indicesIndex += 3u)
										{
											std::swap(indicesU16[indicesIndex + 1u], indicesU16[indicesIndex + 2u]);
										}
									}

									indices = indicesU16;
								}
								else if (accessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_INT)
								{
									std::vector<u32> indicesU32 = ReadData<u32>(startData, accessor.count, bufferView.byte_stride);

									if (convertToLeftHanded)
									{
										// 2番目と3番目をスワップする
										for (u64 indicesIndex = 0u; indicesIndex < indicesU32.size(); indicesIndex += 3u)
										{
											std::swap(indicesU32[indicesIndex + 1u], indicesU32[indicesIndex + 2u]);
										}
									}

									indices = indicesU32;
								}

								subMesh.m_indices = std::move(indices);
							}
						}
					}

					return true;
				}

				[[nodiscard]]
				bool FetchImages(
					const tg3_model& model,
					std::vector<GLTFMeshImporterResources::Image>& images
				)
				{
					const u32 imageCount = model.images_count;

					images.resize(imageCount);
					for (u32 i = 0u; i < imageCount; ++i)
					{
						const tg3_image& image = model.images[i];
						
						const auto imageName = std::string(
							image.name.data,
							image.name.len
						);

						const auto imageURI = std::string(
							image.uri.data,
							image.uri.len
						);

						images[i].m_name = imageName;
						images[i].m_uri = imageURI;
					}

					return true;
				}

				[[nodiscard]]
				bool FetchSamplers(
					const tg3_model& model,
					std::vector<GLTFMeshImporterResources::Sampler>& samplers
				)
				{
					const u32 samplerCount = model.samplers_count;

					samplers.resize(samplerCount);
					for (u32 i = 0u; i < samplerCount; ++i)
					{
						const tg3_sampler& sampler = model.samplers[i];
						
						const auto samplerName = std::string(
							sampler.name.data,
							sampler.name.len
						);

						const i32 minFilter = sampler.min_filter;
						const i32 magFilter = sampler.mag_filter;
						const i32 wrapS = sampler.wrap_s;
						const i32 wrapT = sampler.wrap_t;

						samplers[i].m_name = samplerName;
						samplers[i].m_minFilter = minFilter;
						samplers[i].m_magFilter = magFilter;
						samplers[i].m_wrapS = wrapS;
						samplers[i].m_wrapT = wrapT;
					}

					return true;
				}

				[[nodiscard]]
				bool FetchTextures(
					const tg3_model& model,
					std::vector<GLTFMeshImporterResources::Texture>& textures
				)
				{
					const u32 textureCount = model.textures_count;

					textures.resize(textureCount);
					for (u32 i = 0u; i < textureCount; ++i)
					{
						const tg3_texture& texture = model.textures[i];
						
						const auto textureName = std::string(
							texture.name.data,
							texture.name.len
						);

						const i32 sampler = texture.sampler;
						const i32 imageSource = texture.source;

						textures[i].m_name = textureName;
						textures[i].m_sampler = sampler;
						textures[i].m_imageSource = imageSource;
					}

					return true;
				}

				[[nodiscard]]
				bool FetchMaterials(
					const tg3_model& model,
					std::vector<GLTFMeshImporterResources::Material>& materials
				)
				{
					const u32 materialCount = model.materials_count;

					materials.resize(materialCount);
					for (u32 i = 0u; i < materialCount; ++i)
					{
						const tg3_material& material = model.materials[i];

						const auto materialName = std::string(
							material.name.data,
							material.name.len
						);

						const auto alphaMode = std::string(
							material.alpha_mode.data,
							material.alpha_mode.len
						);

						const auto alphaCutoff = static_cast<f32>(
							material.alpha_cutoff
						);

						const bool doubleSided = (material.double_sided == 1);

						// PBR
						const auto baseColorFactor = Vector4(
							static_cast<f32>(material.pbr_metallic_roughness.base_color_factor[0]),
							static_cast<f32>(material.pbr_metallic_roughness.base_color_factor[1]),
							static_cast<f32>(material.pbr_metallic_roughness.base_color_factor[2]),
							static_cast<f32>(material.pbr_metallic_roughness.base_color_factor[3])
						);

						const i32 baseColorTextureIndex = material.pbr_metallic_roughness.base_color_texture.index;
						const i32 baseColorTextureTexcoordIndex = material.pbr_metallic_roughness.base_color_texture.tex_coord;

						const auto metallicFactor = static_cast<f32>(material.pbr_metallic_roughness.metallic_factor);
						const auto roughnessFactor = static_cast<f32>(material.pbr_metallic_roughness.roughness_factor);

						const i32 metallicRoughnessTextureIndex = material.pbr_metallic_roughness.metallic_roughness_texture.index;
						const i32 metallicRoughnessTextureTexcoordIndex= material.pbr_metallic_roughness.metallic_roughness_texture.tex_coord;

						// Normal map
						const auto normalScale = static_cast<f32>(material.normal_texture.scale);

						const i32 normalTextureIndex = material.normal_texture.index;
						const i32 normalTextureTexcoordIndex = material.normal_texture.tex_coord;

						// Occlusion
						const auto occlusionStrength = static_cast<f32>(material.occlusion_texture.strength);

						const i32 occlusionTextureIndex = material.occlusion_texture.index;
						const i32 occlusionTextureTexcoordIndex = material.occlusion_texture.tex_coord;

						// Emissive
						const auto emissiveFactor = Vector3(
							static_cast<f32>(material.emissive_factor[0]),
							static_cast<f32>(material.emissive_factor[1]),
							static_cast<f32>(material.emissive_factor[2])
						);

						const i32 emissiveTextureIndex = material.emissive_texture.index;
						const i32 emissiveTextureTexcoordIndex = material.emissive_texture.tex_coord;
	
						materials[i].m_name = materialName;
						materials[i].m_alphaMode = alphaMode;
						materials[i].m_alphaCutoff = alphaCutoff;
						materials[i].m_doubleSided = doubleSided;

						materials[i].m_baseColorFactor = baseColorFactor;
						materials[i].m_baseColorTextureIndex = baseColorTextureIndex;
						materials[i].m_baseColorTextureTexcoordIndex = baseColorTextureTexcoordIndex;

						materials[i].m_metallicFactor = metallicFactor;
						materials[i].m_roughnessFactor = roughnessFactor;
						materials[i].m_metallicRoughnessTextureIndex = metallicRoughnessTextureIndex;
						materials[i].m_metallicRoughnessTextureTexcoordIndex = metallicRoughnessTextureTexcoordIndex;

						materials[i].m_normalScale = normalScale;
						materials[i].m_normalTextureIndex = normalTextureIndex;
						materials[i].m_normalTextureTexcoordIndex = normalTextureTexcoordIndex;

						materials[i].m_occlusionStrength = occlusionStrength;
						materials[i].m_occlusionTextureIndex = occlusionTextureIndex;
						materials[i].m_occlusionTextureTexcoordIndex = occlusionTextureTexcoordIndex;

						materials[i].m_emissiveFactor = emissiveFactor;
						materials[i].m_emissiveTextureIndex = emissiveTextureIndex;
						materials[i].m_emissiveTextureTexcoordIndex = emissiveTextureTexcoordIndex;
					}

					return true;
				}

				[[nodiscard]]
				bool BuildMeshes(
					const std::vector<GLTFMeshImporterResources::Node>& nodes,
					const std::vector<GLTFMeshImporterResources::Mesh>& originalMeshes,
					std::vector<GLTFMeshImporterResources::Mesh>& meshes
				)
				{
					using Node = GLTFMeshImporterResources::Node;
					using Mesh = GLTFMeshImporterResources::Mesh;

					for (u64 nodeIndex = 0u; nodeIndex < nodes.size(); ++nodeIndex)
					{
						const Node& node = nodes[nodeIndex];
						if (node.m_mesh == -1)
						{
							continue;
						}
						const Mesh& originalMesh = originalMeshes[node.m_mesh];
						
						Mesh& mesh = meshes.emplace_back();
						mesh.m_name = originalMesh.m_name;
						mesh.m_worldTransform = node.m_world;
						mesh.m_subMeshes = originalMesh.m_subMeshes;
					}

					return true;
				}
			} // namespace

			GLTFMeshImporter::GLTFMeshImporter()
			{
			}

			GLTFMeshImporterResult GLTFMeshImporter::Import(const GLTFMeshImporterConfig& config)
			{
				// オプションのパース
				tg3_parse_options options = {};
				options.required_sections = TG3_REQUIRE_ALL;
				options.images_as_is = 1; // Dont decode image.
				options.strictness = TG3_STRICT;

				tg3_error_stack errors = {};
				tg3_model model = {};

				const std::string filename = config.m_path.string();
				const u32 filenameLength = filename.length();

				// ファイルのパース
				const tg3_error_code error = tg3_parse_file(
					&model, &errors, filename.c_str(), filenameLength, &options
				);
				if (error != TG3_OK)
				{
					TS_ASSERT(false, "");
				}

				// ルートノードの取得
				TS_ASSERT(
					model.scenes_count == 1 && model.scenes_count > 0,
					"単一シーンのみサポートしています。SceneCount: {}", model.scenes_count
				);

				const tg3_scene& scene = model.scenes[model.default_scene];
				const auto sceneName = std::string(scene.name.data, scene.name.len);

				TS_INFO_LOG("-- GLTF MeshImporter --");
				TS_INFO_LOG("SceneName: {}", sceneName.empty() ? "None" : sceneName);

				GLTFMeshImporterResources resources = {};
				resources.m_scene.m_name = sceneName;

				GLTFMeshImporterResult result = {};
				
				if (!FetchNodes(model, resources.m_nodes, config.m_convertToLeftHanded))
				{
					return result;
				}

				if (!FetchNodesParentRelations(model, resources.m_nodes))
				{
					return result;
				}

				if (!ComputeNodesWorld(resources.m_nodes))
				{
					return result;
				}

				if (!FetchMeshes(model, resources.m_originalMeshes, config.m_convertToLeftHanded))
				{
					return result;
				}

				if (!FetchImages(model, resources.m_images))
				{
					return result;
				}

				if (!FetchSamplers(model, resources.m_samplers))
				{
					return result;
				}

				if (!FetchTextures(model, resources.m_textures))
				{
					return result;
				}

				if (!FetchMaterials(model, resources.m_materials))
				{
					return result;
				}

				if (!BuildMeshes(resources.m_nodes, resources.m_originalMeshes, resources.m_meshes))
				{
					return result;
				}

				tg3_model_free(&model);
				tg3_error_stack_free(&errors);

				result.m_resources = std::move(resources);
				result.m_succeeded = true;

				return result;
			}
		} // namespace asset
	} // namespace kit
} // namespace ts