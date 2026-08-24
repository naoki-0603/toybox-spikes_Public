// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Asset/Importer/MeshImporter.hpp"

#include "Asset/Importer/MeshFormat/GLTFMeshImporter.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			MeshImporterResult MeshImporter::Import(const MeshImportConfig& config)
			{
				GLTFMeshImporter gltfMeshImporter = {};

				GLTFMeshImporterConfig gltfMeshImporterConfig = {};
				gltfMeshImporterConfig.m_path = config.m_path;
				gltfMeshImporterConfig.m_convertToLeftHanded = config.m_convertToLeftHanded;
				
				GLTFMeshImporterResult result = gltfMeshImporter.Import(gltfMeshImporterConfig);

				return TranslateFromGLTF(result);
			}

			MeshImporterResult MeshImporter::TranslateFromGLTF(GLTFMeshImporterResult& result)
			{
				MeshImporterResult commonResult = {};
				commonResult.m_succeeded = result.m_succeeded;
				
				if (!commonResult.m_succeeded)
				{
					return commonResult;
				}

				const GLTFMeshImporterResources& resources = result.m_resources;

				// Samplers
				std::vector<MeshImporterResult::Sampler> samplers = {};
				{
					samplers.resize(resources.m_samplers.size());
					for (u64 i = 0u; i < samplers.size(); ++i)
					{
						const GLTFMeshImporterResources::Sampler& gltfSampler = resources.m_samplers[i];
						MeshImporterResult::Sampler& sampler = samplers[i];

						sampler.m_name = gltfSampler.m_name;
						
						// MinFilter
						{
							// NEAREST
							if (gltfSampler.m_minFilter == 9728)
							{
								sampler.m_minFilter = MeshImporterResult::Sampler::MinFilter::Nearest;
							}
							// LINEAR
							else if (gltfSampler.m_magFilter == 9729)
							{
								sampler.m_minFilter = MeshImporterResult::Sampler::MinFilter::Linear;
							}
							// NEAREST_MIPMAP_NEAREST
							else if (gltfSampler.m_magFilter == 9984)
							{
								sampler.m_minFilter = MeshImporterResult::Sampler::MinFilter::NearestMipMapNearest;
							}
							// LINEAR_MIPMAP_NEAREST
							else if (gltfSampler.m_magFilter == 9985)
							{
								sampler.m_minFilter = MeshImporterResult::Sampler::MinFilter::LinearMipMapNearest;
							}
							// NEAREST_MIPMAP_LINEAR
							else if (gltfSampler.m_magFilter == 9986)
							{
								sampler.m_minFilter = MeshImporterResult::Sampler::MinFilter::NearestMipMapLinear;
							}
							// LINEAR_MIPMAP_LINEAR
							else if (gltfSampler.m_magFilter == 9987)
							{
								sampler.m_minFilter = MeshImporterResult::Sampler::MinFilter::LinearMipMapLinear;
							}
						}

						// MagFilter
						{
							// NEAREST
							if (gltfSampler.m_magFilter == 9728)
							{
								sampler.m_magFilter = MeshImporterResult::Sampler::MagFilter::Nearest;
							}
							// LINEAR
							else if (gltfSampler.m_magFilter == 9729)
							{
								sampler.m_magFilter = MeshImporterResult::Sampler::MagFilter::Linear;
							}
						}

						// WrapS(U)
						{
							// CLAMP_TO_EDGE
							if (gltfSampler.m_wrapS == 33071)
							{
								sampler.m_wrapU = MeshImporterResult::Sampler::Wrap::ClampToEdge;
							}
							// MIRRORED_REPEAT
							else if (gltfSampler.m_wrapS == 33648)
							{
								sampler.m_wrapU = MeshImporterResult::Sampler::Wrap::MirroredReapeat;
							}
							// REPEAT
							else if (gltfSampler.m_wrapS == 10497)
							{
								sampler.m_wrapU = MeshImporterResult::Sampler::Wrap::Repeat;
							}
						}

						// WrapT(V)
						{
							// CLAMP_TO_EDGE
							if (gltfSampler.m_wrapT == 33071)
							{
								sampler.m_wrapV = MeshImporterResult::Sampler::Wrap::ClampToEdge;
							}
							// MIRRORED_REPEAT
							else if (gltfSampler.m_wrapT == 33648)
							{
								sampler.m_wrapV = MeshImporterResult::Sampler::Wrap::MirroredReapeat;
							}
							// REPEAT
							else if (gltfSampler.m_wrapT == 10497)
							{
								sampler.m_wrapV = MeshImporterResult::Sampler::Wrap::Repeat;
							}
						}
					}
				}

				// Materials
				{
					commonResult.m_materials.resize(resources.m_materials.size());
					for (u64 i = 0u; i < resources.m_materials.size(); ++i)
					{
						const GLTFMeshImporterResources::Material& gltfMaterial = resources.m_materials[i];
						MeshImporterResult::Material& material = commonResult.m_materials[i];

						// Name
						material.m_name = gltfMaterial.m_name;

						// AlphaMode
						if (gltfMaterial.m_alphaMode == "OPAQUE")
						{
							material.m_alphaMode = MeshImporterResult::AlphaMode::Opaque;
						}
						else if (gltfMaterial.m_alphaMode == "MASK")
						{
							material.m_alphaMode = MeshImporterResult::AlphaMode::Mask;
						}
						else if (gltfMaterial.m_alphaMode == "BLEND")
						{
							material.m_alphaMode = MeshImporterResult::AlphaMode::Blend;
						}
						material.m_alphaCutoff = gltfMaterial.m_alphaCutoff;

						// double sided
						material.m_doubleSided = gltfMaterial.m_doubleSided;

						{
							std::hash<std::string> hasher;

							// Base Color
							material.m_baseColorFactor = gltfMaterial.m_baseColorFactor;

							if (gltfMaterial.m_baseColorTextureIndex != -1)
							{
								const GLTFMeshImporterResources::Texture& gltfBaseColorTexture = resources.m_textures[gltfMaterial.m_baseColorTextureIndex];
								const GLTFMeshImporterResources::Image& gltfBaseColorImage = resources.m_images[gltfBaseColorTexture.m_imageSource];

								material.m_baseColorTexture.m_name = gltfBaseColorImage.m_name;
								material.m_baseColorTexture.m_uri = fs::path(gltfBaseColorImage.m_uri).filename().string();
								material.m_baseColorTexture.m_uriHash = hasher(material.m_baseColorTexture.m_uri);
								material.m_baseColorTextureSampler = samplers[gltfBaseColorTexture.m_sampler];
								material.m_baseColorTextureTexcoord = gltfMaterial.m_baseColorTextureTexcoordIndex;
							}

							// MetallicRoughness
							material.m_metallicFactor = gltfMaterial.m_metallicFactor;
							material.m_roughnessFactor = gltfMaterial.m_roughnessFactor;

							if (gltfMaterial.m_metallicRoughnessTextureIndex != -1)
							{
								const GLTFMeshImporterResources::Texture& gltfMetallicRoughnessTexture = resources.m_textures[gltfMaterial.m_metallicRoughnessTextureIndex];
								const GLTFMeshImporterResources::Image& gltfMetallicRoughnessImage = resources.m_images[gltfMetallicRoughnessTexture.m_imageSource];

								material.m_metallicRoughnessTexture.m_name = gltfMetallicRoughnessImage.m_name;
								material.m_metallicRoughnessTexture.m_uri = fs::path(gltfMetallicRoughnessImage.m_uri).filename().string();
								material.m_metallicRoughnessTexture.m_uriHash = hasher(material.m_metallicRoughnessTexture.m_uri);
								material.m_metallicRoughnessTextureSampler = samplers[gltfMetallicRoughnessTexture.m_sampler];
								material.m_metallicRoughnessTextureTexcoord = gltfMaterial.m_metallicRoughnessTextureTexcoordIndex;
							}

							// Normal
							material.m_normalScale = gltfMaterial.m_normalScale;

							if (gltfMaterial.m_normalTextureIndex != -1)
							{
								const GLTFMeshImporterResources::Texture& gltfNormalTexture = resources.m_textures[gltfMaterial.m_normalTextureIndex];
								const GLTFMeshImporterResources::Image& gltfNormalImage = resources.m_images[gltfNormalTexture.m_imageSource];

								material.m_normalTexture.m_name = gltfNormalImage.m_name;
								material.m_normalTexture.m_uri = fs::path(gltfNormalImage.m_uri).filename().string();
								material.m_normalTexture.m_uriHash = hasher(material.m_normalTexture.m_uri);
								material.m_normalTextureSampler = samplers[gltfNormalTexture.m_sampler];
								material.m_normalTextureTexcoord = gltfMaterial.m_normalTextureTexcoordIndex;
							}

							// Occlusion
							material.m_occlusionStrength = gltfMaterial.m_occlusionStrength;

							if (gltfMaterial.m_occlusionTextureIndex != -1)
							{
								const GLTFMeshImporterResources::Texture& gltfOcclusionTexture = resources.m_textures[gltfMaterial.m_occlusionTextureIndex];
								const GLTFMeshImporterResources::Image& gltfOcclusionImage = resources.m_images[gltfOcclusionTexture.m_imageSource];

								material.m_occlusionTexture.m_name = gltfOcclusionImage.m_name;
								material.m_occlusionTexture.m_uri = fs::path(gltfOcclusionImage.m_uri).filename().string();
								material.m_occlusionTexture.m_uriHash = hasher(material.m_occlusionTexture.m_uri);
								material.m_occlusionTextureSampler = samplers[gltfOcclusionTexture.m_sampler];
								material.m_occlusionTextureTexcoord = gltfMaterial.m_occlusionTextureTexcoordIndex;
							}
							
							// Emmisive
							material.m_emissiveFactor = gltfMaterial.m_emissiveFactor;

							if (gltfMaterial.m_emissiveTextureIndex != -1)
							{
								const GLTFMeshImporterResources::Texture& gltfEmissiveTexture = resources.m_textures[gltfMaterial.m_emissiveTextureIndex];
								const GLTFMeshImporterResources::Image& gltfEmissiveImage = resources.m_images[gltfEmissiveTexture.m_imageSource];

								material.m_emissiveTexture.m_name = gltfEmissiveImage.m_name;
								material.m_emissiveTexture.m_uri = fs::path(gltfEmissiveImage.m_uri).filename().string();
								material.m_emissiveTexture.m_uriHash = hasher(material.m_emissiveTexture.m_uri);
								material.m_emissiveTextureSampler = samplers[gltfEmissiveTexture.m_sampler];
								material.m_emissiveTextureTexcoord = gltfMaterial.m_emissiveTextureTexcoordIndex;
							}
						}
					}
				}

				// Mesh
				{
					commonResult.m_meshes.resize(resources.m_meshes.size());
					for (u64 i = 0u; i < commonResult.m_meshes.size(); ++i)
					{
						const GLTFMeshImporterResources::Mesh& gltfMesh = resources.m_meshes[i];
						MeshImporterResult::Mesh& mesh = commonResult.m_meshes[i];

						mesh.m_name = gltfMesh.m_name;
						mesh.m_world = gltfMesh.m_worldTransform;

						mesh.m_subMeshes.resize(gltfMesh.m_subMeshes.size());
						for (u64 si = 0u; si < mesh.m_subMeshes.size(); ++si)
						{
							const GLTFMeshImporterResources::Mesh::SubMesh& gltfSubMesh = gltfMesh.m_subMeshes[si];
							MeshImporterResult::Mesh::SubMesh& subMesh = mesh.m_subMeshes[si];

							subMesh.m_material = gltfSubMesh.m_material;
							
							// Triangle
							if (gltfSubMesh.m_mode == 4 || gltfSubMesh.m_mode == -1)
							{
								subMesh.m_mode = MeshImporterResult::Mesh::Mode::Triangles;
							}
							else
							{
								TS_ASSERT(false, "対応していないプリミティブトポロジーです");
							}

							subMesh.m_positions = gltfSubMesh.m_positions;

							if (!gltfSubMesh.m_normals.empty())
							{
								subMesh.m_normals = gltfSubMesh.m_normals;
							}

							if (!gltfSubMesh.m_texcoords0.empty())
							{
								subMesh.m_texcoords0 = gltfSubMesh.m_texcoords0;
							}

							if (!gltfSubMesh.m_texcoords1.empty())
							{
								subMesh.m_texcoords1 = gltfSubMesh.m_texcoords1;
							}

							if (std::holds_alternative<std::vector<u8>>(gltfSubMesh.m_indices))
							{
								subMesh.m_indicesU8 = std::get<std::vector<u8>>(gltfSubMesh.m_indices);
							}
							else if (std::holds_alternative<std::vector<u16>>(gltfSubMesh.m_indices))
							{
								subMesh.m_indicesU16 = std::get<std::vector<u16>>(gltfSubMesh.m_indices);
							}
							else if (std::holds_alternative<std::vector<u32>>(gltfSubMesh.m_indices))
							{
								subMesh.m_indicesU32 = std::get<std::vector<u32>>(gltfSubMesh.m_indices);
							}
						}
					}
				}

				return commonResult;
			}
		} // namespace asset
	} // namespace kit
} // namespace ts