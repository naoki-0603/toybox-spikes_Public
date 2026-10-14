// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Asset/AssetManager.hpp"

#include "Asset/Importer/ShaderImporter.hpp"
#include "Asset/Importer/TextureImporter.hpp"
#include "Asset/Importer/MeshImporter.hpp"

#include "Asset/Builder/MaterialAssetBuilder.hpp"
#include "Asset/Builder/StaticMeshAssetBuilder.hpp"

#include "Graphics/StaticMeshVertex.hpp"

#include "Graphics/RHI/RHI_States.hpp"
#include "Graphics/RHI/RHI_Buffer.hpp"

#include "Graphics/RenderResourceManager.hpp"
#include "Graphics/MaterialConstants.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			AssetManager::AssetManager() :
				m_database(),
				m_renderResourceManager(),
				m_shaderImporter(),
				m_textureImporter(),
				m_meshImporter(),
				m_loadShaderMutex(),
				m_loadTextureMutex(),
				m_loadMaterialMutex(),
				m_loadStaticMeshMutex()
			{
			}
			
			bool AssetManager::Create(const AssetManagerInitDesc& desc)
			{
				if (!desc.m_renderResourceManager)
				{
					TS_FATAL_LOG("RenderResourceManagerがnullptrです。");

					return false;
				}
				m_renderResourceManager = desc.m_renderResourceManager;

				if (!desc.m_shaderCompiler)
				{
					TS_FATAL_LOG("ShaderCompilerがnullptrです。");

					return false;
				}
				m_shaderImporter = new ShaderImporter(desc.m_shaderCompiler);
				m_textureImporter = new TextureImporter();
				m_meshImporter = new MeshImporter();

				return true;
			}

			bool AssetManager::Destroy()
			{
				TS_SAFE_RELEASE(m_shaderImporter);
				TS_SAFE_RELEASE(m_textureImporter);
				TS_SAFE_RELEASE(m_meshImporter);

				return true;
			}

			const ShaderAsset* AssetManager::FindShader(ShaderAssetHandle handle) const
			{
				std::shared_lock lock(m_loadShaderMutex);

				return m_database.FindShader(handle);
			}

			const TextureAsset* AssetManager::FindTexture(TextureAssetHandle handle) const
			{
				std::shared_lock lock(m_loadTextureMutex);

				return m_database.FindTexture(handle);
			}

			const MaterialAsset* AssetManager::FindMaterial(MaterialAssetHandle handle) const
			{
				std::shared_lock lock(m_loadMaterialMutex);

				return m_database.FindMaterial(handle);
			}

			const StaticMeshAsset* AssetManager::FindStaticMesh(StaticMeshAssetHandle handle) const
			{
				std::shared_lock lock(m_loadStaticMeshMutex);

				return m_database.FindStaticMeshAsset(handle);
			}

			ShaderAssetHandle AssetManager::LoadShader(
				const LoadShaderConfig& config
			)
			{
				ShaderImportConfig importConfig = {};
				importConfig.m_compilerConfig.m_entryPoint = config.m_entryPoint;
				importConfig.m_compilerConfig.m_options = config.m_options;
				importConfig.m_compilerConfig.m_path = config.m_path;
				importConfig.m_compilerConfig.m_target = config.m_target;
				importConfig.m_compilerConfig.m_macros = config.m_macros;

				const ShaderImpoterResult result = m_shaderImporter->Import(importConfig);
				
				const ShaderResourceHandle resourceHandle = m_renderResourceManager->CreateShader(
					config.m_shaderType, result.m_binary
				);

				ShaderAsset shaderAsset = {};
				shaderAsset.m_metadata.m_timestamp = 0;
				shaderAsset.m_metadata.m_state = AssetState::Completed;
				shaderAsset.m_resourceHandle = resourceHandle;

				ShaderAssetHandle handle = {};
				{
					std::unique_lock lock(m_loadShaderMutex);
					handle = m_database.AddShader(shaderAsset);
				}

				return handle;
			}

			TextureAssetHandle AssetManager::LoadTexture(
				const LoadTextureConfig& config
			)
			{
				ZoneScoped;

				TextureImportConfig importConfig = {};
				importConfig.m_path = config.m_path;

				const TextureImpoterResult result = m_textureImporter->Import(
					importConfig
				);

				const graphics::RHI_Format format = (result.m_channel == 4) 
					? graphics::RHI_Format::R8G8B8A8_UNorm 
					: graphics::RHI_Format::Unknown;

				const TextureResourceHandle handle = m_renderResourceManager->CreateTexture(
					result.m_pixels,
					result.m_size,
					result.m_mipLevels,
					format
				);

				TextureAsset textureAsset = {};
				textureAsset.m_metadata.m_timestamp = 0;
				textureAsset.m_metadata.m_state = AssetState::Completed;
				textureAsset.m_resourceHandle = handle;

				const std::string fileName = importConfig.m_path.filename().string();
				std::hash<std::string> hasher;
				
				const u64 textureHash = hasher(fileName);

				bool succeededAddTextureHash = true;
				TextureAssetHandle textureAssetHandle = {};
				{
					std::unique_lock lock(m_loadTextureMutex);
					textureAssetHandle = m_database.AddTexture(textureAsset);
					succeededAddTextureHash = m_database.AddTextureHash(
						textureHash, textureAssetHandle
					);
					
					if (!succeededAddTextureHash)
					{
						TS_FATAL_LOG(
							"現状ファイル名だけをハッシュ化してるため同名のファイルがあると衝突する。現状は実装優先のため無視している。今後対応予定"
						);
					}

					TS_INFO_LOG("Texture Successfully Loaded: {}", fileName);
				}

				return textureAssetHandle;
			}

			std::vector<StaticMeshAssetHandle> AssetManager::LoadStaticMesh(
				const LoadMeshConfig& config
			)
			{
				// MeshImport
				MeshImportConfig importConfig = {};
				importConfig.m_path = config.m_path;
				importConfig.m_convertToLeftHanded = config.m_convertToLeftHanded;

				const MeshImporterResult result = m_meshImporter->Import(
					importConfig
				);
				if (!result.m_succeeded)
				{
					return {};
				}

				// Material
				std::vector<MaterialAssetHandle> materialAssetHandles = {};
				{
					materialAssetHandles.resize(result.m_materials.size());

					for (u64 i = 0u; i < result.m_materials.size(); ++i)
					{
						const MeshImporterResult::Material& material = result.m_materials[i];

						graphics::PBRMaterialConstants pbrMaterialConstants = {};

						MaterialAssetBuilderDesc desc = {};
						desc.m_name = material.m_name;

						// Parameters
						{
							desc.m_parameters.m_alphaCutoff = material.m_alphaCutoff;
							
							switch (material.m_alphaMode)
							{
							case MeshImporterResult::AlphaMode::Opaque:
								desc.m_alphaMode = MaterialAlphaMode::Opaque;
								break;
							case MeshImporterResult::AlphaMode::Mask:
								desc.m_alphaMode = MaterialAlphaMode::Mask;
								break;
							case MeshImporterResult::AlphaMode::Blend:
								desc.m_alphaMode = MaterialAlphaMode::Blend;
								break;
							}
							desc.m_parameters.m_doubleSided = material.m_doubleSided;

							desc.m_parameters.m_baseColorFactor = material.m_baseColorFactor;
							desc.m_parameters.m_metallicFactor = material.m_metallicFactor;
							desc.m_parameters.m_roughnessFactor = material.m_roughnessFactor;
							desc.m_parameters.m_normalScale = material.m_normalScale;
							desc.m_parameters.m_occlusionStrength = material.m_occlusionStrength;
							desc.m_parameters.m_emissiveFactor = material.m_emissiveFactor;

							// 定数に保存する
							pbrMaterialConstants.m_alphaCutoff = desc.m_parameters.m_alphaCutoff;
							pbrMaterialConstants.m_alphaMode = static_cast<i32>(desc.m_alphaMode);
							pbrMaterialConstants.m_baseColorFactor = desc.m_parameters.m_baseColorFactor;
							pbrMaterialConstants.m_roughnessFactor = desc.m_parameters.m_roughnessFactor;
							pbrMaterialConstants.m_normalScale = desc.m_parameters.m_normalScale;
							pbrMaterialConstants.m_occlusionStrength = desc.m_parameters.m_occlusionStrength;
							pbrMaterialConstants.m_emissiveFactor = desc.m_parameters.m_emissiveFactor;
						}

						// Shader
						{
							desc.m_shaderAssetHandle = config.m_meshPBRPixelShaderHandle;
						}

						// Textures
						{
							std::shared_lock lock(m_loadTextureMutex);

							// Base color texture
							if (material.m_baseColorTexture.m_uriHash != 0)
							{
								const TextureAssetHandle handle = m_database.FindTextureHandleFromHash(
									material.m_baseColorTexture.m_uriHash
								);

								const TextureAsset* texture = m_database.FindTexture(handle);
								desc.m_baseColorHandle = texture->m_resourceHandle;

								// 定数に保存する
								pbrMaterialConstants.m_baseColorTextureIndex = 0;
								pbrMaterialConstants.m_baseColorTextureTexcoord = material.m_baseColorTextureTexcoord;
							}

							// metallic roughness
							if (material.m_metallicRoughnessTexture.m_uriHash != 0)
							{
								const TextureAssetHandle handle = m_database.FindTextureHandleFromHash(
									material.m_metallicRoughnessTexture.m_uriHash
								);

								const TextureAsset* texture = m_database.FindTexture(handle);
								desc.m_metallicRoughnessHandle = texture->m_resourceHandle;

								// 定数に保存する
								pbrMaterialConstants.m_metallicRoughnessTextureIndex = 1;
								pbrMaterialConstants.m_metallicRoughnessTextureTexcoord = material.m_metallicRoughnessTextureTexcoord;
							}

							// normal
							if (material.m_normalTexture.m_uriHash != 0)
							{
								const TextureAssetHandle handle = m_database.FindTextureHandleFromHash(
									material.m_normalTexture.m_uriHash
								);

								const TextureAsset* texture = m_database.FindTexture(handle);
								desc.m_normalHandle = texture->m_resourceHandle;

								// 定数に保存する
								pbrMaterialConstants.m_normalTextureIndex = 2;
								pbrMaterialConstants.m_normalTextureTexcoord = material.m_normalTextureTexcoord;
							}

							// occlusion
							if (material.m_occlusionTexture.m_uriHash != 0)
							{
								const TextureAssetHandle handle = m_database.FindTextureHandleFromHash(
									material.m_occlusionTexture.m_uriHash
								);

								const TextureAsset* texture = m_database.FindTexture(handle);
								desc.m_occlusionHandle = texture->m_resourceHandle;

								// 定数に保存する
								pbrMaterialConstants.m_occlusionTextureIndex = 3;
								pbrMaterialConstants.m_occlusionTextureTexcoord = material.m_occlusionTextureTexcoord;
							}

							// emissive
							if (material.m_emissiveTexture.m_uriHash != 0)
							{
								const TextureAssetHandle handle = m_database.FindTextureHandleFromHash(
									material.m_emissiveTexture.m_uriHash
								);

								const TextureAsset* texture = m_database.FindTexture(handle);
								desc.m_emissiveHandle = texture->m_resourceHandle;

								// 定数に保存する
								pbrMaterialConstants.m_emissiveTextureIndex = 4;
								pbrMaterialConstants.m_emissiveTextureTexcoord = material.m_emissiveTextureTexcoord;
							}
						}

						// Samplers
						{
							using namespace graphics;

							using Sampler = MeshImporterResult::Sampler;
							using MagFilter = MeshImporterResult::Sampler::MagFilter;
							using MinFilter = MeshImporterResult::Sampler::MinFilter;
							using Wrap = MeshImporterResult::Sampler::Wrap;

							auto CreateSampler = [](const Sampler& sampler) -> RHI_SamplerState
							{
									RHI_SamplerState state = {};

									// TODO(naoki): m_minFilterも考慮する形に修正する
									switch (sampler.m_magFilter)
									{
									case MagFilter::Linear:
										state.m_filter = RHI_SamplerFilter::Linear;
										break;
									case MagFilter::Nearest:
										state.m_filter = RHI_SamplerFilter::Point;
										break;
									}

									switch (sampler.m_wrapU)
									{
									case Wrap::ClampToEdge:
										state.m_addressU = RHI_SamplerTextureAddress::Clamp;
										break;
									case Wrap::MirroredReapeat:
										state.m_addressU = RHI_SamplerTextureAddress::Mirror;
										break;
									case Wrap::Repeat:
										state.m_addressU = RHI_SamplerTextureAddress::Wrap;
										break;
									}

									switch (sampler.m_wrapV)
									{
									case Wrap::ClampToEdge:
										state.m_addressV = RHI_SamplerTextureAddress::Clamp;
										state.m_addressW = state.m_addressV;
										break;
									case Wrap::MirroredReapeat:
										state.m_addressV = RHI_SamplerTextureAddress::Mirror;
										state.m_addressW = state.m_addressV;
										break;
									case Wrap::Repeat:
										state.m_addressV = RHI_SamplerTextureAddress::Wrap;
										state.m_addressW = state.m_addressV;
										break;
									}

									return state;
							};

							// Base color
							if (material.m_baseColorTexture.m_uriHash != 0)
							{
								const RHI_SamplerState sampler = CreateSampler(material.m_baseColorTextureSampler);
								desc.m_baseColorSamplerHandle = m_renderResourceManager->CreateOrGetSampler(sampler);

								// 定数に保存する
								pbrMaterialConstants.m_baseColorSamplerIndex = 3;
							}

							// Metallic roughness
							if (material.m_metallicRoughnessTexture.m_uriHash != 0)
							{
								const RHI_SamplerState sampler = CreateSampler(material.m_metallicRoughnessTextureSampler);
								desc.m_metallicRoughnessSamplerHandle = m_renderResourceManager->CreateOrGetSampler(sampler);

								// 定数に保存する
								pbrMaterialConstants.m_metallicRoughnessSamplerIndex = 4;
							}

							// Normal
							if (material.m_normalTexture.m_uriHash != 0)
							{
								const RHI_SamplerState sampler = CreateSampler(material.m_normalTextureSampler);
								desc.m_normalSamplerHandle = m_renderResourceManager->CreateOrGetSampler(sampler);

								// 定数に保存する
								pbrMaterialConstants.m_normalTextureSamplerIndex = 5;
							}

							// Occlusion
							if (material.m_occlusionTexture.m_uriHash != 0)
							{
								const RHI_SamplerState sampler = CreateSampler(material.m_occlusionTextureSampler);
								desc.m_occlusionSamplerHandle = m_renderResourceManager->CreateOrGetSampler(sampler);

								// 定数に保存する
								pbrMaterialConstants.m_occlusionTextureSamplerIndex = 6;
							}

							// Emissive
							if (material.m_emissiveTexture.m_uriHash != 0)
							{
								const RHI_SamplerState sampler = CreateSampler(material.m_emissiveTextureSampler);
								desc.m_emissiveSamplerHandle = m_renderResourceManager->CreateOrGetSampler(sampler);

								// 定数に保存する
								pbrMaterialConstants.m_emissiveTextureSamplerIndex = 7;
							}
						}

						// 定数バッファの生成
						{
							using namespace graphics;

							// 256バイト境界に配置する
							const u64 strideInBytes = memory::AlignUp(sizeof(PBRMaterialConstants), 256u);

							RHI_BufferDesc bufferDesc = {};
							bufferDesc.m_type = RHI_BufferType::ConstantBuffer;
							bufferDesc.m_usage = RHI_BufferUsage::Dynamic;
							bufferDesc.m_strideInBytes = strideInBytes;
							bufferDesc.m_count = 1u;
							bufferDesc.m_data = &pbrMaterialConstants;

							desc.m_constantBufferHandle = m_renderResourceManager->CreateBuffer(
								bufferDesc
							);
						}

						// マテリアルアセットの作成
						MaterialAsset materialAsset = {};
						{
							MaterialAssetBuilder materialAssetBuilder = {};
							if (!materialAssetBuilder.Build(desc, materialAsset))
							{
								TS_ASSERT(false, "MaterialAssetのビルドに失敗しました");
							}
						}

						// マテリアルアセットの登録
						MaterialAssetHandle& materialAssetHandle = materialAssetHandles[i];
						{
							std::unique_lock<std::shared_mutex> lock(m_loadMaterialMutex);
							materialAssetHandle = m_database.AddMaterial(materialAsset);
						}
					}
				}

				std::unordered_map<std::string, u32> meshMaterialTable = {};

				// Mesh
				std::vector<StaticMeshAssetBuilderDesc> builderDescs = {};
				{
					builderDescs.resize(result.m_meshes.size());
					for (u64 meshIndex = 0u; meshIndex < result.m_meshes.size(); ++meshIndex)
					{
						const MeshImporterResult::Mesh& mesh = result.m_meshes[meshIndex];
					
						StaticMeshAssetBuilderDesc& staticMeshBuilderDesc = builderDescs[meshIndex];
						staticMeshBuilderDesc.m_name = mesh.m_name;
						staticMeshBuilderDesc.m_subMeshes.resize(mesh.m_subMeshes.size());

						for (u64 subMeshIndex = 0u; subMeshIndex < mesh.m_subMeshes.size(); ++subMeshIndex)
						{
							using namespace graphics;

							const MeshImporterResult::Mesh::SubMesh& subMesh = mesh.m_subMeshes[subMeshIndex];
							StaticMeshAsset::SubMesh& subMeshAsset = staticMeshBuilderDesc.m_subMeshes[subMeshIndex];

							std::string convertedName = mesh.m_name;
							convertedName += "_" + std::to_string(subMeshIndex);

							TS_ASSERT(convertedName.length() < sizeof(subMeshAsset.m_name), "サブメッシュ名が長すぎます。");
							const u64 copyLength = std::min<u64>(convertedName.length(), sizeof(subMeshAsset.m_name) - 1u);
							std::memcpy(subMeshAsset.m_name, convertedName.data(), copyLength);
							subMeshAsset.m_name[copyLength] = '\0';

							std::vector<StaticMeshVertex> vertices = {};
							vertices.resize(subMesh.m_positions.size());
							for (u64 vertexIndex = 0u; vertexIndex < vertices.size(); ++vertexIndex)
							{
								vertices[vertexIndex].m_position = Vector4(subMesh.m_positions[vertexIndex], 1.0f);
								vertices[vertexIndex].m_normal = Vector4(subMesh.m_normals[vertexIndex], 0.0f);
								// vertices[i].m_tangent
								
								if (!subMesh.m_texcoords0.empty())
								{
									vertices[vertexIndex].m_texcoords[0] = subMesh.m_texcoords0[vertexIndex];
								}

								if (!subMesh.m_texcoords1.empty())
								{
									vertices[vertexIndex].m_texcoords[1] = subMesh.m_texcoords1[vertexIndex];
								}
							}

							// 頂点バッファ
							{
								RHI_BufferDesc bufferDesc = {};
								bufferDesc.m_type = RHI_BufferType::VertexBuffer;
								bufferDesc.m_usage = RHI_BufferUsage::Immutable;
								bufferDesc.m_strideInBytes = sizeof(StaticMeshVertex);
								bufferDesc.m_count = vertices.size();
								bufferDesc.m_data = vertices.data();

								BufferResourceHandle vertexBufferHandle = m_renderResourceManager->CreateBuffer(bufferDesc);
								subMeshAsset.m_vertexBufferHandle = vertexBufferHandle;
							}

							// インデックスバッファ
							{
								u64 indexCount = 0u;
								u64 indexStride = 0u;
								const void* indexData = nullptr;

								const bool isU8 = !(subMesh.m_indicesU8.empty());
								const bool isU16 = !(subMesh.m_indicesU16.empty());
								const bool isU32 = !(subMesh.m_indicesU32.empty());

								if (isU8)
								{
									indexCount = subMesh.m_indicesU8.size();
									indexData = subMesh.m_indicesU8.data();
									indexStride = sizeof(u8);
								}
								else if (isU16)
								{
									indexCount = subMesh.m_indicesU16.size();
									indexData = subMesh.m_indicesU16.data();
									indexStride = sizeof(u16);
								}
								else if (isU32)
								{
									indexCount = subMesh.m_indicesU32.size();
									indexData = subMesh.m_indicesU32.data();
									indexStride = sizeof(u32);
								}

								RHI_BufferDesc bufferDesc = {};
								bufferDesc.m_type = RHI_BufferType::IndexBuffer;
								bufferDesc.m_usage = RHI_BufferUsage::Immutable;
								bufferDesc.m_strideInBytes = indexStride;
								bufferDesc.m_count = indexCount;
								bufferDesc.m_data = indexData;

								BufferResourceHandle indexBufferHandle = m_renderResourceManager->CreateBuffer(bufferDesc);
								subMeshAsset.m_indexBufferHandle = indexBufferHandle;
								subMeshAsset.m_indexCount = indexCount;
							}
							subMeshAsset.m_worldTransform = mesh.m_world;
							
							meshMaterialTable[subMeshAsset.m_name] = subMesh.m_material;
						}
					}
				}

				// マテリアルハンドルを各サブメッシュにアタッチする
				for (u64 meshIndex = 0u; meshIndex < builderDescs.size(); ++meshIndex)
				{
					StaticMeshAssetBuilderDesc& desc = builderDescs[meshIndex];
					for (u64 subMeshIndex = 0u; subMeshIndex < desc.m_subMeshes.size(); ++subMeshIndex)
					{
						const u32 materialIndex = meshMaterialTable.at(
							desc.m_subMeshes[subMeshIndex].m_name
						);

						desc.m_subMeshes[subMeshIndex].m_materialAssetHandle = materialAssetHandles[materialIndex];
					}
				}

				// メッシュアセットの作成
				std::vector<StaticMeshAssetHandle> staticMeshHandles = {};
				staticMeshHandles.resize(builderDescs.size());
				for (u64 builderIndex = 0u; builderIndex < builderDescs.size(); ++builderIndex)
				{
					StaticMeshAsset staticMeshAsset = {};

					StaticMeshAssetBuilder builder = {};
					if (!builder.Build(builderDescs[builderIndex], staticMeshAsset))
					{
						TS_ASSERT(false, "スタティックメッシュの構築に失敗しました");
					}

					{
						std::unique_lock lock(m_loadStaticMeshMutex);
						staticMeshHandles[builderIndex] = m_database.AddStaticMesh(staticMeshAsset);
					}
				}

				{
					std::shared_lock lock(m_loadStaticMeshMutex);
					TS_INFO_LOG("Mesh Successfully Loaded: {}", config.m_path.filename().string());
				}

				return staticMeshHandles;
			}
		} // namespace asset
	} // namespace kit
} // namespace ts