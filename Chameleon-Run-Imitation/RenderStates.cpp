#include "stdafx.h"

ID3D11BlendState* ba::renderstates::blend::kBSOver = nullptr;
ID3D11BlendState* ba::renderstates::blend::kBSTranslucent = nullptr;
ID3D11BlendState* ba::renderstates::blend::kBSAdditive = nullptr;
ID3D11BlendState* ba::renderstates::blend::kBSModulate = nullptr;
ID3D11BlendState* ba::renderstates::blend::kBSModulate2 = nullptr;

bool ba::renderstates::rasterizer::Init(ID3D11Device* device)
{
	// Do nothing now.
	return true;
}

void ba::renderstates::rasterizer::Release()
{
	// Do nothing now.
}

bool ba::renderstates::depthstencil::Init(ID3D11Device* device)
{
	// Do nothing now.
	return true;
}

void ba::renderstates::depthstencil::Release()
{
	// Do nothing now.
}

bool ba::renderstates::blend::Init(ID3D11Device* device)
{
	// The source texture is opaque.
	//
	D3D11_BLEND_DESC blend_desc{};
	blend_desc.RenderTarget[0].BlendEnable = true;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT res = device->CreateBlendState(&blend_desc, &kBSOver);
	if (FAILED(res))
		return false;
	//__

	// The source texture is transparent depanding on the alpha value.
	//
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	HRESULT res = device->CreateBlendState(&blend_desc, &kBSTranslucent);
	if (FAILED(res))
	{
		Release();
		return false;
	}
	//__

	// The source color is added to the destination color.
	//
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	HRESULT res = device->CreateBlendState(&blend_desc, &kBSAdditive);
	if (FAILED(res))
	{
		Release();
		return false;
	}
	//__

	// The destination color is multiplied by the source color.
	//  (src * dest + dest * 0)
	//
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	HRESULT res = device->CreateBlendState(&blend_desc, &kBSModulate);
	if (FAILED(res))
	{
		Release();
		return false;
	}
	//__

	// The destination color is multiplied by the source color and by two.
	//  (src * dest + dest * src == 2 * src * dest)
	//
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	HRESULT res = device->CreateBlendState(&blend_desc, &kBSModulate2);
	if (FAILED(res))
	{
		Release();
		return false;
	}
	//__

	return true;
}

void ba::renderstates::blend::Release()
{
	ReleaseCOM(kBSOver);
	ReleaseCOM(kBSTranslucent);
	ReleaseCOM(kBSAdditive);
	ReleaseCOM(kBSModulate);
	ReleaseCOM(kBSModulate2);
}

bool ba::renderstates::InitAll(ID3D11Device* device)
{
	if (!rasterizer::Init(device)) { return false; }
	if (!depthstencil::Init(device)) { ReleaseAll(); return false; }
	if (!blend::Init(device)) { ReleaseAll(); return false; }

	return true;
}

void ba::renderstates::ReleaseAll()
{
	rasterizer::Release();
	depthstencil::Release();
	blend::Release();
}
