#include "stdafx.h"

namespace ba
{
	//
	// Helpers
	//

	void DestroyTextures(std::vector<ID3D11Texture2D*>& textures)
	{
		for (int i = 0; i < textures.size(); ++i)
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

	bool ba::TextureManager::CreateTexSRV(const std::wstring& file_name, ID3D11ShaderResourceView** out_srv)
	{
		if (!device_)
			return false;

		if (srvs_.find(file_name) == srvs_.end())
		{
			HRESULT res = D3DX11CreateShaderResourceViewFromFile(
				device_, file_name.c_str(), nullptr, nullptr,
				out_srv, nullptr
			);
			if (FAILED(res))
				return false;

			srvs_[file_name] = *out_srv;
		}
		else
		{
			*out_srv = srvs_[file_name];
		}
		return true;
	}

	bool ba::TextureManager::CreateTexArrSRV(
		const std::vector<std::wstring>& file_names, ID3D11ShaderResourceView** out_srv,
		DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
		UINT filter = D3DX11_FILTER_NONE, UINT mip_filter = D3DX11_FILTER_LINEAR
	)
	{
		if (!device_ || !dc_)
			return false;

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
			for (UINT mip_level = 0; mip_level < tex_arr_desc.MipLevels; ++mip_level)
			{
				res = dc_->Map(textures[tex_idx], mip_level, D3D11_MAP_READ, 0, &mapped_data);
				if (FAILED(res))
				{
					dc_->Unmap(textures[tex_idx], mip_level);
					DestroyTextures(textures);
					DestroyCOM(tex_arr);
					return false;
				}

				dc_->UpdateSubresource(tex_arr, D3D11CalcSubresource(), nullptr, )

				dc_->Unmap(textures[tex_idx], mip_level);
			}
			
		}
		//__

		return true;
	}
}
