#include "stdafx.h"

ba::NormalMappingEffect::NormalMappingEffect() :
	techs_(nullptr),

	world_(nullptr),
	world_inv_trans_(nullptr),
	tex_mapping_(nullptr),
	material_(nullptr),
	diffuse_map_(nullptr),
	normal_map_(nullptr),
	bone_transforms_(nullptr),

	eye_pos_(nullptr),
	view_(nullptr),
	shadow_transform_(nullptr),
	shadow_map_(nullptr),
	ssao_map_(nullptr),

	proj_(nullptr),

	directional_lights_(nullptr),
	fog_start_(nullptr),
	fog_range_(nullptr),
	fog_color_(nullptr),
	shadow_map_size_(nullptr),
	to_tex_(nullptr),
	cube_map_(nullptr)
{
}

ba::NormalMappingEffect::~NormalMappingEffect()
{
}

bool ba::NormalMappingEffect::Init(ID3D11Device* device, const std::wstring& file_name)
{
	if (techs_)
		return false;

	if (!EffectWrapper::Init(device, file_name))
		return false;

	ID3DX11Effect* e = effect();

	techs_ = new ID3DX11EffectTechnique * [TechType::kMax];

	techs_[kLight1] = e->GetTechniqueByName("Light1");
	techs_[kLight2] = e->GetTechniqueByName("Light2");
	techs_[kLight3] = e->GetTechniqueByName("Light3");
	techs_[kLight0Tex] = e->GetTechniqueByName("Light0Tex");
	techs_[kLight1Tex] = e->GetTechniqueByName("Light1Tex");
	techs_[kLight2Tex] = e->GetTechniqueByName("Light2Tex");
	techs_[kLight3Tex] = e->GetTechniqueByName("Light3Tex");
	techs_[kLight0TexAlphaClip] = e->GetTechniqueByName("Light0TexAlphaClip");
	techs_[kLight1TexAlphaClip] = e->GetTechniqueByName("Light1TexAlphaClip");
	techs_[kLight2TexAlphaClip] = e->GetTechniqueByName("Light2TexAlphaClip");
	techs_[kLight3TexAlphaClip] = e->GetTechniqueByName("Light3TexAlphaClip");
	techs_[kLight1Fog] = e->GetTechniqueByName("Light1Fog");
	techs_[kLight2Fog] = e->GetTechniqueByName("Light2Fog");
	techs_[kLight3Fog] = e->GetTechniqueByName("Light3Fog");
	techs_[kLight0TexFog] = e->GetTechniqueByName("Light0TexFog");
	techs_[kLight1TexFog] = e->GetTechniqueByName("Light1TexFog");
	techs_[kLight2TexFog] = e->GetTechniqueByName("Light2TexFog");
	techs_[kLight3TexFog] = e->GetTechniqueByName("Light3TexFog");
	techs_[kLight0TexAlphaClipFog] = e->GetTechniqueByName("Light0TexAlphaClipFog");
	techs_[kLight1TexAlphaClipFog] = e->GetTechniqueByName("Light1TexAlphaClipFog");
	techs_[kLight2TexAlphaClipFog] = e->GetTechniqueByName("Light2TexAlphaClipFog");
	techs_[kLight3TexAlphaClipFog] = e->GetTechniqueByName("Light3TexAlphaClipFog");

	techs_[kLight1Reflect] = e->GetTechniqueByName("Light1Reflect");
	techs_[kLight2Reflect] = e->GetTechniqueByName("Light2Reflect");
	techs_[kLight3Reflect] = e->GetTechniqueByName("Light3Reflect");
	techs_[kLight0TexReflect] = e->GetTechniqueByName("Light0TexReflect");
	techs_[kLight1TexReflect] = e->GetTechniqueByName("Light1TexReflect");
	techs_[kLight2TexReflect] = e->GetTechniqueByName("Light2TexReflect");
	techs_[kLight3TexReflect] = e->GetTechniqueByName("Light3TexReflect");
	techs_[kLight0TexAlphaClipReflect] = e->GetTechniqueByName("Light0TexAlphaClipReflect");
	techs_[kLight1TexAlphaClipReflect] = e->GetTechniqueByName("Light1TexAlphaClipReflect");
	techs_[kLight2TexAlphaClipReflect] = e->GetTechniqueByName("Light2TexAlphaClipReflect");
	techs_[kLight3TexAlphaClipReflect] = e->GetTechniqueByName("Light3TexAlphaClipReflect");
	techs_[kLight1FogReflect] = e->GetTechniqueByName("Light1FogReflect");
	techs_[kLight2FogReflect] = e->GetTechniqueByName("Light2FogReflect");
	techs_[kLight3FogReflect] = e->GetTechniqueByName("Light3FogReflect");
	techs_[kLight0TexFogReflect] = e->GetTechniqueByName("Light0TexFogReflect");
	techs_[kLight1TexFogReflect] = e->GetTechniqueByName("Light1TexFogReflect");
	techs_[kLight2TexFogReflect] = e->GetTechniqueByName("Light2TexFogReflect");
	techs_[kLight3TexFogReflect] = e->GetTechniqueByName("Light3TexFogReflect");
	techs_[kLight0TexAlphaClipFogReflect] = e->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	techs_[kLight1TexAlphaClipFogReflect] = e->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	techs_[kLight2TexAlphaClipFogReflect] = e->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	techs_[kLight3TexAlphaClipFogReflect] = e->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	techs_[kLight1Skinned] = e->GetTechniqueByName("Light1Skinned");
	techs_[kLight2Skinned] = e->GetTechniqueByName("Light2Skinned");
	techs_[kLight3Skinned] = e->GetTechniqueByName("Light3Skinned");
	techs_[kLight0TexSkinned] = e->GetTechniqueByName("Light0TexSkinned");
	techs_[kLight1TexSkinned] = e->GetTechniqueByName("Light1TexSkinned");
	techs_[kLight2TexSkinned] = e->GetTechniqueByName("Light2TexSkinned");
	techs_[kLight3TexSkinned] = e->GetTechniqueByName("Light3TexSkinned");
	techs_[kLight0TexAlphaClipSkinned] = e->GetTechniqueByName("Light0TexAlphaClipSkinned");
	techs_[kLight1TexAlphaClipSkinned] = e->GetTechniqueByName("Light1TexAlphaClipSkinned");
	techs_[kLight2TexAlphaClipSkinned] = e->GetTechniqueByName("Light2TexAlphaClipSkinned");
	techs_[kLight3TexAlphaClipSkinned] = e->GetTechniqueByName("Light3TexAlphaClipSkinned");
	techs_[kLight1FogSkinned] = e->GetTechniqueByName("Light1FogSkinned");
	techs_[kLight2FogSkinned] = e->GetTechniqueByName("Light2FogSkinned");
	techs_[kLight3FogSkinned] = e->GetTechniqueByName("Light3FogSkinned");
	techs_[kLight0TexFogSkinned] = e->GetTechniqueByName("Light0TexFogSkinned");
	techs_[kLight1TexFogSkinned] = e->GetTechniqueByName("Light1TexFogSkinned");
	techs_[kLight2TexFogSkinned] = e->GetTechniqueByName("Light2TexFogSkinned");
	techs_[kLight3TexFogSkinned] = e->GetTechniqueByName("Light3TexFogSkinned");
	techs_[kLight0TexAlphaClipFogSkinned] = e->GetTechniqueByName("Light0TexAlphaClipFogSkinned");
	techs_[kLight1TexAlphaClipFogSkinned] = e->GetTechniqueByName("Light1TexAlphaClipFogSkinned");
	techs_[kLight2TexAlphaClipFogSkinned] = e->GetTechniqueByName("Light2TexAlphaClipFogSkinned");
	techs_[kLight3TexAlphaClipFogSkinned] = e->GetTechniqueByName("Light3TexAlphaClipFogSkinned");

	techs_[kLight1ReflectSkinned] = e->GetTechniqueByName("Light1ReflectSkinned");
	techs_[kLight2ReflectSkinned] = e->GetTechniqueByName("Light2ReflectSkinned");
	techs_[kLight3ReflectSkinned] = e->GetTechniqueByName("Light3ReflectSkinned");
	techs_[kLight0TexReflectSkinned] = e->GetTechniqueByName("Light0TexReflectSkinned");
	techs_[kLight1TexReflectSkinned] = e->GetTechniqueByName("Light1TexReflectSkinned");
	techs_[kLight2TexReflectSkinned] = e->GetTechniqueByName("Light2TexReflectSkinned");
	techs_[kLight3TexReflectSkinned] = e->GetTechniqueByName("Light3TexReflectSkinned");
	techs_[kLight0TexAlphaClipReflectSkinned] = e->GetTechniqueByName("Light0TexAlphaClipReflectSkinned");
	techs_[kLight1TexAlphaClipReflectSkinned] = e->GetTechniqueByName("Light1TexAlphaClipReflectSkinned");
	techs_[kLight2TexAlphaClipReflectSkinned] = e->GetTechniqueByName("Light2TexAlphaClipReflectSkinned");
	techs_[kLight3TexAlphaClipReflectSkinned] = e->GetTechniqueByName("Light3TexAlphaClipReflectSkinned");
	techs_[kLight1FogReflectSkinned] = e->GetTechniqueByName("Light1FogReflectSkinned");
	techs_[kLight2FogReflectSkinned] = e->GetTechniqueByName("Light2FogReflectSkinned");
	techs_[kLight3FogReflectSkinned] = e->GetTechniqueByName("Light3FogReflectSkinned");
	techs_[kLight0TexFogReflectSkinned] = e->GetTechniqueByName("Light0TexFogReflectSkinned");
	techs_[kLight1TexFogReflectSkinned] = e->GetTechniqueByName("Light1TexFogReflectSkinned");
	techs_[kLight2TexFogReflectSkinned] = e->GetTechniqueByName("Light2TexFogReflectSkinned");
	techs_[kLight3TexFogReflectSkinned] = e->GetTechniqueByName("Light3TexFogReflectSkinned");
	techs_[kLight0TexAlphaClipFogReflectSkinned] = e->GetTechniqueByName("Light0TexAlphaClipFogReflectSkinned");
	techs_[kLight1TexAlphaClipFogReflectSkinned] = e->GetTechniqueByName("Light1TexAlphaClipFogReflectSkinned");
	techs_[kLight2TexAlphaClipFogReflectSkinned] = e->GetTechniqueByName("Light2TexAlphaClipFogReflectSkinned");
	techs_[kLight3TexAlphaClipFogReflectSkinned] = e->GetTechniqueByName("Light3TexAlphaClipFogReflectSkinned");

	world_ = e->GetVariableByName("world")->AsMatrix();
	world_inv_trans_ = e->GetVariableByName("world_inv_trans")->AsMatrix();
	tex_mapping_ = e->GetVariableByName("tex_mapping")->AsMatrix();
	material_ = e->GetVariableByName("material");
	diffuse_map_ = e->GetVariableByName("diffuse_map")->AsShaderResource();
	normal_map_ = e->GetVariableByName("normal_map")->AsShaderResource();
	bone_transforms_ = e->GetVariableByName("bone_transforms")->AsMatrix();

	eye_pos_ = e->GetVariableByName("eye_pos")->AsVector();
	view_ = e->GetVariableByName("view")->AsMatrix();
	shadow_transform_ = e->GetVariableByName("shadow_transform")->AsMatrix();
	shadow_map_ = e->GetVariableByName("shadow_map")->AsShaderResource();
	ssao_map_ = e->GetVariableByName("ssao_map")->AsShaderResource();

	proj_ = e->GetVariableByName("proj")->AsMatrix();

	directional_lights_ = e->GetVariableByName("directional_lights");
	fog_start_ = e->GetVariableByName("fog_start")->AsScalar();
	fog_range_ = e->GetVariableByName("fog_range")->AsScalar();
	fog_color_ = e->GetVariableByName("fog_color")->AsVector();
	shadow_map_size_ = e->GetVariableByName("shadow_map_size")->AsScalar();
	to_tex_ = e->GetVariableByName("to_tex")->AsMatrix();
	cube_map_ = e->GetVariableByName("cube_map")->AsShaderResource();

	return true;
}

void ba::NormalMappingEffect::Release()
{
	if (techs_)
	{
		delete[]techs_;
		techs_ = nullptr;
	}

	EffectWrapper::Release();
}

ID3DX11EffectTechnique* ba::NormalMappingEffect::tech(TechType type)
{
	return techs_[type];
}

void ba::NormalMappingEffect::SetWorld(const XMMATRIX& matrix)
{
	world_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalMappingEffect::SetWorldInvTrans(const XMMATRIX& matrix)
{
	world_inv_trans_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalMappingEffect::SetTexMapping(const XMMATRIX& matrix)
{
	tex_mapping_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalMappingEffect::SetMaterial(const light::Material& material)
{
	material_->SetRawValue(&material, 0, sizeof(light::Material));
}

void ba::NormalMappingEffect::SetDiffuseMap(ID3D11ShaderResourceView* srv)
{
	diffuse_map_->SetResource(srv);
}

void ba::NormalMappingEffect::SetNormalMap(ID3D11ShaderResourceView* srv)
{
	normal_map_->SetResource(srv);
}

void ba::NormalMappingEffect::SetBoneTransforms(const XMFLOAT4X4* matrices, UINT count)
{
	bone_transforms_->SetMatrixArray(reinterpret_cast<const float*>(matrices), 0, count);
}

void ba::NormalMappingEffect::SetEyePos(const XMFLOAT3& pos_w)
{
	eye_pos_->SetRawValue(&pos_w, 0, sizeof(XMFLOAT3));
}

void ba::NormalMappingEffect::SetView(const XMMATRIX& matrix)
{
	view_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalMappingEffect::SetShadowTransform(const XMMATRIX& matrix)
{
	shadow_transform_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalMappingEffect::SetShadowMap(ID3D11ShaderResourceView* srv)
{
	shadow_map_->SetResource(srv);
}

void ba::NormalMappingEffect::SetSSAOMap(ID3D11ShaderResourceView* srv)
{
	ssao_map_->SetResource(srv);
}

void ba::NormalMappingEffect::SetProj(const XMMATRIX& matrix)
{
	proj_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalMappingEffect::SetDirectionalLights(const light::DirectionalLight* lits)
{
	directional_lights_->SetRawValue(lits, 0, 3 * sizeof(light::DirectionalLight));
}

void ba::NormalMappingEffect::SetFogStart(float distance)
{
	fog_start_->SetFloat(distance);
}

void ba::NormalMappingEffect::SetFogRange(float range)
{
	fog_range_->SetFloat(range);
}

void ba::NormalMappingEffect::SetFogColor(const XMVECTOR& color)
{
	fog_color_->SetFloatVector(reinterpret_cast<const float*>(&color));
}

void ba::NormalMappingEffect::SetShadowMapSize(float size)
{
	shadow_map_size_->SetFloat(size);
}

void ba::NormalMappingEffect::SetToTex(const XMMATRIX& matrix)
{
	to_tex_->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void ba::NormalMappingEffect::SetCubeMap(ID3D11ShaderResourceView* srv)
{
	cube_map_->SetResource(srv);
}
