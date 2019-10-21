#include "stdafx.h"

ba::Renderer::Renderer() :
	dc_(nullptr),
	rendering_components_{}
{
}

ba::Renderer::~Renderer()
{
}

bool ba::Renderer::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	dc_ = dc;

	if (!effects::InitAll(device)) { Destroy(); return false; }
	if(!renderstates::InitAll(device)) { Destroy(); return false; }

	return true;
}

void ba::Renderer::Destroy()
{
	effects::DestroyAll();
	renderstates::DestroyAll();
}

void ba::Renderer::RenderScene(const std::vector<ModelInstance>& model_instances, const EffectVariableBundlePerFrame& bundle)
{
	ID3D11RenderTargetView* rtvs_[1] = { rendering_components_.rtv };
	dc_->OMSetRenderTargets(1, rtvs_, rendering_components_.dsv);
	dc_->RSSetViewports(1, rendering_components_.viewport);

	dc_->ClearRenderTargetView(rendering_components_.rtv, reinterpret_cast<const float*>(&color::kSilver));
	dc_->ClearDepthStencilView(rendering_components_.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);

	ID3DX11EffectTechnique* tech = nullptr;

	tech = effects::kBasicEffect.tech(BasicEffect::kLight3);

	effects::kBasicEffect.SetEyePos(rendering_components_.cam->position_w_xf());
	effects::kBasicEffect.SetView(rendering_components_.cam->view());
	effects::kBasicEffect.SetShadowTransform(rendering_components_.shadow_map->world_to_tex());
	effects::kBasicEffect.SetShadowMap(rendering_components_.shadow_map->srv());
	effects::kBasicEffect.SetSSAOMap(rendering_components_.ssao_map->ssao_map_srv());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < model_instances.size(); ++model_idx)
		{
			Model* model = model_instances[model_idx].model;

			world = model->mesh.transform()
				* model_instances[model_idx].scale
				* model_instances[model_idx].rotation
				* model_instances[model_idx].translation;

			world_inv_trans = mathhelper::InverseTranspose(world);

			effects::kBasicEffect.SetWorld(world);
			effects::kBasicEffect.SetWorldInvTrans(world_inv_trans);
			effects::kBasicEffect.SetTexMapping(XMMatrixIdentity());
			effects::kBasicEffect.SetMaterial(model->mesh.material());
			effects::kBasicEffect.SetDiffuseMap(model->diffuse_map);

			tech->GetPassByIndex(p)->Apply(0, dc_);
			model->mesh.Draw(dc_);
		}

		// Unbind the SRVs in case the resources are bound as render targets.
		effects::kBasicEffect.SetShadowMap(nullptr);
		effects::kBasicEffect.SetSSAOMap(nullptr);
		tech->GetPassByIndex(p)->Apply(0, dc_);
	}
}

void ba::Renderer::RenderShadowMap(const std::vector<ModelInstance>& model_instances, const EffectVariableBundlePerFrame& bundle)
{
	ID3D11RenderTargetView* rtvs_[1] = { nullptr };
	dc_->OMSetRenderTargets(1, rtvs_, rendering_components_.shadow_map->dsv());
	dc_->ClearDepthStencilView(rendering_components_.shadow_map->dsv(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	dc_->RSSetViewports(1, &rendering_components_.shadow_map->viewport());

	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);

	ID3DX11EffectTechnique* tech = nullptr;

	tech = effects::kShadowMapEffect.tech(ShadowMapEffect::kBuildShadowMap);

	effects::kShadowMapEffect.SetLightView(rendering_components_.shadow_map->view());
	effects::kShadowMapEffect.SetLightProj(rendering_components_.shadow_map->proj());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < model_instances.size(); ++model_idx)
		{
			Model* model = model_instances[model_idx].model;

			world = model->mesh.transform()
				* model_instances[model_idx].scale
				* model_instances[model_idx].rotation
				* model_instances[model_idx].translation;

			world_inv_trans = mathhelper::InverseTranspose(world);

			effects::kShadowMapEffect.SetWorld(world);
			effects::kShadowMapEffect.SetWorldInvTrans(world_inv_trans);
			effects::kShadowMapEffect.SetTexMapping(XMMatrixIdentity());
			effects::kShadowMapEffect.SetDiffuseMap(model->diffuse_map);

			tech->GetPassByIndex(p)->Apply(0, dc_);
			model->mesh.Draw(dc_);
		}
	}
	dc_->RSSetState(nullptr);
}

void ba::Renderer::RenderNormalDepthMap(const std::vector<ModelInstance>& model_instances, const EffectVariableBundlePerFrame& bundle)
{
	ID3D11RenderTargetView* rtvs_[1] = { rendering_components_.ssao_map->normal_depth_map_rtv() };
	dc_->OMSetRenderTargets(1, rtvs_, rendering_components_.ssao_map->dsv());
	dc_->RSSetViewports(1, rendering_components_.viewport);

	float clear_color[4] = { 0.0f, 0.0f, -1.0f, 1e5f };
	dc_->ClearRenderTargetView(rtvs_[0], clear_color);
	dc_->ClearDepthStencilView(rendering_components_.ssao_map->dsv(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);

	ID3DX11EffectTechnique* tech = nullptr;

	tech = effects::kNormalDepthMapEffect.tech(NormalDepthMapEffect::kNormalDepthMap);

	effects::kNormalDepthMapEffect.SetView(rendering_components_.cam->view());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < model_instances.size(); ++model_idx)
		{
			Model* model = model_instances[model_idx].model;

			world = model->mesh.transform()
				* model_instances[model_idx].scale
				* model_instances[model_idx].rotation
				* model_instances[model_idx].translation;

			world_inv_trans = mathhelper::InverseTranspose(world);

			effects::kNormalDepthMapEffect.SetWorld(world);
			effects::kNormalDepthMapEffect.SetWorldInvTrans(world_inv_trans);
			effects::kNormalDepthMapEffect.SetTexMapping(XMMatrixIdentity());
			effects::kNormalDepthMapEffect.SetDiffuseMap(model->diffuse_map);

			tech->GetPassByIndex(p)->Apply(0, dc_);
			model->mesh.Draw(dc_);
		}
	}
}

void ba::Renderer::SetEffectVariablesOnStartAndResize(const EffectVariableBundleOnStartAndResize& bundle)
{
	// BasicEffect
	effects::kBasicEffect.SetProj(rendering_components_.cam->proj());

	// ShadowMapEffect

	// NormalDepthMapEffect
	effects::kNormalDepthMapEffect.SetProj(rendering_components_.cam->proj());
}

void ba::Renderer::SetEffectVariablesChangeRarely(const EffectVariableBundleChangeRarely& bundle)
{
	// BasicEffect
	effects::kBasicEffect.SetDirectionalLights(bundle.directional_lights);
	effects::kBasicEffect.SetFogStart(bundle.fog_start);
	effects::kBasicEffect.SetFogRange(bundle.fog_range);
	effects::kBasicEffect.SetFogColor(bundle.fog_color);
	effects::kBasicEffect.SetShadowMapSize(bundle.shadow_map_size);
	effects::kBasicEffect.SetToTex(bundle.to_tex);

	// ShadowMapEffect

	// NormalDepthMapEffect
}

void ba::Renderer::set_rendering_components(const SceneRenderingComponents& rendering_component)
{
	rendering_components_ = rendering_component;
}
