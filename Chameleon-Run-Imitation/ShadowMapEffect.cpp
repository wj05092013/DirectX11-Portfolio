#include "stdafx.h"

ba::ShadowMapEffect::ShadowMapEffect() :
	techs_(nullptr),
	world_(nullptr),
	world_inv_trans_(nullptr),
	tex_mapping_(nullptr),
	diffuse_map_(nullptr),
	bone_transforms_(nullptr),

	light_view_(nullptr),
	light_proj_(nullptr)
{
}

ba::ShadowMapEffect::~ShadowMapEffect()
{
}

bool ba::ShadowMapEffect::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (techs_)
		return false;

	if (!EffectWrapper::Init(device, file_name))
		return false;

	ID3DX11Effect* e = effect();

	techs_ = new ID3DX11EffectTechnique * [TechType::kMax];

	techs_[kBuildShadowMap] = e->GetTechniqueByName("BuildShadowMap");
	techs_[kBuildShadowMapAlphaClip] = e->GetTechniqueByName("BuildShadowMapAlphaClip");
	techs_[kBuildShadowMapSkinned] = e->GetTechniqueByName("BuildShadowMapSkinned");
	techs_[kBuildShadowMapAlphaClipSkinned] = e->GetTechniqueByName("BuildShadowMapAlphaClipSkinned");

	world_ = e->GetVariableByName("world")->AsMatrix();
	world_inv_trans_ = e->GetVariableByName("world_inv_trans")->AsMatrix();
	tex_mapping_ = e->GetVariableByName("tex_mapping")->AsMatrix();
	diffuse_map_ = e->GetVariableByName("diffuse_map")->AsShaderResource();
	bone_transforms_ = e->GetVariableByName("bone_transforms")->AsMatrix();

	light_view_ = e->GetVariableByName("light_view")->AsMatrix();
	light_proj_ = e->GetVariableByName("light_proj")->AsMatrix();

	return true;
}

void ba::ShadowMapEffect::Release()
{
	if (techs_)
	{
		delete[]techs_;
		techs_ = nullptr;
	}

	EffectWrapper::Release();
}

ID3DX11EffectTechnique* ba::ShadowMapEffect::tech(TechType type)
{
	return techs_[type];
}

void ba::ShadowMapEffect::SetWorld(const XMMATRIX& matrix)
{
	world_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::ShadowMapEffect::SetWorldInvTrans(const XMMATRIX& matrix)
{
	world_inv_trans_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::ShadowMapEffect::SetTexMapping(const XMMATRIX& matrix)
{
	tex_mapping_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::ShadowMapEffect::SetDiffuseMap(ID3D11ShaderResourceView* srv)
{
	diffuse_map_->SetResource(srv);
}

void ba::ShadowMapEffect::SetBoneTransforms(const XMFLOAT4X4* matrices, UINT count)
{
	bone_transforms_->SetMatrixArray(reinterpret_cast<const float*>(matrices), 0, count);
}

void ba::ShadowMapEffect::SetLightView(const XMMATRIX& matrix)
{
	light_view_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::ShadowMapEffect::SetLightProj(const XMMATRIX& matrix)
{
	light_proj_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}
