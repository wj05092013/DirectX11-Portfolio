#include "stdafx.h"

ba::TextureManager::TextureManager() :
	device_(nullptr)
{
}

ba::TextureManager& ba::TextureManager::GetInstance()
{
	static TextureManager instance;
	return instance;
}

void ba::TextureManager::Init(ID3D11Device* device)
{
	device_ = device;
}

void ba::TextureManager::Release()
{
	device_ = nullptr;
}

bool ba::TextureManager::CreateSRV(const std::wstring& file_name, ID3D11ShaderResourceView** out_srv)
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
