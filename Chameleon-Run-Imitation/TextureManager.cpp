#include "stdafx.h"

namespace ba
{
	//
	// Helpers
	//

	void DestroyTextures(std::vector<ID3D11Texture2D*>& textures)
	{
		for (UINT i = 0; i < textures.size(); ++i)
		{
			DestroyCOM(textures[i]);
		}
		textures.clear();
	}

	//
	// TextureManager Class
	//

	ba::TextureManager::TextureManager() :
		device_(nullptr),
		dc_(nullptr)
	{
	}

	ba::TextureManager& ba::TextureManager::GetInstance()
	{
		static TextureManager instance;
		return instance;
	}

	void ba::TextureManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
	{
		device_ = device;
		dc_ = dc;
	}

	void ba::TextureManager::Destroy()
	{
		device_ = nullptr;
	}

	bool ba::TextureManager::CreateTex2DSRV(const std::wstring& file_name, ID3D11ShaderResourceView** out_srv)
	{
		if (!device_)
			return false;

		std::wstring key_string = L"Tex2D" + file_name;

		if (srvs_.find(key_string) == srvs_.end())
		{
			HRESULT res = D3DX11CreateShaderResourceViewFromFile(
				device_, key_string.c_str(), nullptr, nullptr,
				out_srv, nullptr
			);
			if (FAILED(res))
				return false;

			srvs_[key_string] = *out_srv;
		}
		else
		{
			*out_srv = srvs_[key_string];
		}
		return true;
	}

	bool ba::TextureManager::CreateTex2DArrSRV(
		const std::vector<std::wstring>& file_names, ID3D11ShaderResourceView** out_srv,
		DXGI_FORMAT format,
		UINT filter, UINT mip_filter
	)
	{
		if (!device_ || !dc_)
			return false;
		
		// Create a key value for the srv to be created.
		std::wstring key_string = L"Tex2DArr" + file_names.front() + file_names.back()
			+ std::to_wstring(static_cast<int>(format)) + std::to_wstring(filter) + std::to_wstring(mip_filter);

		if (srvs_.find(key_string) != srvs_.end())
		{
			*out_srv = srvs_[key_string];
			return true;
		}

		// Create textures from the input image files.
		//
		UINT arr_size = file_names.size();
		std::vector<ID3D11Texture2D*> textures(arr_size, nullptr);

		D3DX11_IMAGE_LOAD_INFO load_info{};
		load_info.Width = D3DX11_FROM_FILE;
		load_info.Height = D3DX11_FROM_FILE;
		load_info.Depth = D3DX11_FROM_FILE;
		load_info.FirstMipLevel = 0;
		load_info.MipLevels = D3DX11_FROM_FILE;
		load_info.Usage = D3D11_USAGE_STAGING;
		load_info.BindFlags = 0;
		load_info.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		load_info.MiscFlags = 0;
		load_info.Format = format;
		load_info.Filter = filter;
		load_info.MipFilter = mip_filter;
		load_info.pSrcInfo = nullptr;

		HRESULT res;
		for (UINT i = 0; i < arr_size; ++i)
		{
			res = D3DX11CreateTextureFromFile(
				device_, file_names[i].c_str(), &load_info, nullptr,
				reinterpret_cast<ID3D11Resource**>(&textures[i]), nullptr
			);
			if (FAILED(res))
			{
				DestroyTextures(textures);
				return false;
			}
		}
		//__

		// Create empty texture array.
		//
		D3D11_TEXTURE2D_DESC tex_desc;
		textures[0]->GetDesc(&tex_desc);

		D3D11_TEXTURE2D_DESC tex_arr_desc{};
		tex_arr_desc.Width = tex_desc.Width;
		tex_arr_desc.Height = tex_desc.Height;
		tex_arr_desc.MipLevels = tex_desc.MipLevels;
		tex_arr_desc.ArraySize = arr_size;
		tex_arr_desc.Format = tex_desc.Format;
		tex_arr_desc.SampleDesc = { 1,0 };
		tex_arr_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_arr_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		ID3D11Texture2D* tex_arr = nullptr;
		res = device_->CreateTexture2D(&tex_arr_desc, nullptr, &tex_arr);
		if (FAILED(res))
		{
			DestroyTextures(textures);
			return false;
		}
		//

		// Copy loaded textures' data into the empty texture array.
		//
		D3D11_MAPPED_SUBRESOURCE mapped_data;
		for (UINT tex_idx = 0; tex_idx < arr_size; ++tex_idx)
		{
			for (UINT mip_level = 0; mip_level < tex_desc.MipLevels; ++mip_level)
			{
				// Get the texture data.
				res = dc_->Map(textures[tex_idx], mip_level, D3D11_MAP_READ, 0, &mapped_data);
				if (FAILED(res))
				{
					dc_->Unmap(textures[tex_idx], mip_level);
					DestroyTextures(textures);
					DestroyCOM(tex_arr);
					return false;
				}

				// Store the data into the element of the texture array.
				dc_->UpdateSubresource(
					tex_arr, D3D11CalcSubresource(mip_level, tex_idx, tex_desc.MipLevels), nullptr,
					mapped_data.pData, mapped_data.RowPitch, mapped_data.DepthPitch
				);

				dc_->Unmap(textures[tex_idx], mip_level);
			}
		}
		//__

		// Create a srv for the texture array.
		//
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
		srv_desc.Format = tex_arr_desc.Format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srv_desc.Texture2DArray.MostDetailedMip = 0;
		srv_desc.Texture2DArray.MipLevels = tex_arr_desc.MipLevels;
		srv_desc.Texture2DArray.FirstArraySlice = 0;
		srv_desc.Texture2DArray.ArraySize = tex_arr_desc.ArraySize;

		res = device_->CreateShaderResourceView(tex_arr, &srv_desc, out_srv);

		//  Release references to the loaded textures and texture array instances.
		DestroyTextures(textures);
		DestroyCOM(tex_arr);

		if (FAILED(res))
			return false;
		//__

		// Store the srv into the map 'srvs_'.
		srvs_[key_string] = *out_srv;

		return true;
	}

	bool TextureManager::CreateRandomTex1DSRV(UINT size, ID3D11ShaderResourceView** out_srv)
	{
		if (!device_ || !dc_)
			return false;

		std::wstring key_string = L"Tex1D" + std::to_wstring(size);

		if (srvs_.find(key_string) != srvs_.end())
		{
			*out_srv = srvs_[key_string];
			return true;
		}

		// Create 4D random vectors. Each component has a value within th range of -1 to 1.
		std::vector<XMFLOAT4> values(size);
		for (UINT i = 0; i < values.size(); ++i)
		{
			values[i].x = mathhelper::RandF(-1.0f, 1.0f);
			values[i].y = mathhelper::RandF(-1.0f, 1.0f);
			values[i].z = mathhelper::RandF(-1.0f, 1.0f);
			values[i].w = mathhelper::RandF(-1.0f, 1.0f);
		}

		D3D11_TEXTURE1D_DESC tex_desc{};
		tex_desc.Width = values.size();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA tex_init{};
		tex_init.pSysMem = &values[0];

		ID3D11Texture1D* tex;
		HRESULT res = device_->CreateTexture1D(&tex_desc, &tex_init, &tex);
		if (FAILED(res))
			return false;

		res = device_->CreateShaderResourceView(tex, nullptr, out_srv);

		DestroyCOM(tex);
		if (FAILED(res))
			return false;

		// Store the srv into the map 'srvs_'.
		srvs_[key_string] = *out_srv;

		return true;
	}
}
