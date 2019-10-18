#include "stdafx.h"

ba::NormalDepthMapEffect::NormalDepthMapEffect() :
	techs_(nullptr),

	world_(nullptr),
	world_inv_trans_(nullptr),
	tex_mapping_(nullptr),
	diffuse_map_(nullptr),

	view_(nullptr),
	proj_(nullptr)
{
}

ba::NormalDepthMapEffect::~NormalDepthMapEffect()
{
}

bool ba::NormalDepthMapEffect::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (techs_)
		return false;

	if (!EffectWrapper::Init(device, file_name))
		return false;

	ID3DX11Effect* e = effect();

	techs_ = new ID3DX11EffectTechnique * [TechType::kMax];

	techs_[kNormalDepthMap] = e->GetTechniqueByName("NormalDepthMap");
	techs_[kNormalDepthMapAlphaClip] = e->GetTechniqueByName("NormalDepthMapAlphaClip");

	world_ = e->GetVariableByName("world")->AsMatrix();
	world_inv_trans_ = e->GetVariableByName("world_inv_trans")->AsMatrix();
	tex_mapping_ = e->GetVariableByName("tex_mapping")->AsMatrix();
	diffuse_map_ = e->GetVariableByName("diffuse_map")->AsShaderResource();

	view_ = e->GetVariableByName("view")->AsMatrix();

	proj_ = e->GetVariableByName("proj")->AsMatrix();

	return true;
}

void ba::NormalDepthMapEffect::Release()
{
	if (techs_)
	{
		delete[]techs_;
		techs_ = nullptr;
	}

	EffectWrapper::Release();
}

ID3DX11EffectTechnique* ba::NormalDepthMapEffect::tech(TechType type)
{
	return techs_[type];
}

void ba::NormalDepthMapEffect::SetWorld(const XMMATRIX& matrix)
{
	world_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalDepthMapEffect::SetWorldInvTrans(const XMMATRIX& matrix)
{
	world_inv_trans_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalDepthMapEffect::SetTexMapping(const XMMATRIX& matrix)
{
	tex_mapping_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalDepthMapEffect::SetDiffuseMap(ID3D11ShaderResourceView* srv)
{
	diffuse_map_->SetResource(srv);
}

void ba::NormalDepthMapEffect::SetView(const XMMATRIX& matrix)
{
	view_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalDepthMapEffect::SetProj(const XMMATRIX& matrix)
{
	proj_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}
