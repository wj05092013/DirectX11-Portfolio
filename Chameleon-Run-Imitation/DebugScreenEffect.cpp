#include "stdafx.h"

ba::DebugScreenEffect::DebugScreenEffect() :
	techs_(nullptr),
	proj_(nullptr),
	debug_tex_(nullptr)
{
}

ba::DebugScreenEffect::~DebugScreenEffect()
{
}

bool ba::DebugScreenEffect::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (techs_)
		return false;

	if (!EffectWrapper::Init(device, file_name))
		return false;

	ID3DX11Effect* e = effect();

	techs_ = new ID3DX11EffectTechnique * [TechType::kMax];

	techs_[kViewRGBA] = e->GetTechniqueByName("ViewRGBA");
	techs_[kViewRed] = e->GetTechniqueByName("ViewRed");
	techs_[kViewGreen] = e->GetTechniqueByName("ViewGreen");
	techs_[kViewBlue] = e->GetTechniqueByName("ViewBlue");
	techs_[kViewAlpha] = e->GetTechniqueByName("ViewAlpha");

	proj_ = e->GetVariableByName("proj")->AsMatrix();
	debug_tex_ = e->GetVariableByName("debug_tex")->AsShaderResource();

	return true;
}

void ba::DebugScreenEffect::Release()
{
	if (techs_)
	{
		delete[]techs_;
		techs_ = nullptr;
	}

	EffectWrapper::Release();
}

ID3DX11EffectTechnique* ba::DebugScreenEffect::tech(TechType type)
{
	return techs_[type];
}

void ba::DebugScreenEffect::SetProj(const XMMATRIX& matrix)
{
	proj_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::DebugScreenEffect::SetDebugTex(ID3D11ShaderResourceView* srv)
{
	debug_tex_->SetResource(srv);
}
