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

	if (!effects::InitAll(device)) { Release(); return false; }
	if(!renderstates::InitAll(device)) { Release(); return false; }
	if (!BuildScreenQuadBuffers(device)) { Release(); return false; }

	return true;
}

void ba::Renderer::Release()
{
	effects::ReleaseAll();
	renderstates::ReleaseAll();
	ReleaseScreenQuadBuffers();
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

	// Add this line after implementing 'FBXLoader' completely(loading texture part).
	//tech = effects::kBasicEffect.tech(BasicEffect::kLight3TexAlphaClipFog);

	// Delete this line later.
	tech = effects::kBasicEffect.tech(BasicEffect::kLight3);

	// Set effect variables.
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
			std::vector<Mesh>& meshes = model_instances[model_idx].model->meshes;

			for (UINT mesh_idx = 0; mesh_idx < meshes.size(); ++mesh_idx)
			{
				// Update effect variables.
				world = meshes[mesh_idx].transform()
					* model_instances[model_idx].scale
					* model_instances[model_idx].rotation
					* model_instances[model_idx].translation;
				world_inv_trans = mathhelper::InverseTranspose(world);
				effects::kBasicEffect.SetWorld(world);
				effects::kBasicEffect.SetWorldInvTrans(world_inv_trans);
				effects::kBasicEffect.SetTexMapping(XMMatrixIdentity());
				effects::kBasicEffect.SetMaterial(meshes[mesh_idx].material());
				//effects::kBasicEffect.SetDiffuseMap();

				tech->GetPassByIndex(p)->Apply(0, dc_);
				meshes[mesh_idx].Draw(dc_);
			}
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

	// Use these codes after implementing FBXLoader perfectly.
	//tech = effects::kShadowMapEffect.tech(ShadowMapEffect::kBuildShadowMap);

	// Delete this line later.
	tech = effects::kShadowMapEffect.tech(ShadowMapEffect::kBuildShadowMap);

	// Set effect variables.
	effects::kShadowMapEffect.SetLightView(rendering_components_.shadow_map->view());
	effects::kShadowMapEffect.SetLightProj(rendering_components_.shadow_map->proj());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < model_instances.size(); ++model_idx)
		{
			std::vector<Mesh>& meshes = model_instances[model_idx].model->meshes;

			for (UINT mesh_idx = 0; mesh_idx < meshes.size(); ++mesh_idx)
			{
				world = meshes[mesh_idx].transform()
					* model_instances[model_idx].scale
					* model_instances[model_idx].rotation
					* model_instances[model_idx].translation;
				world_inv_trans = mathhelper::InverseTranspose(world);
				effects::kShadowMapEffect.SetWorld(world);
				effects::kShadowMapEffect.SetWorldInvTrans(world_inv_trans);
				effects::kShadowMapEffect.SetTexMapping(XMMatrixIdentity());
				//effects::kShadowMapEffect.SetDiffuseMap();

				tech->GetPassByIndex(p)->Apply(0, dc_);
				meshes[mesh_idx].Draw(dc_);
			}
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

	// Use these codes after implementing FBXLoader perfectly.
	//tech = effects::kNormalDepthMapEffect.tech(NormalDepthMapEffect::kNormalDepthMapAlphaClip);

	// Delete this line later.
	tech = effects::kNormalDepthMapEffect.tech(NormalDepthMapEffect::kNormalDepthMap);

	// Set effect variables.
	effects::kNormalDepthMapEffect.SetView(rendering_components_.cam->view());

	XMMATRIX world, world_inv_trans;

	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);
	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		for (UINT model_idx = 0; model_idx < model_instances.size(); ++model_idx)
		{
			std::vector<Mesh>& meshes = model_instances[model_idx].model->meshes;

			for (UINT mesh_idx = 0; mesh_idx < meshes.size(); ++mesh_idx)
			{
				world = meshes[mesh_idx].transform()
					* model_instances[model_idx].scale
					* model_instances[model_idx].rotation
					* model_instances[model_idx].translation;
				world_inv_trans = mathhelper::InverseTranspose(world);
				effects::kNormalDepthMapEffect.SetWorld(world);
				effects::kNormalDepthMapEffect.SetWorldInvTrans(world_inv_trans);
				effects::kNormalDepthMapEffect.SetTexMapping(XMMatrixIdentity());
				//effects::kNormalDepthMapEffect.SetDiffuseMap();

				tech->GetPassByIndex(p)->Apply(0, dc_);
				meshes[mesh_idx].Draw(dc_);
			}
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

void ba::Renderer::set_rendering_components(SceneRenderingComponents& rendering_component)
{
	rendering_components_ = rendering_component;
}

void ba::Renderer::BlendTexture(ID3D11RenderTargetView* dst, ID3D11ShaderResourceView* src, const D3D11_VIEWPORT* viewport, const XMMATRIX& tex_mapping, BlendMode blend_mode)
{
	ID3D11RenderTargetView* rtvs[1] = { dst };
	
	dc_->OMSetRenderTargets(1, rtvs, nullptr);
	dc_->RSSetViewports(1, viewport);

	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);
	dc_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT strides = sizeof(inputvertex::PosNormalTex::Vertex);
	UINT offsets = 0;
	dc_->IASetVertexBuffers(0, 1, &screen_quad_vb_, &strides, &offsets);
	dc_->IASetIndexBuffer(screen_quad_ib_, DXGI_FORMAT_R32_UINT, 0);

	effects::kRenderTextureEffect.SetTexMapping(tex_mapping);
	effects::kRenderTextureEffect.SetInputImage(src);
}

bool ba::Renderer::BuildScreenQuadBuffers(ID3D11Device* device)
{
	if (screen_quad_vb_ || screen_quad_ib_)
		return false;

	GeometryGenerator::Geometry geo;
	GeometryGenerator::CreateFullscreenQuad(geo);

	std::vector<inputvertex::PosNormalTex::Vertex> vertices(geo.vertices.size());

	for (UINT i = 0; i < vertices.size(); ++i)
	{
		vertices[i].pos = geo.vertices[i].pos;
		vertices[i].normal = geo.vertices[i].normal;
		vertices[i].uv = geo.vertices[i].uv;
	}

	D3D11_BUFFER_DESC vb_desc{};
	vb_desc.ByteWidth = sizeof(inputvertex::PosNormalTex::Vertex) * vertices.size();
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_init{};
	vb_init.pSysMem = &vertices[0];
	HRESULT res = device->CreateBuffer(&vb_desc, &vb_init, &screen_quad_vb_);
	if (FAILED(res))
		return false;

	D3D11_BUFFER_DESC ib_desc{};
	ib_desc.ByteWidth = sizeof(UINT) * geo.indices.size();
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_init{};
	ib_init.pSysMem = &geo.indices[0];
	res = device->CreateBuffer(&ib_desc, &ib_init, &screen_quad_ib_);
	if (FAILED(res))
	{
		ReleaseCOM(screen_quad_vb_);
		return false;
	}

	return true;
}

void ba::Renderer::ReleaseScreenQuadBuffers()
{
	ReleaseCOM(screen_quad_vb_);
	ReleaseCOM(screen_quad_ib_);
}
