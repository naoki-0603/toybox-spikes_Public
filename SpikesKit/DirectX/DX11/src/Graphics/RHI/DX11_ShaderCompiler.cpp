// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/DX11_ShaderCompiler.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			DX11_ShaderCompiler::DX11_ShaderCompiler() : RHI_ShaderCompiler()
			{
			}

			bool DX11_ShaderCompiler::Destroy()
			{
				Release();

				return true;
			}

			std::vector<u8> DX11_ShaderCompiler::Compile(
				const graphics::RHI_ShaderCompilerConfig& config
			)
			{
				u32 compilerFlag = 0u;

				if (static_cast<u32>(config.m_options) & static_cast<u32>(graphics::RHI_ShaderCompilerOptions::EnableDebug))
				{
					compilerFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
				}

				std::vector<D3D_SHADER_MACRO> macros = {};
				for (const auto& macro : config.m_macros)
				{
					macros.emplace_back(
						D3D_SHADER_MACRO(macro.m_name.c_str(), macro.m_definition.c_str())
					);
				}

				// 終端を表すNullマクロを挿入する
				macros.emplace_back(D3D_SHADER_MACRO(nullptr, nullptr));

				ComPtr<ID3DBlob> shaderBlob = nullptr;
				ComPtr<ID3DBlob> errorMessage = nullptr;

				const HRESULT hr = D3DCompileFromFile(
					config.m_path.wstring().c_str(),
					macros.empty() ? nullptr : macros.data(),
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					config.m_entryPoint.c_str(),
					config.m_target.c_str(),
					compilerFlag,
					0u,
					shaderBlob.ReleaseAndGetAddressOf(),
					errorMessage.ReleaseAndGetAddressOf()
				);
				if (hr != S_OK)
				{
					TS_ERROR_LOG("{} のコンパイルに失敗しました。", config.m_path.string());
					
					if (errorMessage)
					{
						const std::string errorMsg(
							reinterpret_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()
						);

						TS_FATAL_LOG("Error: {}", errorMsg);
					}
					else
					{
						TS_FATAL_LOG("Error: シェーダーファイルが見つからないか、エラーメッセージが生成されませんでした。");
					}

					return {};
				}
				std::vector<u8> shaderCode(shaderBlob->GetBufferSize());
				std::memcpy(shaderCode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

				return shaderCode;
			}

			void DX11_ShaderCompiler::Release()
			{
				delete this;
			}

			extern "C" __declspec(dllexport) graphics::RHI_ShaderCompiler* CreateRHIShaderCompiler()
			{
				return new DX11_ShaderCompiler;
			}
		} // namespace dx11
	} // namespace kit
} // namespace ts 
