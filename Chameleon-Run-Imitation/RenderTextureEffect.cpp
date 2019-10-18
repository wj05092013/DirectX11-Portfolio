#include "stdafx.h"

ba::RenderTextureEffect::RenderTextureEffect() :
	tech_(nullptr),
	tex_mapping_(nullptr),
	input_image_(nullptr)
{
}

ba::RenderTextureEffect::~RenderTextureEffect()
{
}

bool ba::RenderTextureEffect::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (tech_)
		return false;

	if (!EffectWrapper::Init(device, file_name))
		return false;

	ID3DX11Effect* e = effect();

	tech_ = e->GetTechniqueByName("RenderTexture");

	tex_mapping_ = e->GetVariableByName("tex_mapping")->AsMatrix();
	input_image_ = e->GetVariableByName("input_image")->AsShaderResource();

	return true;
}


ID3DX11EffectTechnique* ba::RenderTextureEffect::tech()
{
	return tech_;
}

void ba::RenderTextureEffect::SetTexMapping(const XMMATRIX& matrix)
{
	tex_mapping_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::RenderTextureEffect::SetInputImage(ID3D11ShaderResourceView* srv)
{
	input_image_->SetResource(srv);
}
