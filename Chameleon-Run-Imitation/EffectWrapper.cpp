#include "stdafx.h"

ba::EffectWrapper::EffectWrapper() :
	effect_(nullptr)
{
}

ba::EffectWrapper::~EffectWrapper()
{
}

bool ba::EffectWrapper::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (effect_)
		return false;

	file_name_ = file_name;
	std::wstring const& substr = file_name_.substr(file_name_.find_last_of(L".") + 1);

	if (substr == L"fx")
		return CreateEffectFromFX(device);
	else if (substr == L"fxo")
		return CreateEffectFromFXO(device);

	return false;
}

void ba::EffectWrapper::Release()
{
	ReleaseCOM(effect_);
}

ID3DX11Effect* ba::EffectWrapper::effect()
{
	return effect_;
}

bool ba::EffectWrapper::CreateEffectFromFX(ID3D11Device* device)
{
	UINT shader_flag = 0;
#if defined(DEBUG)||defined(_DEBUG)
	shader_flag |= D3D10_SHADER_DEBUG;
	shader_flag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiled_shader = nullptr;
	ID3D10Blob* compilation_msg = nullptr;

	HRESULT res = D3DX11CompileFromFile(
		file_name_.c_str(), nullptr, nullptr, nullptr, "fx_5_0",
		shader_flag, 0, nullptr, &compiled_shader, &compilation_msg, nullptr
	);
	if (compilation_msg)
	{
		ReleaseCOM(compilation_msg);
		return false;
	}
	if (FAILED(res))
	{
		return false;
	}

	res = D3DX11CreateEffectFromMemory(
		compiled_shader->GetBufferPointer(), compiled_shader->GetBufferSize(), 0,
		device, &effect_
	);
	ReleaseCOM(compiled_shader);
	if (FAILED(res))
	{
		return false;
	}

	return true;
}

bool ba::EffectWrapper::CreateEffectFromFXO(ID3D11Device* device)
{
	std::ifstream fin(file_name_, std::ios::binary);
	if (!fin)
		return false;

	fin.seekg(0, std::ios_base::end);
	int size = static_cast<int>(fin.tellg());
	fin.seekg(0, std::ios_base::beg);

	std::vector<char> compiled_shader(size);
	fin.read(&compiled_shader[0], size);
	fin.close();

	HRESULT res = D3DX11CreateEffectFromMemory(&compiled_shader[0], size, 0, device, &effect_);
	if (FAILED(res))
		return false;

	return true;
}
