#include "stdafx.h"

bool ba::CreateTexRTVAndSRV(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format, ID3D11RenderTargetView** out_rtv, ID3D11ShaderResourceView** out_srv)
{
	// Create a texture with the initial data.
	//
	D3D11_TEXTURE2D_DESC tex_desc{};
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = format;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* tex = nullptr;
	HRESULT res = device->CreateTexture2D(&tex_desc, nullptr, &tex);
	if (FAILED(res))
		return false;
	//__

	// Create rtv for the texture.
	//
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
	rtv_desc.Format = tex_desc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView* rtv = nullptr;
	device->CreateRenderTargetView(tex, &rtv_desc, &rtv);
	if (FAILED(res))
	{
		ReleaseCOM(tex);
		return false;
	}
	//__

	// Create srv for the texture.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = tex_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = tex_desc.MipLevels;

	ID3D11ShaderResourceView* srv = nullptr;
	res = device->CreateShaderResourceView(tex, &srv_desc, &srv);
	ReleaseCOM(tex);
	if (FAILED(res))
	{
		ReleaseCOM(rtv);
		return false;
	}
	//__

	*out_rtv = rtv;
	*out_srv = srv;

	return true;
}
