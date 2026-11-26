// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/DX11_Shader.hpp"
#include "Graphics/RHI/DX11_Device.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			DX11_Shader::DX11_Shader() : RHI_Shader(),
				m_vertexShader(),
				m_pixelShader(),
				m_computeShader()
			{
			}
			
			bool DX11_Shader::Create(
				graphics::RHI_Device* device,
				const graphics::RHI_ShaderDesc& desc
			)
			{
				auto dx11Device = static_cast<DX11_Device*>(device);
				ID3D11Device* nativeDevice = dx11Device->GetNativeDevice();

				HRESULT hr = S_OK;

				switch (desc.m_type)
				{
					case graphics::RHI_ShaderType::VertexShader:
					{
						hr = nativeDevice->CreateVertexShader(
							desc.m_shaderBinary,
							desc.m_shaderBinarySize,
							nullptr,
							m_vertexShader.ReleaseAndGetAddressOf()
						);
						if (hr != S_OK)
						{
							TS_FATAL_LOG("VSの作成に失敗しました。");

							return false;
						}
					}
					break;
					case graphics::RHI_ShaderType::PixelShader:
					{
						hr = nativeDevice->CreatePixelShader(
							desc.m_shaderBinary,
							desc.m_shaderBinarySize,
							nullptr,
							m_pixelShader.ReleaseAndGetAddressOf()
						);
						if (hr != S_OK)
						{
							TS_FATAL_LOG("PSの作成に失敗しました。");

							return false;
						}
					}
					break;
					case graphics::RHI_ShaderType::ComputeShader:
					{
						hr = nativeDevice->CreateComputeShader(
							desc.m_shaderBinary,
							desc.m_shaderBinarySize,
							nullptr,
							m_computeShader.ReleaseAndGetAddressOf()
						);
						if (hr != S_OK)
						{
							TS_FATAL_LOG("CSの作成に失敗しました。");

							return false;
						}
					}
					break;
					default:
					{
						TS_FATAL_LOG("不明なシェーダータイプです");

						return false;
					}
					break;
				}

				m_type = desc.m_type;
				m_shaderBinarySize = desc.m_shaderBinarySize;
				
				m_shaderBinary = new u8[m_shaderBinarySize];
				std::memcpy(m_shaderBinary, desc.m_shaderBinary, m_shaderBinarySize);

				return true;
			}
			
			bool DX11_Shader::Destroy()
			{
				Release();

				return true;
			}

			void DX11_Shader::Release()
			{
				TS_SAFE_RELEASE_ARRAY(m_shaderBinary);

				switch (m_type)
				{
				case graphics::RHI_ShaderType::VertexShader:
				{
					m_vertexShader.Reset();
				}
				break;
				case graphics::RHI_ShaderType::PixelShader:
				{
					m_pixelShader.Reset();
				}
				break;
				case graphics::RHI_ShaderType::ComputeShader:
				{
					m_computeShader.Reset();
				}
				break;
				}

				delete this;
			}
		} // namespace dx11
	} // namespace kit
} // namespace ts