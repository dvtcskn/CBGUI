/* ---------------------------------------------------------------------------------------
* MIT License
*
* Copyright (c) 2022 Davut Coþkun.
* All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* ---------------------------------------------------------------------------------------
*/

#include "pch.h"

#include "Renderer_DX12.h"
#include "WindowsPlatform.h"
#include "cbString.h"

#include "dx12/d3dx12.h"

#include <DirectXMath.h>

using namespace cbgui;

struct UIMaterial_D3D12 : public IUIMaterial
{
	cbClassBody(cbClassConstructor, UIMaterial_D3D12, IUIMaterial)
public:
	UIMaterial_D3D12()
		: Material(nullptr)
	{}
	UIMaterial_D3D12(D3D12Material::D3D12MaterialInstance* Mat)
		: IUIMaterial()
		, Material(Mat)
	{}
	virtual ~UIMaterial_D3D12()
	{
		Material = nullptr;
	}

	D3D12Material::D3D12MaterialInstance* Material;
};

__declspec(align(256)) struct CBGradientIdx
{
	unsigned int GradientIdx;
};
static_assert((sizeof(CBGradientIdx) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

struct OnScreenWidgetMatrixDX12
{
	DirectX::XMMATRIX Matrix;
	DirectX::XMMATRIX Offset[3];
};
static_assert((sizeof(OnScreenWidgetMatrixDX12) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

Renderer_DX12::Renderer_DX12(WindowsPlatform* pOwner)
	: LastMaterial(nullptr)
	, Owner(pOwner)
	, GradientIndex(14)
	, bShowLines(false)
	, bEnableStencilClipping(false)
{
	std::string DepthTestVSShader = "														\
							cbuffer UICBuffer : register(b0)						\
							{														\
								matrix WidgetMatrix;								\
							}														\
																					\
							float4 mainVS(float4 pos : POSITION) : SV_POSITION		\
							{														\
								float4 position = float4(pos.xyz, 1.0f);			\
								position = mul(position, WidgetMatrix);				\
																					\
								position.z = 0.0f;									\
								position.w = 1.0f;									\
																					\
								return position;									\
							}														\
																					\
							float4 mainPS() : SV_TARGET								\
							{														\
								return float4(1.0f, 1.0f, 1.0f, 0.0f);				\
							}";

	std::string WidgetBaseVS = "															\
							cbuffer UICBuffer : register(b0)						\
							{														\
								matrix WidgetMatrix;								\
							};														\
																					\
							struct GeometryVSIn										\
							{														\
								float4 position : POSITION;							\
								float2 texCoord : TEXCOORD;							\
								float4 Color : COLOR;								\
							};														\
																					\
							struct GeometryVSOut									\
							{														\
								float4 position : SV_Position;						\
								float2 texCoord : TEXCOORD;							\
								float4 Color : COLOR;								\
							};														\
																					\
							GeometryVSOut GeometryVS(GeometryVSIn input)			\
							{														\
								GeometryVSOut output;								\
																					\
								float4 pos = float4(input.position.xyz, 1.0f);		\
								pos = mul(pos, WidgetMatrix);						\
																					\
								pos.z = 0.0f;										\
								pos.w = 1.0f;										\
																					\
								output.position = pos;								\
																					\
								output.Color = input.Color;							\
								output.texCoord = input.texCoord;					\
																					\
								return output;										\
							}";

	std::string WidgetBasePS_Flat = "															\
							struct GeometryVSOut										\
							{															\
								float4 position : SV_Position;							\
								float2 texCoord : TEXCOORD;								\
								float4 Color : COLOR;									\
							};															\
																						\
							float4 WidgetFlatColorPS(GeometryVSOut Input) : SV_TARGET	\
							{															\
								return Input.Color;										\
							}";

	std::string WidgetBasePS_Gradient = "																										\
							struct GeometryVSOut																						\
							{																											\
								float4 position : SV_Position;																			\
								float2 texCoord : TEXCOORD;																				\
								float4 Color : COLOR;																					\
							};																											\
																																		\
							cbuffer CBGradientIdx : register(b1)																		\
							{																											\
								uint GradientIdx;																						\
							};																											\
																																		\
							float4 RGBtoFloat(float r, float g, float b)																\
							{																											\
								return float4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);												\
							}																											\
																																		\
							float4 Gradient3(float4 First, float4 Mid, float4 End, float Alpha)											\
							{																											\
								float h = 0.5;																							\
								return lerp(lerp(First, Mid, Alpha / h), lerp(Mid, End, (Alpha - h) / (1.0 - h)), step(h, Alpha));		\
							}																											\
																																		\
							float4 Gradient2(float4 First, float4 End, float Alpha)														\
							{																											\
								return float4(lerp(First, End, Alpha));																	\
							}																											\
							float4 GradientPS(GeometryVSOut Input) : SV_TARGET																										\
							{																																						\
								[forcecase]																																			\
								switch (GradientIdx)																																\
								{																																					\
								case 0: return Gradient2(RGBtoFloat(255.0f, 0.0f, 132.0f), RGBtoFloat(51.0f, 0.0f, 27.0f), Input.texCoord.r);										\
								case 1: return Gradient2(RGBtoFloat(67.0f, 198.0f, 172.0f), RGBtoFloat(25.0f, 22.0f, 84.0f), Input.texCoord.r);										\
								case 2:	return Gradient2(RGBtoFloat(239.0f, 50.0f, 217.0f), RGBtoFloat(137.0f, 255.0f, 253.0f), Input.texCoord.r);									\
								case 3:	return Gradient2(RGBtoFloat(0.0f, 92.0f, 151.0f), RGBtoFloat(54.0f, 55.0f, 149.0f), Input.texCoord.r);										\
								case 4:	return float4(lerp(RGBtoFloat(203.0f, 45.0f, 62.0f), RGBtoFloat(239.0f, 71.0f, 58.0f), Input.texCoord.r));									\
								case 5:	return Gradient3(RGBtoFloat(15.0f, 12.0f, 41.0f), RGBtoFloat(48.0f, 43.0f, 99.0f), RGBtoFloat(36.0f, 36.0f, 62.0f), Input.texCoord.r);		\
								case 6:	return Gradient2(RGBtoFloat(33.0f, 34.0f, 42.0f), RGBtoFloat(58.0f, 96.0f, 115.0f), Input.texCoord.r);										\
								case 7:	return float4(lerp(RGBtoFloat(0.0f, 4.0f, 40.0f), RGBtoFloat(0.0f, 78.0f, 146.0f), Input.texCoord.r));										\
								case 8:	return float4(lerp(RGBtoFloat(233.0f, 100.0f, 67.0f), RGBtoFloat(144.0f, 78.0f, 149.0f), Input.texCoord.r));								\
								case 9:	return float4(lerp(RGBtoFloat(219.0f, 230.0f, 246.0f), RGBtoFloat(197.0f, 121.0f, 109.0f), Input.texCoord.r));								\
								case 10: return float4(lerp(RGBtoFloat(211.0f, 204.0f, 227.0f), RGBtoFloat(233.0f, 228.0f, 240.0f), Input.texCoord.r));								\
								case 11: return float4(lerp(RGBtoFloat(116.0f, 235.0f, 213.0f), RGBtoFloat(172.0f, 182.0f, 229.0f), Input.texCoord.r));								\
								case 12: return float4(lerp(RGBtoFloat(20.0f, 30.0f, 48.0f), RGBtoFloat(36.0f, 59.0f, 85.0f), Input.texCoord.r));									\
								case 13: return Gradient2(RGBtoFloat(0.0f, 0.0f, 0.0f), RGBtoFloat(67.0f, 67.0f, 67.0f), Input.texCoord.r);											\
								case 14: return float4(lerp(RGBtoFloat(96.0f, 108.0f, 136.0f), RGBtoFloat(63.0f, 76.0f, 107.0f), Input.texCoord.r));								\
								case 15: return float4(lerp(RGBtoFloat(96.0f, 108.0f, 136.0f), RGBtoFloat(63.0f, 76.0f, 107.0f), Input.texCoord.r));								\
								}																																					\
																																													\
								return float4(0.0f, 0.0f, 0.0f, 1.0f);																												\
							}";

	std::string WidgetBasePS_Font = "																						\
							struct GeometryVSOut																	\
							{																						\
								float4 position : SV_Position;														\
								float2 texCoord : TEXCOORD;															\
								float4 Color : COLOR;																\
							};																						\
																													\
							Texture2D<float> gFontTexture : register(t0, space0);									\
							SamplerState gLinearSampler : register(s0, space0);										\
																													\
							float4 FontPS(GeometryVSOut input) : SV_Target0											\
							{																						\
								float4 alpha = gFontTexture.Sample(gLinearSampler, input.texCoord);													\
								return float4(input.Color.rgb, input.Color.a * smoothstep(0.0 - (1.0f / 64.0f), 1.0 + (1.0f / 64.0f), alpha.a));	\
							}";

	CMD = std::make_unique<D3D12CommandBuffer>();

	std::vector<FrameBufferAttachmentInfo> Infos;
	{
		FrameBufferAttachmentInfo Info;
		Info.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
		Info.Usage = eImageUsage::Depth;
		Infos.push_back(Info);
	}

	FBODesc Desc;
	Desc.Dimensions.X = Owner->GetWindowWidth();
	Desc.Dimensions.Y = Owner->GetWindowHeight();
	DepthFBO = std::make_unique<D3D12FrameBuffer>("DepthFBO", Infos, Desc);

	{
		sBufferDesc BufferDesc;
		BufferDesc.Size = sizeof(OnScreenWidgetMatrixDX12);
		WidgetConstantBuffer = std::make_unique<D3D12ConstantBuffer>("DepthTest", BufferDesc);

		OnScreenWidgetMatrixDX12 WidgetMatrix;
		WidgetMatrix.Matrix = (DirectX::XMMATRIX&)GetViewportTransform(Owner->GetWindowWidth(), Owner->GetWindowHeight());
		WidgetConstantBuffer->Map(&WidgetMatrix);
	}

	{
		sBufferDesc BufferDesc;
		BufferDesc.Size = sizeof(CBGradientIdx);
		GradientConstantBuffer = std::make_unique<D3D12ConstantBuffer>("GradientConstantBuffer", BufferDesc);

		CBGradientIdx GradientIdx;
		GradientIdx.GradientIdx = GradientIndex;
		GradientConstantBuffer->Map(&GradientIdx);
	}

	{
		D3D12PipelineDesc pPipelineDesc;
		pPipelineDesc.BlendAttribute = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		for (std::size_t i = 0; i < 8; i++)
		{
			pPipelineDesc.BlendAttribute.RenderTarget[i].BlendEnable = true;
			pPipelineDesc.BlendAttribute.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			pPipelineDesc.BlendAttribute.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			pPipelineDesc.BlendAttribute.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		}
		pPipelineDesc.BlendAttribute.IndependentBlendEnable = true;

		pPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pPipelineDesc.RasterizerAttribute = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pPipelineDesc.RasterizerAttribute.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;

		std::vector<VertexAttributeDesc> VertexLayout =
		{
			{ "POSITION",	DXGI_FORMAT_R32G32B32A32_FLOAT,  0,	offsetof(cbgui::cbGeometryVertexData, position),   false },
			{ "TEXCOORD",	DXGI_FORMAT_R32G32_FLOAT,   0,	offsetof(cbgui::cbGeometryVertexData, texCoord),   false },
			{ "COLOR",		DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	offsetof(cbgui::cbGeometryVertexData, Color),	  false },
		};
		pPipelineDesc.VertexLayout = VertexLayout;

		pPipelineDesc.DepthStencilAttribute = CD3DX12_DEPTH_STENCIL_DESC1(D3D12_DEFAULT);
		pPipelineDesc.DepthStencilAttribute.DepthEnable = false;
		pPipelineDesc.DepthStencilAttribute.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pPipelineDesc.DepthStencilAttribute.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		pPipelineDesc.DepthStencilAttribute.StencilEnable = true;

		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		pPipelineDesc.DepthStencilAttribute.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBaseVS.data(), WidgetBaseVS.length(), "GeometryVS", eShaderType::Vertex));
		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBasePS_Flat.data(), WidgetBasePS_Flat.length(), "WidgetFlatColorPS", eShaderType::Pixel));

		pPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Vertex, 0));

		pPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

		DefaultUIFlatColorMaterial = std::make_shared<D3D12Material>("Flat", EMaterialBlendMode::Opaque, pPipelineDesc);

		pPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Pixel, 1));
		pPipelineDesc.ShaderAttachments.clear();
		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBaseVS.data(), WidgetBaseVS.length(), "GeometryVS", eShaderType::Vertex));
		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBasePS_Gradient.data(), WidgetBasePS_Gradient.length(), "GradientPS", eShaderType::Pixel));
		DefaultStartScreenMaterial = std::make_shared<D3D12Material>("StartScreen", EMaterialBlendMode::Opaque, pPipelineDesc);

		//pPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		//DefaultUIFlatColorMaterial_DepthDisable = std::make_shared<D3D12Material>("Flat", EMaterialBlendMode::Opaque, pPipelineDesc);

		pPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		pPipelineDesc.DescriptorSetLayout.clear();
		pPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Vertex, 0));
		pPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eTexture, eShaderType::Pixel, 0));

		//pPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		//DefaultUITexturedMaterial_DepthDisable = std::make_shared<D3D12Material>("Textured", EMaterialBlendMode::Opaque, pPipelineDesc);

		//pPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		pPipelineDesc.DescriptorSetLayout.clear();
		pPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Vertex, 0));

		pPipelineDesc.BlendAttribute = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		pPipelineDesc.RasterizerAttribute.AntialiasedLineEnable = true;
		pPipelineDesc.DepthStencilAttribute = CD3DX12_DEPTH_STENCIL_DESC1(D3D12_DEFAULT); // enable Greater ?
		pPipelineDesc.ShaderAttachments.clear();
		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBaseVS.data(), WidgetBaseVS.length(), "GeometryVS", eShaderType::Vertex));
		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBasePS_Flat.data(), WidgetBasePS_Flat.length(), "WidgetFlatColorPS", eShaderType::Pixel));
		DefaultUILineMaterial = std::make_shared<D3D12Material>("Line", EMaterialBlendMode::Opaque, pPipelineDesc);

		{
			D3D12Material::D3D12MaterialInstance* LineMatInstance = DefaultUILineMaterial->CreateInstance("LineInstance");
			DefaultLineColorMatStyle = UIMaterialStyle::Create("Line", UIMaterial_D3D12::Create(LineMatInstance));
		}

		{
			D3D12PipelineDesc FontPipelineDesc;
			FontPipelineDesc.BlendAttribute = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			for (std::size_t i = 0; i < 8; i++)
			{
				FontPipelineDesc.BlendAttribute.RenderTarget[i].BlendEnable = true;
				FontPipelineDesc.BlendAttribute.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				FontPipelineDesc.BlendAttribute.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				FontPipelineDesc.BlendAttribute.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_SRC_ALPHA;
			}
			FontPipelineDesc.BlendAttribute.IndependentBlendEnable = true;

			FontPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			FontPipelineDesc.RasterizerAttribute = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			FontPipelineDesc.RasterizerAttribute.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;

			FontPipelineDesc.DepthStencilAttribute = CD3DX12_DEPTH_STENCIL_DESC1(D3D12_DEFAULT);
			FontPipelineDesc.DepthStencilAttribute.DepthEnable = false;
			FontPipelineDesc.DepthStencilAttribute.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			FontPipelineDesc.DepthStencilAttribute.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			FontPipelineDesc.DepthStencilAttribute.StencilEnable = true;

			FontPipelineDesc.DepthStencilAttribute.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			FontPipelineDesc.DepthStencilAttribute.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			FontPipelineDesc.DepthStencilAttribute.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			FontPipelineDesc.DepthStencilAttribute.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

			FontPipelineDesc.DepthStencilAttribute.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			FontPipelineDesc.DepthStencilAttribute.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			FontPipelineDesc.DepthStencilAttribute.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			FontPipelineDesc.DepthStencilAttribute.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

			std::vector<VertexAttributeDesc> VertexLayout =
			{
				{ "POSITION",	DXGI_FORMAT_R32G32B32A32_FLOAT,  0,	offsetof(cbgui::cbGeometryVertexData, position),   false },
				{ "TEXCOORD",	DXGI_FORMAT_R32G32_FLOAT,   0,	offsetof(cbgui::cbGeometryVertexData, texCoord),   false },
				{ "COLOR",		DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	offsetof(cbgui::cbGeometryVertexData, Color),	  false },
			};
			FontPipelineDesc.VertexLayout = VertexLayout;

			FontPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Vertex, 0));
			FontPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eTexture, eShaderType::Pixel, 0));
			FontPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eSampler_Point, eShaderType::Pixel, 0));

			FontPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			FontPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

			FontPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBaseVS.data(), WidgetBaseVS.length(), "GeometryVS", eShaderType::Vertex));
			FontPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)WidgetBasePS_Font.data(), WidgetBasePS_Font.length(), "FontPS", eShaderType::Pixel));

			DefaultUIFontMaterial = std::make_shared<D3D12Material>("Font_Mat", EMaterialBlendMode::Opaque, FontPipelineDesc);

			//FontPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			//DefaultUIFontMaterial_DepthDisable = std::make_shared<D3D12Material>("Font_Mat", EMaterialBlendMode::Opaque, FontPipelineDesc);

			{
				cbgui::cbFontDesc FontDesc("DejaVu Sans");
				FontDesc.FontSize = 24;
				//FontDesc.DefaultSpaceWidth = 3;
				FontDesc.DefaultFontLocation = "c:/Windows/Fonts/";
				FontDesc.Fonts.insert({ eFontType::Regular, cbFontDesc::cbFontLoadDesc("DejaVuSans.ttf") });
				FontDesc.Fonts.insert({ eFontType::Bold, cbFontDesc::cbFontLoadDesc("DejaVuSans-Bold.ttf") });
				FontDesc.Fonts.insert({ eFontType::BoldItalic, cbFontDesc::cbFontLoadDesc("DejaVuSans-BoldOblique.ttf") });
				FontDesc.Fonts.insert({ eFontType::Italic, cbFontDesc::cbFontLoadDesc("DejaVuSans-Oblique.ttf") });
				FontDesc.Fonts.insert({ eFontType::Light, cbFontDesc::cbFontLoadDesc("DejaVuSans-ExtraLight.ttf") });
				//FontDesc.AtlasHeight = 4096;
				//FontDesc.AtlasWidth = 4096;
				//FontDesc.Numchars = 256;
				//FontDesc.LightItalicFontLocation = "..//Content//";
				//FontDesc.SDF = true;
				FontDesc.fFontTextureUpdate_Callback = [&](const void* Texture, std::size_t RowPitch, std::size_t MinX, std::size_t MinY, std::size_t MaxX, std::size_t MaxY)
				{
					DefaultUIFontMaterial->GetInstance(std::string("Font_Mat") + "_Instance")->UpdateTexture(0, Texture, RowPitch, MinX, MinY, MaxX, MaxY);
				};
				cbFontResources::Get().AddFreeTypeFont(FontDesc);

				auto pFont = cbFontResources::Get().GetFontFamily("DejaVu Sans");
				auto Atlas = pFont->GetTexture();

				TextureDesc Desc;
				Desc.Dimensions.X = pFont->GetDesc().AtlasWidth;
				Desc.Dimensions.Y = pFont->GetDesc().AtlasHeight;
				Desc.MipLevels = 1;
				Desc.Format = /*DXGI_FORMAT_R8_SNORM//*/ DXGI_FORMAT_R8_UNORM;
				D3D12Material::D3D12MaterialInstance* FontMatInstance = DefaultUIFontMaterial->CreateInstance(std::string("Font_Mat") + "_Instance");
				FontMatInstance->AddTexture("Font_Mat", 0, &Atlas[0], 0, Desc);

				FontStyle = UIFontMaterialStyle::Create("Text", "DejaVu Sans", UIMaterial_D3D12::Create(FontMatInstance));
			}
		}

		{
			D3D12Material::D3D12MaterialInstance* FlatColorInstance = DefaultUIFlatColorMaterial->CreateInstance("UIFlatColorInstance");
			DefaultFlatColorMatStyle = UIMaterialStyle::Create("Image", UIMaterial_D3D12::Create(FlatColorInstance));
		}

		{
			D3D12Material::D3D12MaterialInstance* StartScreenWaveInstance = DefaultStartScreenMaterial->CreateInstance("StartScreenInstance");
			StartScreenWave = UIMaterialStyle::Create("StartScreen", UIMaterial_D3D12::Create(StartScreenWaveInstance));
		}
	}

	{
		D3D12PipelineDesc pPipelineDesc;
		pPipelineDesc.BlendAttribute = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		pPipelineDesc.DepthStencilAttribute.DepthEnable = false;
		pPipelineDesc.DepthStencilAttribute.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pPipelineDesc.DepthStencilAttribute.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		pPipelineDesc.DepthStencilAttribute.StencilEnable = true;

		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		pPipelineDesc.DepthStencilAttribute.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		pPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pPipelineDesc.RasterizerAttribute = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		std::vector<VertexAttributeDesc> VertexLayout =
		{
			{ "POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, false },
		};
		pPipelineDesc.VertexLayout = VertexLayout;

		pPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Vertex, 0));

		pPipelineDesc.NumRenderTargets = 0;
		pPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)DepthTestVSShader.data(), DepthTestVSShader.length(), "mainVS", eShaderType::Vertex));
		//pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment(L"DepthStencilTest_Shader.hlsl", "mainVS", eShaderType::Vertex));

		DepthMainPipeline = std::make_unique<D3D12Pipeline>(pPipelineDesc);
	}

	{
		D3D12PipelineDesc pPipelineDesc;
		pPipelineDesc.BlendAttribute = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		pPipelineDesc.DepthStencilAttribute.DepthEnable = false;
		pPipelineDesc.DepthStencilAttribute.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pPipelineDesc.DepthStencilAttribute.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		pPipelineDesc.DepthStencilAttribute.StencilEnable = true;

		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR_SAT;
		pPipelineDesc.DepthStencilAttribute.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		pPipelineDesc.DepthStencilAttribute.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR_SAT;
		pPipelineDesc.DepthStencilAttribute.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		pPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pPipelineDesc.RasterizerAttribute = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		std::vector<VertexAttributeDesc> VertexLayout =
		{
			{ "POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, false },
		};
		pPipelineDesc.VertexLayout = VertexLayout;

		pPipelineDesc.DescriptorSetLayout.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Vertex, 0));

		pPipelineDesc.NumRenderTargets = 0;
		pPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

		pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment((void*)DepthTestVSShader.data(), DepthTestVSShader.length(), "mainVS", eShaderType::Vertex));
		//pPipelineDesc.ShaderAttachments.push_back(ShaderAttachment(L"DepthStencilTest_Shader.hlsl", "mainVS", eShaderType::Vertex));

		DepthPipeline = std::make_unique<D3D12Pipeline>(pPipelineDesc);
	}

	{
		std::vector<DescriptorSetLayoutBinding> Bindings;
		Bindings.push_back(DescriptorSetLayoutBinding(EDescriptorType::eUniformBuffer, eShaderType::Vertex, 0));
		RootSignature = std::make_unique<D3D12RootSignature>(CMD->GetDevice(), Bindings);
	}

	{
		sBufferDesc Desc;
		Desc.Size = sizeof(cbVector4) * 4;
		Desc.Stride = sizeof(cbVector4);
		DepthVertexBuffer = std::make_unique<D3D12VertexBuffer>(Desc, nullptr);
	}
	{
		sBufferDesc Desc;
		Desc.Size = sizeof(cbVector4) * 4;
		Desc.Stride = sizeof(cbVector4);
		DepthFullScreenVertexBuffer = std::make_unique<D3D12VertexBuffer>(Desc, nullptr);
	}

	CMD->WaitForGPU();
}

Renderer_DX12::~Renderer_DX12()
{
	Owner = nullptr;
	CMD = nullptr;
	RootSignature = nullptr;
	DepthMainPipeline = nullptr;
	DepthPipeline = nullptr;
	DepthVertexBuffer = nullptr;
	DepthFullScreenVertexBuffer = nullptr;
	DepthFBO = nullptr;
	WidgetConstantBuffer = nullptr;

	LastMaterial = nullptr;
	DefaultUIFontMaterial = nullptr;
	DefaultUILineMaterial = nullptr;
	DefaultUIFlatColorMaterial = nullptr;
	DefaultStartScreenMaterial = nullptr;

	FontStyle = nullptr;
	DefaultFlatColorMatStyle = nullptr;
	DefaultLineColorMatStyle = nullptr;
	StartScreenWave = nullptr;

	GradientConstantBuffer = nullptr;
}

void Renderer_DX12::BeginPlay()
{

}

void Renderer_DX12::Tick(const float DeltaTime)
{

}

void Renderer_DX12::ResizeWindow(std::size_t Width, std::size_t Height)
{
	DepthFBO = nullptr;
	std::vector<FrameBufferAttachmentInfo> Infos;
	{
		FrameBufferAttachmentInfo Info;
		Info.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
		Info.Usage = eImageUsage::Depth;
		Infos.push_back(Info);
	}

	FBODesc Desc;
	Desc.Dimensions.X = (uint32_t)Width;
	Desc.Dimensions.Y = (uint32_t)Height;
	CMD->WaitForGPU();
	DepthFBO = std::make_unique<D3D12FrameBuffer>("DepthFBO", Infos, Desc);
	CMD->WaitForGPU();
	{
		OnScreenWidgetMatrixDX12 WidgetMatrix;
		WidgetMatrix.Matrix = (DirectX::XMMATRIX&)GetViewportTransform((int)Width, (int)Height);
		WidgetConstantBuffer->Map(&WidgetMatrix);
	}
}

void Renderer_DX12::InputProcess(HWND hWnd, std::uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg)
	{
		switch (uMsg)
		{
		case WM_SYSKEYUP:
			break;
		case WM_SYSKEYDOWN:
			break;
		case WM_KEYDOWN:
		{
			int iKeyPressed = static_cast<int>(wParam);

			if (iKeyPressed == 39)
			{
				GradientIndex++;
				if (GradientIndex > 15)
					GradientIndex = 0;
				CBGradientIdx GradientIdx;
				GradientIdx.GradientIdx = GradientIndex;
				GradientConstantBuffer->Map(&GradientIdx);
			}
			if (iKeyPressed == 37)
			{
				if (GradientIndex != 0)
					GradientIndex--;
				else
					GradientIndex = 15;
				CBGradientIdx GradientIdx;
				GradientIdx.GradientIdx = GradientIndex;
				GradientConstantBuffer->Map(&GradientIdx);
			}
			if (iKeyPressed == 38)
			{
				bShowLines = true;
			}
			if (iKeyPressed == 40)
			{
				bShowLines = false;
			}
			if (iKeyPressed == 109)
			{
				bEnableStencilClipping = false;
			}
			if (iKeyPressed == 107)
			{
				bEnableStencilClipping = true;
			}
		}
		}
	}
}

IUIMaterialStyle* Renderer_DX12::GetMaterialStyle(std::string Name) const
{
	if (Name == FontStyle->GetName())
		return FontStyle.get();
	else if (Name == DefaultLineColorMatStyle->GetName())
		return DefaultLineColorMatStyle.get();
	else if (Name == StartScreenWave->GetName())
		return StartScreenWave.get();
	else if (Name == DefaultFlatColorMatStyle->GetName())
		return DefaultFlatColorMatStyle.get();

	return DefaultFlatColorMatStyle.get();
}

bool Renderer_DX12::DepthPass(cbgui::cbWidgetObj* Widget)
{
	if (Widget->HasOwner())
		return false;

	LastMaterial = nullptr;

	//CMD->ClearDepthFBO(DepthFBO.get());

	//auto DSVCPU = DepthFBO->GetDSVCPUDescHandle();
	//CMD->Get()->OMSetRenderTargets(0, nullptr, FALSE, &DSVCPU);

	CMD->SetGraphicsRootSignature(RootSignature->Get());

	{
		CMD->SetStencilRef(1);

		std::vector<cbVector4> Data;
		Data.push_back(cbVector4::Zero());
		Data.push_back(cbVector4(cbVector((float)Owner->GetWindowWidth(), 0), cbVector(0.0f, 1.0f)));
		Data.push_back(cbVector4(cbVector(0, (float)Owner->GetWindowHeight()), cbVector(0.0f, 1.0f)));
		Data.push_back(cbVector4(cbVector((float)Owner->GetWindowWidth(), (float)Owner->GetWindowHeight()), cbVector(0.0f, 1.0f)));

		sBufferSubresource Subresource;
		Subresource.pSysMem = Data.data();
		Subresource.Size = Data.size() * sizeof(cbVector4);
		Subresource.Location = 0;
		DepthFullScreenVertexBuffer->UpdateSubresource(CMD->Get(), &Subresource);

		DepthFullScreenVertexBuffer->ApplyBuffer(CMD->Get());

		CMD->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		WidgetConstantBuffer->ApplyBuffer(0, CMD->Get());
		CMD->SetScissorRect(0, 0, Owner->GetWindowHeight(), Owner->GetWindowWidth());
		DepthMainPipeline->ApplyPipeline(CMD.get());
		CMD->Draw(4);
	}
	{
		CMD->SetStencilRef(0);

		const auto Bounds = Widget->GetBounds();
		std::vector<cbVector4> Data;
		Data.push_back(cbVector4(Bounds.GetCorner(0), cbVector(0.0f, 1.0f)));
		Data.push_back(cbVector4(Bounds.GetCorner(1), cbVector(0.0f, 1.0f)));
		Data.push_back(cbVector4(Bounds.GetCorner(3), cbVector(0.0f, 1.0f)));
		Data.push_back(cbVector4(Bounds.GetCorner(2), cbVector(0.0f, 1.0f)));
		for (auto& pData : Data)
			pData = cbgui::RotateVectorAroundPoint(pData, Bounds.GetCenter(), Widget->GetRotation());

		sBufferSubresource Subresource;
		Subresource.pSysMem = Data.data();
		Subresource.Size = Data.size() * sizeof(cbVector4);
		Subresource.Location = 0;
		DepthVertexBuffer->UpdateSubresource(CMD->Get(), &Subresource);

		DepthVertexBuffer->ApplyBuffer(CMD->Get());

		CMD->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		WidgetConstantBuffer->ApplyBuffer(0, CMD->Get());
		CMD->SetScissorRect(0, 0, Owner->GetWindowHeight(), Owner->GetWindowWidth());
		DepthPipeline->ApplyPipeline(CMD.get());
		CMD->Draw(4);
	}

	CMD->FinishRecordCommandList();
	CMD->ExecuteCommandList();

	return true;
}

void Renderer_DX12::Render()
{
	std::vector<ICanvas::WidgetHierarchy*> DrawLatest;
	std::vector<ICanvas::WidgetHierarchy*> LastInTheHierarchy;

	std::function<void(ICanvas::WidgetHierarchy*, std::optional<cbgui::cbIntBounds>, const eZOrderMode&, AVertexBuffer*, AIndexBuffer*)> fDraw;
	fDraw = [&](ICanvas::WidgetHierarchy* Node, std::optional<cbgui::cbIntBounds> ScissorsRect, const eZOrderMode& Mode, AVertexBuffer* VertexBuffer, AIndexBuffer* IndexBuffer) -> void
	{
		if (!Node->Widget->IsVisible())
			return;

		//const cbgui::cbIntBounds& Bounds(ScissorsRect.has_value() ? *ScissorsRect : Node->Widget->GetBounds());

		Draw(Node, Node->Widget->GetCulledBounds(),/*Bounds,*/ VertexBuffer, IndexBuffer);

		for (const auto& pChild : Node->Nodes)
		{
			if (!pChild->Widget->IsVisible())
			{
				continue;
			}
			if (Mode == eZOrderMode::InOrder && pChild->Widget->GetZOrderMode() == eZOrderMode::Latest)
			{
				DrawLatest.push_back(pChild);
				continue;
			}
			else if (Mode == eZOrderMode::InOrder && pChild->Widget->GetZOrderMode() == eZOrderMode::LastInTheHierarchy)
			{
				LastInTheHierarchy.push_back(pChild);
				continue;
			}
			fDraw(pChild, std::nullopt,/*cbIntBounds(pChild->Widget->GetBounds()).Crop(Bounds),*/ Mode, VertexBuffer, IndexBuffer);
		}
	};

	{
		CMD->BeginRecordCommandList();

		CMD->SetHeaps();

		if (bEnableStencilClipping)
			CMD->ClearDepthFBO(DepthFBO.get());

		CMD->ClearBackBuffer();
		CMD->SetBackBufferAsRenderTarget(DepthFBO.get());

		D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)Owner->GetWindowWidth(),  (float)Owner->GetWindowHeight(), 0.0f, 1.0f};
		CMD->Get()->RSSetViewports(1, &viewport);

		CMD->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const std::vector<ICanvas*>& Canvases = Owner->GetCanvases();
		for (const auto& Canvas : Canvases)
		{
			auto VertexBuffer = Canvas->GetVertexBuffer();
			auto IndexBuffer = Canvas->GetIndexBuffer();
			CMD->SetVertexBuffer(VertexBuffer);
			CMD->SetIndexBuffer(IndexBuffer);

			{
				const std::vector<ICanvas::WidgetHierarchy*>& Widgets = Canvas->GetWidgetHierarchy();
				for (const auto& Widget : Widgets)
				{
					if (!Widget)
						continue;

					if (!Widget->Widget->IsVisible())
						continue;

					if (Widget->Widget->GetZOrderMode() == eZOrderMode::Latest)
					{
						DrawLatest.push_back(Widget);
						continue;
					}
					fDraw(Widget, std::nullopt, eZOrderMode::InOrder, VertexBuffer, IndexBuffer);

					if (LastInTheHierarchy.size() > 0)
					{
						for (auto& pWidget : LastInTheHierarchy)
						{
							fDraw(pWidget, std::nullopt /*pWidget->Widget->GetCulledBounds()*/, eZOrderMode::LastInTheHierarchy, VertexBuffer, IndexBuffer);
						}
						LastInTheHierarchy.clear();
					}
					if (bEnableStencilClipping)
					{
						CMD->ClearDepthFBO(DepthFBO.get());
						CMD->SetStencilRef(0);
						CMD->SetBackBufferAsRenderTarget(DepthFBO.get());
						//bEnableStencilClipping = false;
					}
				}
			}

			if (DrawLatest.size() > 0)
			{
				for (auto& Widget : DrawLatest)
				{
					fDraw(Widget, std::nullopt /*Widget->Widget->GetCulledBounds()*/, eZOrderMode::Latest, VertexBuffer, IndexBuffer);
				}
				DrawLatest.clear();
			}
		}

		CMD->FinishRecordCommandList();
		CMD->ExecuteCommandList();
	}
	LastMaterial = nullptr;
}

void Renderer_DX12::Draw(ICanvas::WidgetHierarchy* Node, const cbgui::cbIntBounds& ScissorsRect, AVertexBuffer* VertexBuffer, AIndexBuffer* IndexBuffer)
{
	if (Node->Widget->HasGeometry())
	{
		if (!Node->MaterialName.has_value())
			return;
		if (ScissorsRect.IsValid())
		{
			CMD->SetScissorRect((std::uint32_t)ScissorsRect.GetTop(), (std::uint32_t)ScissorsRect.GetLeft(), (std::uint32_t)ScissorsRect.GetBottom(), (std::uint32_t)ScissorsRect.GetRight());
		}
		else
		{
			return;
		}

		const auto& GeometryDrawData = Node->Widget->GetGeometryDrawData();
		const auto& pMat = GetMaterialStyle(Node->MaterialName.value());
		const auto pMaterialInstace = static_cast<UIMaterial_D3D12*>(pMat->GetMaterial(GeometryDrawData.StyleState))->Material;
		D3D12Material* pMaterial = pMaterialInstace->GetParent();
		if (LastMaterial != pMaterial)
		{
			LastMaterial = pMaterial;
			LastMaterial->ApplyMaterial(CMD.get());
			WidgetConstantBuffer->ApplyBuffer(0, CMD->Get());
			if (Node->MaterialName == StartScreenWave->GetName())
				GradientConstantBuffer->ApplyBuffer(1, CMD->Get());
		}
		pMaterialInstace->ApplyMaterialInstance(CMD.get());

		if (Node->bVertexDirty || Node->bIndexDirty)
		{
			if (Node->bVertexDirty)
			{
				auto Data = Node->Widget->GetVertexData();
				if (Data.size() > 0)
				{
					sBufferSubresource Subresource;
					Subresource.pSysMem = Data.data();
					Subresource.Size = Data.size() * sizeof(cbGeometryVertexData);
					Subresource.Location = Node->DrawParams.VertexOffset * sizeof(cbGeometryVertexData);
					CMD->UpdateBufferSubresource(VertexBuffer, &Subresource);
				}
				Node->bVertexDirty = false;
			}
			if (Node->bIndexDirty)
			{
				auto Indices = Node->Widget->GetIndexData();
				if (Indices.size() > 0)
				{
					sBufferSubresource Subresource;
					Subresource.pSysMem = Indices.data();
					Subresource.Size = GeometryDrawData.IndexCount * sizeof(std::uint32_t);
					Subresource.Location = Node->DrawParams.IndexOffset * sizeof(std::uint32_t);
					CMD->UpdateBufferSubresource(IndexBuffer, &Subresource);
				}
				Node->bIndexDirty = false;
			}
		}
		CMD->DrawIndexedInstanced((std::uint32_t)GeometryDrawData.DrawCount, 1, (std::uint32_t)Node->DrawParams.IndexOffset, (std::uint32_t)Node->DrawParams.VertexOffset, 0);
	}
	else
	{
		if (bEnableStencilClipping)
		{
			if (Node->Widget->GetRotation() != 0.0f && Node->Widget->HasAnyChildren())
			{
				if (DepthPass(Node->Widget))
				{
					CMD->BeginRecordCommandList();
					CMD->SetHeaps();
					D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)Owner->GetWindowWidth(),  (float)Owner->GetWindowHeight(), 0.0f, 1.0f};
					CMD->Get()->RSSetViewports(1, &viewport);
					CMD->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					CMD->SetVertexBuffer(VertexBuffer);
					CMD->SetIndexBuffer(IndexBuffer);

					if (LastMaterial)
						LastMaterial->ApplyMaterial(CMD.get());

					CMD->SetBackBufferAsRenderTarget(DepthFBO.get());
					CMD->SetStencilRef(2);
				}
			}
		}

		if (!bShowLines)
			return;

		if (ScissorsRect.IsValid())
		{
			CMD->SetScissorRect((std::uint32_t)ScissorsRect.GetTop(), (std::uint32_t)ScissorsRect.GetLeft(), (std::uint32_t)ScissorsRect.GetBottom(), (std::uint32_t)ScissorsRect.GetRight());
		}
		else
		{
			return;
		}

		const auto& GeometryDrawData = Node->Widget->GetGeometryDrawData();
		const auto pMaterialInstace = static_cast<UIMaterial_D3D12*>(DefaultLineColorMatStyle->GetMaterial(GeometryDrawData.StyleState))->Material;
		D3D12Material* pMaterial = pMaterialInstace->GetParent();
		if (LastMaterial != pMaterial)
		{
			LastMaterial = pMaterial;
			LastMaterial->ApplyMaterial(CMD.get());
			WidgetConstantBuffer->ApplyBuffer(0, CMD->Get());
			if (Node->MaterialName == StartScreenWave->GetName())
				GradientConstantBuffer->ApplyBuffer(1, CMD->Get());
		}
		pMaterialInstace->ApplyMaterialInstance(CMD.get());

		if (Node->bVertexDirty || Node->bIndexDirty)
		{
			if (Node->bVertexDirty)
			{
				auto Data = Node->Widget->GetVertexData();
				if (Data.size() > 0)
				{
					sBufferSubresource Subresource;
					Subresource.pSysMem = Data.data();
					Subresource.Size = Data.size() * sizeof(cbGeometryVertexData);
					Subresource.Location = Node->DrawParams.VertexOffset * sizeof(cbGeometryVertexData);
					CMD->UpdateBufferSubresource(VertexBuffer, &Subresource);
				}
				Node->bVertexDirty = false;
			}
			if (Node->bIndexDirty)
			{
				auto Indices = Node->Widget->GetIndexData();
				if (Indices.size() > 0)
				{
					sBufferSubresource Subresource;
					Subresource.pSysMem = Indices.data();
					Subresource.Size = GeometryDrawData.IndexCount * sizeof(std::uint32_t);
					Subresource.Location = Node->DrawParams.IndexOffset * sizeof(std::uint32_t);
					CMD->UpdateBufferSubresource(IndexBuffer, &Subresource);
				}
				Node->bIndexDirty = false;
			}
		}
		CMD->DrawIndexedInstanced((std::uint32_t)GeometryDrawData.DrawCount, 1, (std::uint32_t)Node->DrawParams.IndexOffset, (std::uint32_t)Node->DrawParams.VertexOffset, 0);
	}
}
