#include "stdafx.h"
#include "Constants.h"

ba::Renderer::Renderer() :
	dc_(nullptr),

	screen_desc_{},
	camera_(nullptr),
	shadow_map_(nullptr),
	ssao_map_(nullptr)
{
}

ba::Renderer::~Renderer()
{
}

bool ba::Renderer::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	dc_ = dc;

	if (!effects::InitAll(device)) { Destroy(); return false; }
	if (!renderstates::InitAll(device)) { Destroy(); return false; }

	return true;
}

void ba::Renderer::Destroy()
{
	effects::DestroyAll();
	renderstates::DestroyAll();
}

void ba::Renderer::RenderScene(const std::vector<Model*>& models, const EffectVariableBundlePerFrame& bundle)
{
	ID3D11RenderTargetView* rtvs_[1] = { screen_desc_.rtv };
	dc_->OMSetRenderTargets(1, rtvs_, screen_desc_.dsv);
	dc_->RSSetViewports(1, screen_desc_.viewport);

	dc_->ClearRenderTargetView(screen_desc_.rtv, reinterpret_cast<const float*>(&color::kSilver));
	dc_->ClearDepthStencilView(screen_desc_.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);

	ID3DX11EffectTechnique* tech = nullptr;

	tech = effects::kBasicEffect.tech(BasicEffect::kLight3Fog);

	effects::kBasicEffect.SetEyePos(camera_->position_w_xf());
	effects::kBasicEffect.SetView(camera_->view());
	effects::kBasicEffect.SetShadowTransform(shadow_map_->world_to_tex());
	effects::kBasicEffect.SetShadowMap(shadow_map_->srv());
	effects::kBasicEffect.SetSSAOMap(ssao_map_->ssao_map_srv());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < models.size(); ++model_idx)
		{
			const ModelData* model_data = models[model_idx]->model_data();

			world = models[model_idx]->local_world();
			world_inv_trans = mathhelper::InverseTranspose(world);

			effects::kBasicEffect.SetWorld(world);
			effects::kBasicEffect.SetWorldInvTrans(world_inv_trans);
			effects::kBasicEffect.SetTexMapping(XMMatrixIdentity());
			effects::kBasicEffect.SetMaterial(models[model_idx]->material());
			effects::kBasicEffect.SetDiffuseMap(model_data->diffuse_map);

			tech->GetPassByIndex(p)->Apply(0, dc_);
			model_data->mesh.Draw(dc_);
		}

		// Unbind the SRVs in case the resources are bound as render targets.
		effects::kBasicEffect.SetShadowMap(nullptr);
		effects::kBasicEffect.SetSSAOMap(nullptr);
		tech->GetPassByIndex(p)->Apply(0, dc_);
	}
}

void ba::Renderer::RenderShadowMap(const std::vector<Model*>& models, const EffectVariableBundlePerFrame& bundle)
{
	ID3D11RenderTargetView* rtvs_[1] = { nullptr };
	dc_->OMSetRenderTargets(1, rtvs_, shadow_map_->dsv());
	dc_->ClearDepthStencilView(shadow_map_->dsv(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	dc_->RSSetViewports(1, &shadow_map_->viewport());

	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);

	ID3DX11EffectTechnique* tech = nullptr;

	tech = effects::kShadowMapEffect.tech(ShadowMapEffect::kBuildShadowMap);

	effects::kShadowMapEffect.SetLightView(shadow_map_->view());
	effects::kShadowMapEffect.SetLightProj(shadow_map_->proj());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < models.size(); ++model_idx)
		{
			const ModelData* model_data = models[model_idx]->model_data();

			world = models[model_idx]->local_world();
			world_inv_trans = mathhelper::InverseTranspose(world);

			effects::kShadowMapEffect.SetWorld(world);
			effects::kShadowMapEffect.SetWorldInvTrans(world_inv_trans);
			effects::kShadowMapEffect.SetTexMapping(XMMatrixIdentity());
			effects::kShadowMapEffect.SetDiffuseMap(model_data->diffuse_map);

			tech->GetPassByIndex(p)->Apply(0, dc_);
			model_data->mesh.Draw(dc_);
		}
	}
	dc_->RSSetState(nullptr);
}

void ba::Renderer::RenderNormalDepthMap(const std::vector<Model*>& models, const EffectVariableBundlePerFrame& bundle)
{
	ID3D11RenderTargetView* rtvs_[1] = { ssao_map_->normal_depth_map_rtv() };
	dc_->OMSetRenderTargets(1, rtvs_, ssao_map_->dsv());
	dc_->RSSetViewports(1, screen_desc_.viewport);

	float clear_color[4] = { 0.0f, 0.0f, -1.0f, 1e5f };
	dc_->ClearRenderTargetView(rtvs_[0], clear_color);
	dc_->ClearDepthStencilView(ssao_map_->dsv(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);

	ID3DX11EffectTechnique* tech = nullptr;

	tech = effects::kNormalDepthMapEffect.tech(NormalDepthMapEffect::kNormalDepthMap);

	effects::kNormalDepthMapEffect.SetView(camera_->view());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < models.size(); ++model_idx)
		{
			const ModelData* model_data = models[model_idx]->model_data();

			world = models[model_idx]->local_world();
			world_inv_trans = mathhelper::InverseTranspose(world);

			effects::kNormalDepthMapEffect.SetWorld(world);
			effects::kNormalDepthMapEffect.SetWorldInvTrans(world_inv_trans);
			effects::kNormalDepthMapEffect.SetTexMapping(XMMatrixIdentity());
			effects::kNormalDepthMapEffect.SetDiffuseMap(model_data->diffuse_map);

			tech->GetPassByIndex(p)->Apply(0, dc_);
			model_data->mesh.Draw(dc_);
		}
	}
}

void ba::Renderer::SetEffectVariablesOnStartAndResize(const EffectVariableBundleOnStartAndResize& bundle)
{
	// BasicEffect
	effects::kBasicEffect.SetProj(camera_->proj());

	// ShadowMapEffect

	// NormalDepthMapEffect
	effects::kNormalDepthMapEffect.SetProj(camera_->proj());
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

void ba::Renderer::set_screen_desc(const ScreenDesc& screen_desc)
{
	screen_desc_ = screen_desc;
}

void ba::Renderer::set_camera(Camera* camera)
{
	camera_ = camera;
}

void ba::Renderer::set_shadow_map(ShadowMap* shadow_map)
{
	shadow_map_ = shadow_map;
}

void ba::Renderer::set_ssao_map(SSAOMap* ssao_map)
{
	ssao_map_ = ssao_map;
}
