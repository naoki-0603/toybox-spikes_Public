// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/DX11_StateManager.hpp"

#include "Core/Hash.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			namespace internal
			{
				D3D11_COMPARISON_FUNC ToD3D11ComparsionFunc(graphics::RHI_ComparsionFunc func)
				{
					switch (func)
					{
					case graphics::RHI_ComparsionFunc::None: return D3D11_COMPARISON_NOT_EQUAL;
					case graphics::RHI_ComparsionFunc::Less: return D3D11_COMPARISON_LESS;
					case graphics::RHI_ComparsionFunc::Equal: return D3D11_COMPARISON_EQUAL;
					case graphics::RHI_ComparsionFunc::LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
					case graphics::RHI_ComparsionFunc::Greater: return D3D11_COMPARISON_GREATER;
					case graphics::RHI_ComparsionFunc::NotEqual: return D3D11_COMPARISON_NOT_EQUAL;
					case graphics::RHI_ComparsionFunc::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
					case graphics::RHI_ComparsionFunc::Always: return D3D11_COMPARISON_ALWAYS;
					}

					TS_ASSERT(false, "不明な比較関数です");

					return {};
				}

				D3D11_BLEND ToD3D11Blend(graphics::RHI_BlendMode blendMode)
				{
					switch (blendMode)
					{
					case graphics::RHI_BlendMode::Zero: return D3D11_BLEND_ZERO;
					case graphics::RHI_BlendMode::One: return D3D11_BLEND_ONE;
					case graphics::RHI_BlendMode::SrcColor: return D3D11_BLEND_SRC_COLOR;
					case graphics::RHI_BlendMode::InvSrcColor: return D3D11_BLEND_INV_SRC_COLOR;
					case graphics::RHI_BlendMode::SrcAlpha: return D3D11_BLEND_SRC_ALPHA;
					case graphics::RHI_BlendMode::InvSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
					case graphics::RHI_BlendMode::DestAlpha: return D3D11_BLEND_DEST_ALPHA;
					case graphics::RHI_BlendMode::InvDestAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
					case graphics::RHI_BlendMode::DestColor: return D3D11_BLEND_DEST_COLOR;
					case graphics::RHI_BlendMode::InvDestColor: return D3D11_BLEND_INV_DEST_COLOR;
					}

					TS_ASSERT(false, "未対応か不明なブレンドモードです。");

					return {};
				}
				
				D3D11_BLEND_OP ToD3D11BlendOp(graphics::RHI_BlendOperation blendOp)
				{
					switch (blendOp)
					{
					case graphics::RHI_BlendOperation::Add: return D3D11_BLEND_OP_ADD;
					case graphics::RHI_BlendOperation::Subtract: return D3D11_BLEND_OP_SUBTRACT;
					case graphics::RHI_BlendOperation::RevSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
					}

					TS_ASSERT(false, "未対応か不明なブレンドオペレーションです。");

					return {};
				}

				UINT ToD3D11ColorWriteEnable(graphics::RHI_ColorWriteMask mask)
				{
					UINT writeMask = {};

					if ((mask & graphics::RHI_ColorWriteMask::WriteEnableRed) == graphics::RHI_ColorWriteMask::WriteEnableRed)
					{
						writeMask |= D3D11_COLOR_WRITE_ENABLE_RED;
					}

					if ((mask & graphics::RHI_ColorWriteMask::WriteEnableGreen) == graphics::RHI_ColorWriteMask::WriteEnableGreen)
					{
						writeMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
					}

					if ((mask & graphics::RHI_ColorWriteMask::WriteEnableBlue) == graphics::RHI_ColorWriteMask::WriteEnableBlue)
					{
						writeMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
					}

					if ((mask & graphics::RHI_ColorWriteMask::WriteEnableAlpha) == graphics::RHI_ColorWriteMask::WriteEnableAlpha)
					{
						writeMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
					}

					return writeMask;
				}

				D3D11_DEPTH_WRITE_MASK ToD3D11DepthWriteMask(graphics::RHI_DepthWriteMask mask)
				{
					switch (mask)
					{
					case graphics::RHI_DepthWriteMask::Zero: return D3D11_DEPTH_WRITE_MASK_ZERO;
					case graphics::RHI_DepthWriteMask::All: return D3D11_DEPTH_WRITE_MASK_ALL;
					}

					TS_ASSERT(false, "不明な書き込みマスクです");

					return {};
				}

				D3D11_STENCIL_OP ToD3D11StencilOp(graphics::RHI_StencilOperation stencilOp)
				{
					switch (stencilOp)
					{
					case graphics::RHI_StencilOperation::Keep: return D3D11_STENCIL_OP_KEEP;	
					case graphics::RHI_StencilOperation::Zero: return D3D11_STENCIL_OP_ZERO;	
					case graphics::RHI_StencilOperation::Replace: return D3D11_STENCIL_OP_REPLACE;
					case graphics::RHI_StencilOperation::IncrSat: return D3D11_STENCIL_OP_INCR_SAT;
					case graphics::RHI_StencilOperation::DecrSat: return D3D11_STENCIL_OP_DECR_SAT;
					case graphics::RHI_StencilOperation::Invert: return D3D11_STENCIL_OP_INVERT;
					case graphics::RHI_StencilOperation::Incr: return D3D11_STENCIL_OP_INCR;	
					case graphics::RHI_StencilOperation::Decr: return D3D11_STENCIL_OP_DECR;	
					}
					
					TS_ASSERT(false, "不明なステンシル演算です");

					return {};
				}

				D3D11_CULL_MODE ToD3D11CullMode(graphics::RHI_CullMode cullMode)
				{
					switch (cullMode)
					{
					case graphics::RHI_CullMode::None: return D3D11_CULL_NONE;
					case graphics::RHI_CullMode::Front: return D3D11_CULL_FRONT;
					case graphics::RHI_CullMode::Back: return D3D11_CULL_BACK;
					}

					TS_FATAL_LOG("不明なCullModeです");

					return static_cast<D3D11_CULL_MODE>(-1);
				}

				D3D11_FILL_MODE ToD3D11FillMode(graphics::RHI_FillMode fillMode)
				{
					switch (fillMode)
					{
					case graphics::RHI_FillMode::Solid: return D3D11_FILL_SOLID;
					case graphics::RHI_FillMode::Wireframe: return D3D11_FILL_WIREFRAME;
					}

					TS_FATAL_LOG("不明なFillModeです");

					return static_cast<D3D11_FILL_MODE>(-1);
				}

				D3D11_FILTER ToD3D11Filter(graphics::RHI_SamplerFilter filter)
				{
					switch (filter)
					{
					case graphics::RHI_SamplerFilter::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
					case graphics::RHI_SamplerFilter::Linear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
					case graphics::RHI_SamplerFilter::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
					}

					TS_ASSERT(false, "未対応か不明なフィルターです");

					return D3D11_FILTER();
				}

				D3D11_TEXTURE_ADDRESS_MODE ToD3D11TextureAddressMode(graphics::RHI_SamplerTextureAddress address)
				{
					switch (address)
					{
					case graphics::RHI_SamplerTextureAddress::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
					case graphics::RHI_SamplerTextureAddress::Clamp: return D3D11_TEXTURE_ADDRESS_CLAMP;
					case graphics::RHI_SamplerTextureAddress::Border: return D3D11_TEXTURE_ADDRESS_BORDER;
					}

					TS_ASSERT(false, "未対応か不明なモードです");

					return D3D11_TEXTURE_ADDRESS_MODE();
				}
			} // namespace internal

			DX11_StateManager::DX11_StateManager() :
				m_blendStateCache(),
				m_depthStencilStateCache(),
				m_rasterizerStateCache(),
				m_samplerStateCache()
			{
			}

			bool DX11_StateManager::Create(
				ID3D11Device* device
			)
			{
				if (!CreateDefaultBlendStates(device))
				{
					return false;
				}

				if (!CreateDefaultDepthStencilStates(device))
				{
					return false;
				}

				if (!CreateDefaultRasterizerStates(device))
				{
					return false;
				}

				if (!CreateDefaultSamplerStates(device))
				{
					return false;
				}

				return true;
			}

			bool DX11_StateManager::Destroy()
			{
				m_blendStateCache.clear();
				m_depthStencilStateCache.clear();
				m_rasterizerStateCache.clear();
				m_samplerStateCache.clear();

				return true;
			}

			ID3D11BlendState* DX11_StateManager::GetOrCreateBlendState(
				ID3D11Device* device,
				const graphics::RHI_BlendState& blendState
			)
			{
				u64 seed = 0u;

				// ハッシュ値の計算と重複確認
				{
					hash::HashCombine(seed, blendState.m_blendEnable);
					hash::HashCombine(seed, blendState.m_alphaToCoverageEnable);
					hash::HashCombine(seed, blendState.m_srcBlend);
					hash::HashCombine(seed, blendState.m_destBlend);
					hash::HashCombine(seed, blendState.m_blendOp);
					hash::HashCombine(seed, blendState.m_srcBlendAlpha);
					hash::HashCombine(seed, blendState.m_destBlendAlpha);
					hash::HashCombine(seed, blendState.m_blendOpAlpha);
					hash::HashCombine(seed, blendState.m_renderTargetWriteMask);

					if (m_blendStateCache.contains(seed))
					{
						return m_blendStateCache.find(seed)->second.Get();
					}
				}

				ComPtr<ID3D11BlendState> state = nullptr;

				// ブレンドステートを作成
				{
					D3D11_BLEND_DESC desc = {};
					desc.AlphaToCoverageEnable = blendState.m_alphaToCoverageEnable;
					desc.RenderTarget[0].BlendEnable = blendState.m_blendEnable;
					desc.RenderTarget[0].SrcBlend = internal::ToD3D11Blend(blendState.m_srcBlend);
					desc.RenderTarget[0].DestBlend = internal::ToD3D11Blend(blendState.m_destBlend);
					desc.RenderTarget[0].BlendOp = internal::ToD3D11BlendOp(blendState.m_blendOp);
					desc.RenderTarget[0].SrcBlendAlpha = internal::ToD3D11Blend(blendState.m_srcBlendAlpha);
					desc.RenderTarget[0].DestBlendAlpha = internal::ToD3D11Blend(blendState.m_destBlendAlpha);
					desc.RenderTarget[0].BlendOpAlpha = internal::ToD3D11BlendOp(blendState.m_blendOpAlpha);
					desc.RenderTarget[0].RenderTargetWriteMask = internal::ToD3D11ColorWriteEnable(
						blendState.m_renderTargetWriteMask
					);

					const HRESULT hr = device->CreateBlendState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "ブレンドステートの作成に失敗しました。");

					m_blendStateCache.emplace(seed, state);
				}

				return state.Get();
			}

			ID3D11DepthStencilState* DX11_StateManager::GetOrCreateDepthStencilState(
				ID3D11Device* device,
				const graphics::RHI_DepthStencilState& depthStencilState
			)
			{
				u64 seed = 0u;

				// ハッシュ値の計算と重複確認
				{
					hash::HashCombine(seed, depthStencilState.m_depthEnable);
					hash::HashCombine(seed, depthStencilState.m_depthWriteMask);
					hash::HashCombine(seed, depthStencilState.m_depthFunc);
					hash::HashCombine(seed, depthStencilState.m_stencilEnable);
					hash::HashCombine(seed, depthStencilState.m_stencilReadMask);
					hash::HashCombine(seed, depthStencilState.m_stencilWriteMask);
					hash::HashCombine(seed, depthStencilState.m_frontFace.m_stencilFailOp);
					hash::HashCombine(seed, depthStencilState.m_frontFace.m_stencilDepthFailOp);
					hash::HashCombine(seed, depthStencilState.m_frontFace.m_stencilPassOp);
					hash::HashCombine(seed, depthStencilState.m_frontFace.m_stencilFunc);
					hash::HashCombine(seed, depthStencilState.m_backFace.m_stencilFailOp);
					hash::HashCombine(seed, depthStencilState.m_backFace.m_stencilDepthFailOp);
					hash::HashCombine(seed, depthStencilState.m_backFace.m_stencilPassOp);
					hash::HashCombine(seed, depthStencilState.m_backFace.m_stencilFunc);

					if (m_depthStencilStateCache.contains(seed))
					{
						return m_depthStencilStateCache.find(seed)->second.Get();
					}
				}

				ComPtr<ID3D11DepthStencilState> state = nullptr;

				// デプスステンシルステートを作成
				{
					D3D11_DEPTH_STENCIL_DESC desc = {};
					desc.DepthEnable = depthStencilState.m_depthEnable;
					desc.DepthWriteMask = internal::ToD3D11DepthWriteMask(depthStencilState.m_depthWriteMask);
					desc.DepthFunc = internal::ToD3D11ComparsionFunc(depthStencilState.m_depthFunc);
					desc.StencilEnable = depthStencilState.m_stencilEnable;
					desc.StencilReadMask = depthStencilState.m_stencilReadMask;
					desc.StencilWriteMask = depthStencilState.m_stencilWriteMask;
					desc.FrontFace.StencilFailOp = internal::ToD3D11StencilOp(depthStencilState.m_frontFace.m_stencilFailOp);
					desc.FrontFace.StencilFunc = internal::ToD3D11ComparsionFunc(depthStencilState.m_frontFace.m_stencilFunc);
					desc.FrontFace.StencilDepthFailOp = internal::ToD3D11StencilOp(depthStencilState.m_frontFace.m_stencilDepthFailOp);
					desc.FrontFace.StencilPassOp = internal::ToD3D11StencilOp(depthStencilState.m_frontFace.m_stencilPassOp);;
					desc.BackFace.StencilFailOp = internal::ToD3D11StencilOp(depthStencilState.m_backFace.m_stencilFailOp);
					desc.BackFace.StencilFunc = internal::ToD3D11ComparsionFunc(depthStencilState.m_backFace.m_stencilFunc);
					desc.BackFace.StencilDepthFailOp = internal::ToD3D11StencilOp(depthStencilState.m_backFace.m_stencilDepthFailOp);
					desc.BackFace.StencilPassOp = internal::ToD3D11StencilOp(depthStencilState.m_backFace.m_stencilPassOp);

					const HRESULT hr = device->CreateDepthStencilState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "デプスステンシルステートの作成に失敗しました。");

					m_depthStencilStateCache.emplace(seed, state);
				}

				return state.Get();
			}

			ID3D11RasterizerState* DX11_StateManager::GetOrCreateRasterizerState(
				ID3D11Device* device,
				const graphics::RHI_RasterizerState& rasterizerState
			)
			{
				u64 seed = 0u;

				// ハッシュ値の計算と重複確認
				{
					hash::HashCombine(seed, rasterizerState.m_fillMode);
					hash::HashCombine(seed, rasterizerState.m_cullMode);
					hash::HashCombine(seed, rasterizerState.m_frontCounterClockwise);
					hash::HashCombine(seed, rasterizerState.m_depthBias);
					hash::HashCombine(seed, rasterizerState.m_depthBiasClamp);
					hash::HashCombine(seed, rasterizerState.m_slopeScaledDepthBias);
					hash::HashCombine(seed, rasterizerState.m_depthClipEnable);
					hash::HashCombine(seed, rasterizerState.m_scissorEnable);
					hash::HashCombine(seed, rasterizerState.m_multisampleEnable);
					hash::HashCombine(seed, rasterizerState.m_antialiasedLineEnable);

					if (m_rasterizerStateCache.contains(seed))
					{
						return m_rasterizerStateCache.find(seed)->second.Get();
					}
				}

				ComPtr<ID3D11RasterizerState> state = nullptr;

				// ラスタライザステートを作成
				{
					D3D11_RASTERIZER_DESC desc = {};
					desc.CullMode = internal::ToD3D11CullMode(rasterizerState.m_cullMode);
					desc.FillMode = internal::ToD3D11FillMode(rasterizerState.m_fillMode);
					desc.FrontCounterClockwise = rasterizerState.m_frontCounterClockwise;
					desc.DepthBias = rasterizerState.m_depthBias;
					desc.DepthBiasClamp = rasterizerState.m_depthBiasClamp;
					desc.SlopeScaledDepthBias = rasterizerState.m_slopeScaledDepthBias;
					desc.DepthClipEnable = rasterizerState.m_depthClipEnable;
					desc.MultisampleEnable = rasterizerState.m_multisampleEnable;
					desc.AntialiasedLineEnable = rasterizerState.m_antialiasedLineEnable;
					desc.ScissorEnable = rasterizerState.m_scissorEnable;

					const HRESULT hr = device->CreateRasterizerState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "ラスタライザステートの作成に失敗しました。");

					m_rasterizerStateCache.emplace(seed, state);
				}
				
				return state.Get();
			}

			ID3D11SamplerState* DX11_StateManager::GetOrCreateSamplerState(
				ID3D11Device* device,
				const graphics::RHI_SamplerState& samplerState
			)
			{
				u64 seed = 0u;

				// ハッシュ値の計算と重複確認
				{
					hash::HashCombine(seed, samplerState.m_filter);
					hash::HashCombine(seed, samplerState.m_addressU);
					hash::HashCombine(seed, samplerState.m_addressV);
					hash::HashCombine(seed, samplerState.m_addressW);
					hash::HashCombine(seed, samplerState.m_mipLODBias);
					hash::HashCombine(seed, samplerState.m_maxAnisotropy);
					hash::HashCombine(seed, samplerState.m_comparisonFunc);
					hash::HashCombine(seed, samplerState.m_borderColor.x);
					hash::HashCombine(seed, samplerState.m_borderColor.y);
					hash::HashCombine(seed, samplerState.m_borderColor.z);
					hash::HashCombine(seed, samplerState.m_borderColor.w);
					hash::HashCombine(seed, samplerState.m_minLOD);
					hash::HashCombine(seed, samplerState.m_maxLOD);

					if (m_samplerStateCache.contains(seed))
					{
						return m_samplerStateCache.find(seed)->second.Get();
					}
				}

				ComPtr<ID3D11SamplerState> state = nullptr;

				{
					D3D11_SAMPLER_DESC desc = {};
					desc.Filter = internal::ToD3D11Filter(samplerState.m_filter);
					desc.AddressU = internal::ToD3D11TextureAddressMode(samplerState.m_addressU);
					desc.AddressV = internal::ToD3D11TextureAddressMode(samplerState.m_addressV);
					desc.AddressW = internal::ToD3D11TextureAddressMode(samplerState.m_addressW);
					desc.MipLODBias = samplerState.m_mipLODBias;
					desc.MaxAnisotropy = samplerState.m_maxAnisotropy;
					desc.ComparisonFunc = internal::ToD3D11ComparsionFunc(samplerState.m_comparisonFunc);
					desc.BorderColor[0] = samplerState.m_borderColor.x;
					desc.BorderColor[1] = samplerState.m_borderColor.y;
					desc.BorderColor[2] = samplerState.m_borderColor.z;
					desc.BorderColor[3] = samplerState.m_borderColor.w;
					desc.MinLOD = samplerState.m_minLOD;
					desc.MaxLOD = samplerState.m_maxLOD;

					const HRESULT hr = device->CreateSamplerState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "サンプラーステートの作成に失敗しました。");

					m_samplerStateCache.emplace(seed, state);
				}

				return state.Get();
			}
			
			bool DX11_StateManager::CreateDefaultBlendStates(ID3D11Device* device)
			{
				using namespace graphics;
				
				// NoBlend
				{
					u64 seed = 0u;

					const RHI_BlendState noBlendState = RHI_BlendState::GetNoBlend();

					hash::HashCombine(seed, noBlendState.m_blendEnable);
					hash::HashCombine(seed, noBlendState.m_alphaToCoverageEnable);
					hash::HashCombine(seed, noBlendState.m_srcBlend);
					hash::HashCombine(seed, noBlendState.m_destBlend);
					hash::HashCombine(seed, noBlendState.m_blendOp);
					hash::HashCombine(seed, noBlendState.m_srcBlendAlpha);
					hash::HashCombine(seed, noBlendState.m_destBlendAlpha);
					hash::HashCombine(seed, noBlendState.m_blendOpAlpha);
					hash::HashCombine(seed, noBlendState.m_renderTargetWriteMask);

					D3D11_BLEND_DESC desc = {};
					desc.AlphaToCoverageEnable = noBlendState.m_alphaToCoverageEnable;
					desc.RenderTarget[0].BlendEnable = noBlendState.m_blendEnable;
					desc.RenderTarget[0].SrcBlend = internal::ToD3D11Blend(noBlendState.m_srcBlend);
					desc.RenderTarget[0].DestBlend = internal::ToD3D11Blend(noBlendState.m_destBlend);
					desc.RenderTarget[0].BlendOp = internal::ToD3D11BlendOp(noBlendState.m_blendOp);
					desc.RenderTarget[0].SrcBlendAlpha = internal::ToD3D11Blend(noBlendState.m_srcBlendAlpha);
					desc.RenderTarget[0].DestBlendAlpha = internal::ToD3D11Blend(noBlendState.m_destBlendAlpha);
					desc.RenderTarget[0].BlendOpAlpha = internal::ToD3D11BlendOp(noBlendState.m_blendOpAlpha);
					desc.RenderTarget[0].RenderTargetWriteMask = internal::ToD3D11ColorWriteEnable(
						noBlendState.m_renderTargetWriteMask
					);

					ComPtr<ID3D11BlendState> state = nullptr;

					const HRESULT hr = device->CreateBlendState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "ブレンドステートの作成に失敗しました。");

					m_blendStateCache.emplace(seed, state);
				}

				return true;
			}
			
			bool DX11_StateManager::CreateDefaultDepthStencilStates(ID3D11Device* device)
			{
				using namespace graphics;

				// DepthOnStencilOn
				{
					u64 seed = 0u;

					const RHI_DepthStencilState depthOnStencilOn = RHI_DepthStencilState::GetDepthOnStencilOn();

					hash::HashCombine(seed, depthOnStencilOn.m_depthEnable);
					hash::HashCombine(seed, depthOnStencilOn.m_depthWriteMask);
					hash::HashCombine(seed, depthOnStencilOn.m_depthFunc);
					hash::HashCombine(seed, depthOnStencilOn.m_stencilEnable);
					hash::HashCombine(seed, depthOnStencilOn.m_stencilReadMask);
					hash::HashCombine(seed, depthOnStencilOn.m_stencilWriteMask);
					hash::HashCombine(seed, depthOnStencilOn.m_frontFace.m_stencilFailOp);
					hash::HashCombine(seed, depthOnStencilOn.m_frontFace.m_stencilDepthFailOp);
					hash::HashCombine(seed, depthOnStencilOn.m_frontFace.m_stencilPassOp);
					hash::HashCombine(seed, depthOnStencilOn.m_frontFace.m_stencilFunc);
					hash::HashCombine(seed, depthOnStencilOn.m_backFace.m_stencilFailOp);
					hash::HashCombine(seed, depthOnStencilOn.m_backFace.m_stencilDepthFailOp);
					hash::HashCombine(seed, depthOnStencilOn.m_backFace.m_stencilPassOp);
					hash::HashCombine(seed, depthOnStencilOn.m_backFace.m_stencilFunc);

					ComPtr<ID3D11DepthStencilState> state = nullptr;

					D3D11_DEPTH_STENCIL_DESC desc = {};
					desc.DepthEnable = depthOnStencilOn.m_depthEnable;
					desc.DepthWriteMask = internal::ToD3D11DepthWriteMask(depthOnStencilOn.m_depthWriteMask);
					desc.DepthFunc = internal::ToD3D11ComparsionFunc(depthOnStencilOn.m_depthFunc);
					desc.StencilEnable = depthOnStencilOn.m_stencilEnable;
					desc.StencilReadMask = depthOnStencilOn.m_stencilReadMask;
					desc.StencilWriteMask = depthOnStencilOn.m_stencilWriteMask;
					desc.FrontFace.StencilFailOp = internal::ToD3D11StencilOp(depthOnStencilOn.m_frontFace.m_stencilFailOp);
					desc.FrontFace.StencilFunc = internal::ToD3D11ComparsionFunc(depthOnStencilOn.m_frontFace.m_stencilFunc);
					desc.FrontFace.StencilDepthFailOp = internal::ToD3D11StencilOp(depthOnStencilOn.m_frontFace.m_stencilDepthFailOp);
					desc.FrontFace.StencilPassOp = internal::ToD3D11StencilOp(depthOnStencilOn.m_frontFace.m_stencilPassOp);;
					desc.BackFace.StencilFailOp = internal::ToD3D11StencilOp(depthOnStencilOn.m_backFace.m_stencilFailOp);
					desc.BackFace.StencilFunc = internal::ToD3D11ComparsionFunc(depthOnStencilOn.m_backFace.m_stencilFunc);
					desc.BackFace.StencilDepthFailOp = internal::ToD3D11StencilOp(depthOnStencilOn.m_backFace.m_stencilDepthFailOp);
					desc.BackFace.StencilPassOp = internal::ToD3D11StencilOp(depthOnStencilOn.m_backFace.m_stencilPassOp);

					const HRESULT hr = device->CreateDepthStencilState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "デプスステンシルステートの作成に失敗しました。");

					m_depthStencilStateCache.emplace(seed, state);
				}

				return true;
			}
			
			bool DX11_StateManager::CreateDefaultRasterizerStates(ID3D11Device* device)
			{
				using namespace graphics;

				// FillSolidCullBack
				{
					u64 seed = 0u;
					
					const RHI_RasterizerState fillSolidCullBack = RHI_RasterizerState::GetFillSolidCullBack();

					hash::HashCombine(seed, fillSolidCullBack.m_fillMode);
					hash::HashCombine(seed, fillSolidCullBack.m_cullMode);
					hash::HashCombine(seed, fillSolidCullBack.m_frontCounterClockwise);
					hash::HashCombine(seed, fillSolidCullBack.m_depthBias);
					hash::HashCombine(seed, fillSolidCullBack.m_depthBiasClamp);
					hash::HashCombine(seed, fillSolidCullBack.m_slopeScaledDepthBias);
					hash::HashCombine(seed, fillSolidCullBack.m_depthClipEnable);
					hash::HashCombine(seed, fillSolidCullBack.m_scissorEnable);
					hash::HashCombine(seed, fillSolidCullBack.m_multisampleEnable);
					hash::HashCombine(seed, fillSolidCullBack.m_antialiasedLineEnable);

					ComPtr<ID3D11RasterizerState> state = nullptr;

					D3D11_RASTERIZER_DESC desc = {};
					desc.CullMode = internal::ToD3D11CullMode(fillSolidCullBack.m_cullMode);
					desc.FillMode = internal::ToD3D11FillMode(fillSolidCullBack.m_fillMode);
					desc.FrontCounterClockwise = fillSolidCullBack.m_frontCounterClockwise;
					desc.DepthBias = fillSolidCullBack.m_depthBias;
					desc.DepthBiasClamp = fillSolidCullBack.m_depthBiasClamp;
					desc.SlopeScaledDepthBias = fillSolidCullBack.m_slopeScaledDepthBias;
					desc.DepthClipEnable = fillSolidCullBack.m_depthClipEnable;
					desc.MultisampleEnable = fillSolidCullBack.m_multisampleEnable;
					desc.AntialiasedLineEnable = fillSolidCullBack.m_antialiasedLineEnable;
					desc.ScissorEnable = fillSolidCullBack.m_scissorEnable;

					const HRESULT hr = device->CreateRasterizerState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "ラスタライザステートの作成に失敗しました。");

					m_rasterizerStateCache.emplace(seed, state);
				}

				// FillSolidCullBackWireframe
				{
					u64 seed = 0u;

					const RHI_RasterizerState fillSolidCullBackWireframe = RHI_RasterizerState::GetFillSolidCullBackWireframe();

					hash::HashCombine(seed, fillSolidCullBackWireframe.m_fillMode);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_cullMode);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_frontCounterClockwise);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_depthBias);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_depthBiasClamp);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_slopeScaledDepthBias);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_depthClipEnable);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_scissorEnable);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_multisampleEnable);
					hash::HashCombine(seed, fillSolidCullBackWireframe.m_antialiasedLineEnable);

					ComPtr<ID3D11RasterizerState> state = nullptr;

					D3D11_RASTERIZER_DESC desc = {};
					desc.CullMode = internal::ToD3D11CullMode(fillSolidCullBackWireframe.m_cullMode);
					desc.FillMode = internal::ToD3D11FillMode(fillSolidCullBackWireframe.m_fillMode);
					desc.FrontCounterClockwise = fillSolidCullBackWireframe.m_frontCounterClockwise;
					desc.DepthBias = fillSolidCullBackWireframe.m_depthBias;
					desc.DepthBiasClamp = fillSolidCullBackWireframe.m_depthBiasClamp;
					desc.SlopeScaledDepthBias = fillSolidCullBackWireframe.m_slopeScaledDepthBias;
					desc.DepthClipEnable = fillSolidCullBackWireframe.m_depthClipEnable;
					desc.MultisampleEnable = fillSolidCullBackWireframe.m_multisampleEnable;
					desc.AntialiasedLineEnable = fillSolidCullBackWireframe.m_antialiasedLineEnable;
					desc.ScissorEnable = fillSolidCullBackWireframe.m_scissorEnable;

					const HRESULT hr = device->CreateRasterizerState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "ラスタライザステートの作成に失敗しました。");

					m_rasterizerStateCache.emplace(seed, state);
				}

				return true;
			}

			bool DX11_StateManager::CreateDefaultSamplerStates(ID3D11Device* device)
			{
				using namespace graphics;

				// ポイントサンプラー
				{
					const RHI_SamplerState pointSampler = RHI_SamplerState::GetPoint();

					u64 seed = 0u;

					hash::HashCombine(seed, pointSampler.m_filter);
					hash::HashCombine(seed, pointSampler.m_addressU);
					hash::HashCombine(seed, pointSampler.m_addressV);
					hash::HashCombine(seed, pointSampler.m_addressW);
					hash::HashCombine(seed, pointSampler.m_mipLODBias);
					hash::HashCombine(seed, pointSampler.m_maxAnisotropy);
					hash::HashCombine(seed, pointSampler.m_comparisonFunc);
					hash::HashCombine(seed, pointSampler.m_borderColor.x);
					hash::HashCombine(seed, pointSampler.m_borderColor.y);
					hash::HashCombine(seed, pointSampler.m_borderColor.z);
					hash::HashCombine(seed, pointSampler.m_borderColor.w);
					hash::HashCombine(seed, pointSampler.m_minLOD);
					hash::HashCombine(seed, pointSampler.m_maxLOD);

					ComPtr<ID3D11SamplerState> state = nullptr;

					D3D11_SAMPLER_DESC desc = {};
					desc.Filter = internal::ToD3D11Filter(pointSampler.m_filter);
					desc.AddressU = internal::ToD3D11TextureAddressMode(pointSampler.m_addressU);
					desc.AddressV = internal::ToD3D11TextureAddressMode(pointSampler.m_addressV);
					desc.AddressW = internal::ToD3D11TextureAddressMode(pointSampler.m_addressW);
					desc.MipLODBias = pointSampler.m_mipLODBias;
					desc.MaxAnisotropy = pointSampler.m_maxAnisotropy;
					desc.ComparisonFunc = internal::ToD3D11ComparsionFunc(pointSampler.m_comparisonFunc);
					desc.BorderColor[0] = pointSampler.m_borderColor.x;
					desc.BorderColor[1] = pointSampler.m_borderColor.y;
					desc.BorderColor[2] = pointSampler.m_borderColor.z;
					desc.BorderColor[3] = pointSampler.m_borderColor.w;
					desc.MinLOD = pointSampler.m_minLOD;
					desc.MaxLOD = pointSampler.m_maxLOD;

					const HRESULT hr = device->CreateSamplerState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "サンプラーステートの作成に失敗しました。");

					m_samplerStateCache.emplace(seed, state);
				}

				// リニアーサンプラー
				{
					const RHI_SamplerState linearSampler = RHI_SamplerState::GetLinear();

					u64 seed = 0u;

					hash::HashCombine(seed, linearSampler.m_filter);
					hash::HashCombine(seed, linearSampler.m_addressU);
					hash::HashCombine(seed, linearSampler.m_addressV);
					hash::HashCombine(seed, linearSampler.m_addressW);
					hash::HashCombine(seed, linearSampler.m_mipLODBias);
					hash::HashCombine(seed, linearSampler.m_maxAnisotropy);
					hash::HashCombine(seed, linearSampler.m_comparisonFunc);
					hash::HashCombine(seed, linearSampler.m_borderColor.x);
					hash::HashCombine(seed, linearSampler.m_borderColor.y);
					hash::HashCombine(seed, linearSampler.m_borderColor.z);
					hash::HashCombine(seed, linearSampler.m_borderColor.w);
					hash::HashCombine(seed, linearSampler.m_minLOD);
					hash::HashCombine(seed, linearSampler.m_maxLOD);

					ComPtr<ID3D11SamplerState> state = nullptr;

					D3D11_SAMPLER_DESC desc = {};
					desc.Filter = internal::ToD3D11Filter(linearSampler.m_filter);
					desc.AddressU = internal::ToD3D11TextureAddressMode(linearSampler.m_addressU);
					desc.AddressV = internal::ToD3D11TextureAddressMode(linearSampler.m_addressV);
					desc.AddressW = internal::ToD3D11TextureAddressMode(linearSampler.m_addressW);
					desc.MipLODBias = linearSampler.m_mipLODBias;
					desc.MaxAnisotropy = linearSampler.m_maxAnisotropy;
					desc.ComparisonFunc = internal::ToD3D11ComparsionFunc(linearSampler.m_comparisonFunc);
					desc.BorderColor[0] = linearSampler.m_borderColor.x;
					desc.BorderColor[1] = linearSampler.m_borderColor.y;
					desc.BorderColor[2] = linearSampler.m_borderColor.z;
					desc.BorderColor[3] = linearSampler.m_borderColor.w;
					desc.MinLOD = linearSampler.m_minLOD;
					desc.MaxLOD = linearSampler.m_maxLOD;

					const HRESULT hr = device->CreateSamplerState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "サンプラーステートの作成に失敗しました。");

					m_samplerStateCache.emplace(seed, state);
				}

				// リニアーサンプラー
				{
					const RHI_SamplerState anisotropicSampler = RHI_SamplerState::GetAnisotropic();

					u64 seed = 0u;

					hash::HashCombine(seed, anisotropicSampler.m_filter);
					hash::HashCombine(seed, anisotropicSampler.m_addressU);
					hash::HashCombine(seed, anisotropicSampler.m_addressV);
					hash::HashCombine(seed, anisotropicSampler.m_addressW);
					hash::HashCombine(seed, anisotropicSampler.m_mipLODBias);
					hash::HashCombine(seed, anisotropicSampler.m_maxAnisotropy);
					hash::HashCombine(seed, anisotropicSampler.m_comparisonFunc);
					hash::HashCombine(seed, anisotropicSampler.m_borderColor.x);
					hash::HashCombine(seed, anisotropicSampler.m_borderColor.y);
					hash::HashCombine(seed, anisotropicSampler.m_borderColor.z);
					hash::HashCombine(seed, anisotropicSampler.m_borderColor.w);
					hash::HashCombine(seed, anisotropicSampler.m_minLOD);
					hash::HashCombine(seed, anisotropicSampler.m_maxLOD);

					ComPtr<ID3D11SamplerState> state = nullptr;

					D3D11_SAMPLER_DESC desc = {};
					desc.Filter = internal::ToD3D11Filter(anisotropicSampler.m_filter);
					desc.AddressU = internal::ToD3D11TextureAddressMode(anisotropicSampler.m_addressU);
					desc.AddressV = internal::ToD3D11TextureAddressMode(anisotropicSampler.m_addressV);
					desc.AddressW = internal::ToD3D11TextureAddressMode(anisotropicSampler.m_addressW);
					desc.MipLODBias = anisotropicSampler.m_mipLODBias;
					desc.MaxAnisotropy = anisotropicSampler.m_maxAnisotropy;
					desc.ComparisonFunc = internal::ToD3D11ComparsionFunc(anisotropicSampler.m_comparisonFunc);
					desc.BorderColor[0] = anisotropicSampler.m_borderColor.x;
					desc.BorderColor[1] = anisotropicSampler.m_borderColor.y;
					desc.BorderColor[2] = anisotropicSampler.m_borderColor.z;
					desc.BorderColor[3] = anisotropicSampler.m_borderColor.w;
					desc.MinLOD = anisotropicSampler.m_minLOD;
					desc.MaxLOD = anisotropicSampler.m_maxLOD;

					const HRESULT hr = device->CreateSamplerState(
						&desc,
						state.ReleaseAndGetAddressOf()
					);
					TS_ASSERT(hr == S_OK, "サンプラーステートの作成に失敗しました。");

					m_samplerStateCache.emplace(seed, state);
				}
				
				return true;
			}
		} // namespace dx11
	} // namespace kit
} // namespace ts