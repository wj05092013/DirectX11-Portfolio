#include "stdafx.h"

ba::BlurSSAOEffect::BlurSSAOEffect() :
	techs_(nullptr),
	texel_width_(nullptr),
	texel_height_(nullptr),
	normal_depth_map_(nullptr),
	input_image_(nullptr)
{
}

ba::BlurSSAOEffect::~BlurSSAOEffect()
{
}

bool ba::BlurSSAOEffect::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (techs_)
		return false;

	if (!EffectWrapper::Init(device, file_name))
		return false;

	ID3DX11Effect* e = effect();

	techs_ = new ID3DX11EffectTechnique * [TechType::kMax];

	techs_[kHorizontalBlur] = e->GetTechniqueByName("HorizontalBlur");
	techs_[kVerticalBlur] = e->GetTechniqueByName("VerticalBlur");

	texel_width_ = e->GetVariableByName("texel_width")->AsScalar();
	texel_height_ = e->GetVariableByName("texel_height")->AsScalar();

	normal_depth_map_ = e->GetVariableByName("normal_depth_map")->AsShaderResource();
	input_image_ = e->GetVariableByName("input_image")->AsShaderResource();

	return true;
}

void ba::BlurSSAOEffect::Release()
{
	if (techs_)
	{
		delete[]techs_;
		techs_ = nullptr;
	}

	EffectWrapper::Release();
}

ID3DX11EffectTechnique* ba::BlurSSAOEffect::tech(TechType type)
{
	return techs_[type];
}

void ba::BlurSSAOEffect::SetTexelWidth(float width)
{
	texel_width_->SetFloat(width);
}

void ba::BlurSSAOEffect::SetTexelHeight(float height)
{
	texel_height_->SetFloat(height);
}

void ba::BlurSSAOEffect::SetNormalDepthMap(ID3D11ShaderResourceView* srv)
{
	normal_depth_map_->SetResource(srv);
}

void ba::BlurSSAOEffect::SetInputImage(ID3D11ShaderResourceView* srv)
{
	input_image_->SetResource(srv);
}
