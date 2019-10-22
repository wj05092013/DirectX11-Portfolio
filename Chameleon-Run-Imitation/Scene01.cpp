#include "stdafx.h"
#include "Scene01.h"

//
// Helper Functions
//

namespace ba
{
	void InitLights(light::DirectionalLight inout_lights[3])
	{
		inout_lights[0].ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		inout_lights[0].diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		inout_lights[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		inout_lights[0].direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

		inout_lights[1].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		inout_lights[1].diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
		inout_lights[1].specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
		inout_lights[1].direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

		inout_lights[2].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		inout_lights[2].diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		inout_lights[2].specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		inout_lights[2].direction = XMFLOAT3(0.0f, -0.707f, -0.707f);
	}
}


//
// Scene01 Members
//

const float ba::Scene01::kCamFovY = 0.25f * XM_PI;
const float ba::Scene01::kCamNearZ = 0.1f;
const float ba::Scene01::kCamFarZ = 1000.0f;
const DirectX::XMFLOAT3 ba::Scene01::kCamInitPos = DirectX::XMFLOAT3(0.0f, 1.0f, -15.0f);
const DirectX::XMFLOAT3 ba::Scene01::kCamInitTarget = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
const DirectX::XMFLOAT3 ba::Scene01::kCamInitUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
const ba::FreeViewpointCamera::FreeViewpointCameraDesc ba::Scene01::kCamInitDesc = { 100.0f, 100.0f, 100.0f, 0.15f };

const UINT ba::Scene01::kShadowMapSize = 2048U;

const float ba::Scene01::kFogStart = 5.0f;
const float ba::Scene01::kFogRange = 20.0f;
const DirectX::XMVECTOR ba::Scene01::kFogColor = ba::color::kSilver;

const DirectX::XMMATRIX ba::Scene01::kToTex = XMMATRIX(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f
);


ba::Scene01::Scene01()
{
}

ba::Scene01::~Scene01()
{
}

bool ba::Scene01::Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer, int client_width, int client_height)
{
	if (!Scene::Init(device, dc, renderer, timer, client_width, client_height))
		return false;

	client_width_ = client_width;
	client_height_ = client_height;
	aspect_ratio_ = static_cast<float>(client_width_) / static_cast<float>(client_height_);

	if (!InitImpl())
	{
		Destroy();
		return false;
	}

	return true;
}

void ba::Scene01::Destroy()
{
	GraphicComponentManager::GetInstance().DestroyComponent("SHADOW_MAP");
	GraphicComponentManager::GetInstance().DestroyComponent("SSAO_MAP");
	GraphicComponentManager::GetInstance().DestroyComponent("FREE_VIEW_CAMERA");

	Scene::Destroy();
}

void ba::Scene01::Render(IDXGISwapChain* swap_chain)
{
	// Render on shadow map.
	renderer_->RenderShadowMap(model_inst_, evb_per_frame_);

	// Render on normal-depth map;
	renderer_->RenderNormalDepthMap(model_inst_, evb_per_frame_);
	// Build ssao map with the normal-depth map and blur it.
	ssao_map_->BuildSSAOMap(camera_->proj());
	ssao_map_->BlurSSAOMap(4);

	// Render on normal render targets.
	renderer_->RenderScene(model_inst_, evb_per_frame_);
}

void ba::Scene01::Update()
{
	shadow_map_->BuildShadowTransform();
	camera_->UpdateViewMatrix();
}

bool ba::Scene01::OnResize(int client_width, int client_height)
{
	// Update the aspect ratio.
	client_width_ = client_width;
	client_height_ = client_height;
	aspect_ratio_ = aspect_ratio_ = static_cast<float>(client_width_) / static_cast<float>(client_height_);

	camera_->SetLens(kCamFovY, aspect_ratio_, kCamNearZ, kCamFarZ);
	if (!ssao_map_->OnResize(client_width_, client_height_, kCamFovY, kCamFarZ))
	{
		Destroy();
		return false;
	}

	renderer_->SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);
}

void ba::Scene01::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
{
}

void ba::Scene01::OnMouseMove(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseLBtnDown(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseRBtnDown(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseLBtnUp(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseRBtnUp(WPARAM w_par, int x, int y)
{
}

bool ba::Scene01::InitImpl()
{
	// Initialize the directional lights.
	InitLights(lights_);

	// Initialize grahic components.
	//
	shadow_map_ = GraphicComponentManager::GetInstance().CreateComponent<ShadowMap>("SHADOW_MAP");
	ssao_map_ = GraphicComponentManager::GetInstance().CreateComponent<SSAOMap>("SSAO_MAP");
	camera_ = GraphicComponentManager::GetInstance().CreateComponent<FreeViewpointCamera>("FREE_VIEW_CAMERA");

	if (!shadow_map_->Init(device_, static_cast<float>(kShadowMapSize), static_cast<float>(kShadowMapSize)))
		return false;
	if (!ssao_map_->Init(device_, dc_, client_width_, client_height_, kCamFovY, kCamFarZ))
		return false;

	shadow_map_->set_directional_light(lights_);
	shadow_map_->set_bounding_sphere();

	camera_->Init(kCamInitDesc);
	camera_->SetLens(kCamFovY, aspect_ratio_, kCamNearZ, kCamFarZ);
	camera_->LookAt(kCamInitPos, kCamInitTarget, kCamInitUp);
	//__

	// Renderer setting.
	renderer_->set_camera(camera_);
	renderer_->set_shadow_map(shadow_map_);
	renderer_->set_ssao_map(ssao_map_);

	// Set effect variables' values to be used for all time.
	evb_change_rarely_.directional_lights = lights_;
	evb_change_rarely_.fog_start = kFogStart;
	evb_change_rarely_.fog_range = kFogRange;
	evb_change_rarely_.fog_color = kFogColor;
	evb_change_rarely_.shadow_map_size = kShadowMapSize;
	evb_change_rarely_.to_tex = kToTex;
	renderer_->SetEffectVariablesChangeRarely(evb_change_rarely_);

	// Set effect variables' values to be used on start and resizing screen.
	renderer_->SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);
}
