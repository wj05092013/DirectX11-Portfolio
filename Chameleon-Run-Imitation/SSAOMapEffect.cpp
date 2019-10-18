#include "stdafx.h"

ba::SSAOMapEffect::SSAOMapEffect() :
	techs_(nullptr),
	far_plane_corners_(nullptr),
	offset_vectors_(nullptr),
	proj_(nullptr),
	to_tex_(nullptr),
	normal_depth_map_(nullptr),
	random_vector_map_(nullptr)
{
}

ba::SSAOMapEffect::~SSAOMapEffect()
{
}

bool ba::SSAOMapEffect::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (techs_)
		return false;

	if (!EffectWrapper::Init(device, file_name))
		return false;

	ID3DX11Effect* e = effect();

	techs_ = new ID3DX11EffectTechnique * [TechType::kMax];

	techs_[kSSAOMap] = e->GetTechniqueByName("SSAOMap");

	far_plane_corners_ = e->GetVariableByName("far_plane_corners")->AsVector();
	offset_vectors_ = e->GetVariableByName("offset_vectors")->AsVector();

	proj_ = e->GetVariableByName("proj")->AsMatrix();
	
	to_tex_ = e->GetVariableByName("to_tex")->AsMatrix();
	normal_depth_map_ = e->GetVariableByName("normal_depth_map")->AsShaderResource();
	random_vector_map_ = e->GetVariableByName("random_vector_map")->AsShaderResource();

	return true;
}

void ba::SSAOMapEffect::Release()
{
	if (techs_)
	{
		delete[]techs_;
		techs_ = nullptr;
	}

	EffectWrapper::Release();
}

ID3DX11EffectTechnique* ba::SSAOMapEffect::tech(TechType type)
{
	return techs_[type];
}

void ba::SSAOMapEffect::SetFarPlaneCorners(const XMFLOAT4 vec[4])
{
	far_plane_corners_->SetFloatVectorArray(reinterpret_cast<const float*>(vec), 0, 4);
}

void ba::SSAOMapEffect::SetOffsetVectors(const XMFLOAT4 vec[14])
{
	offset_vectors_->SetFloatVectorArray(reinterpret_cast<const float*>(vec), 0, 14);
}

void ba::SSAOMapEffect::SetProj(const XMMATRIX& matrix)
{
	proj_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::SSAOMapEffect::SetToTex(const XMMATRIX& matrix)
{
	to_tex_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::SSAOMapEffect::SetNormalDepthMap(ID3D11ShaderResourceView* srv)
{
	normal_depth_map_->SetResource(srv);
}

void ba::SSAOMapEffect::SetRandomVectorMap(ID3D11ShaderResourceView* srv)
{
	normal_depth_map_->SetResource(srv);
}
