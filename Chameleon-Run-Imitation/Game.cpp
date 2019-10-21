#include "stdafx.h"
#include "Game.h"

const float ba::Game::kShadowMapSize = 2048.0f;

const DirectX::XMVECTOR ba::Game::kInitCamPos = XMVectorSet(0.0f, 1.0f, -15.0f, 1.0f);
const DirectX::XMVECTOR ba::Game::kInitCamTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR ba::Game::kInitCamUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

const float ba::Game::kFovY = 0.25f * XM_PI;
const float ba::Game::kNearZ = 0.1f;
const float ba::Game::kFarZ = 1000.0f;

const float ba::Game::kForwardMovingRate = 100.0f;
const float ba::Game::kRightMovingRate = 100.0f;
const float ba::Game::kUpperMovingRate = 100.0f;
const float ba::Game::kRotationRate = 0.15f;

ba::Game::Game() :
	scene_bounds_{},

	wolf_model_(nullptr),
	
	last_mouse_pos_{}
{
}

ba::Game::~Game()
{
}

void ba::Game::Render()
{
	// Render on shadow map.
	renderer_.RenderShadowMap(model_instances_, evb_per_frame_);

	// Render on normal-depth map;
	renderer_.RenderNormalDepthMap(model_instances_, evb_per_frame_);
	// Build ssao map with the normal-depth map and blur it.
	ssao_map_.BuildSSAOMap(cam_);
	ssao_map_.BlurSSAOMap(4);

	// Render on normal render targets.
	renderer_.RenderScene(model_instances_, evb_per_frame_);

	debug_screen_.set_srv(ssao_map_.normal_depth_map_srv());
	debug_screen_.Render(dc_);

	swap_chain_->Present(0U, 0U);
}

void ba::Game::UpdateDirectX()
{
	cam_.UpdateViewMatrix();
	shadow_map_.BuildShadowTransform();
}

bool ba::Game::OnResize()
{
	if (!Application::OnResize())
		return false;

	cam_.SetLens(kFovY, aspect_ratio(), kNearZ, kFarZ);
	ssao_map_.OnResize(client_width_, client_height_, kFovY, kFarZ);

	renderer_.SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);

	return true;
}

bool ba::Game::InitDirectX()
{
	if (!Application::InitDirectX())
		return false;

	if (!renderer_.Init(device_, dc_))
		return false;
	if (!inputvertex::InitAll(device_))
		return false;
	if (!shadow_map_.Init(device_, static_cast<float>(kShadowMapSize), static_cast<float>(kShadowMapSize)))
		return false;
	if (!ssao_map_.Init(device_, dc_, client_width_, client_height_, kFovY, kFarZ))
		return false;
	if (!debug_screen_.Init(device_))
		return false;
	debug_screen_.set_ndc_position_size(0.4f, -0.4f, 0.6f, 0.6f);

	cam_.LookAt(XMFLOAT3(0.0f, 5.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	cam_.SetLens(kFovY, aspect_ratio(), kNearZ, kFarZ);

	TextureManager::GetInstance().Init(device_);

	if (!InitModels())
		return false;

	InitSceneBounds();
	InitLights();

	// Set rendering components participating on all kind of rendering.
	Renderer::SceneRenderingComponents rendering_component;
	rendering_component.rtv = rtv_;
	rendering_component.dsv = dsv_;
	rendering_component.viewport = &viewport_;
	rendering_component.cam = &cam_;
	rendering_component.shadow_map = &shadow_map_;
	rendering_component.ssao_map = &ssao_map_;

	// Set effect variables' values to be used for all time.
	evb_change_rarely_.directional_lights = lights_;
	evb_change_rarely_.fog_start = 5.0f;
	evb_change_rarely_.fog_range = 20.0f;
	evb_change_rarely_.fog_color = color::kSilver;
	evb_change_rarely_.shadow_map_size = kShadowMapSize;
	evb_change_rarely_.to_tex = XMMATRIX(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	// Set effect variables' values to be used on application start and resize.
	//  Now, the structure has no members.

	renderer_.set_rendering_components(rendering_component);
	renderer_.SetEffectVariablesChangeRarely(evb_change_rarely_);
	renderer_.SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);

	return true;
}

void ba::Game::ReleaseDirectX()
{
	renderer_.Release();
	shadow_map_.Release();
	ssao_map_.Release();
	ReleaseModels();

	TextureManager::GetInstance().Release();

	inputvertex::ReleaseAll();

	Application::ReleaseDirectX();
}

void ba::Game::InitSceneBounds()
{
	scene_bounds_.center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scene_bounds_.radius = 100.0f;
}

void ba::Game::InitLights()
{
	lights_[0].ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	lights_[0].diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	lights_[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	lights_[0].direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	lights_[1].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lights_[1].diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	lights_[1].specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	lights_[1].direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	lights_[2].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lights_[2].diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	lights_[2].specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	lights_[2].direction = XMFLOAT3(0.0f, -0.707f, -0.707f);
}

bool ba::Game::InitModels()
{
	

	return true;
}

void ba::Game::ReleaseModels()
{
	if (wolf_model_)
	{
		delete wolf_model_;
		wolf_model_ = nullptr;
	}
	if (floor_model_)
	{
		delete floor_model_;
		floor_model_ = nullptr;
	}

	model_instances_.clear();
}

void ba::Game::UpdateOnKeyInput()
{
	// Control the camera.
	//
	if (key_pressed_['W'])
	if (key_pressed_['S'])
	if (key_pressed_['D'])
	if (key_pressed_['A'])
	if (key_pressed_['E'])
	if (key_pressed_['Q'])
	//__
}

void ba::Game::OnMouseMove(WPARAM w_par, int x, int y)
{
	if ((w_par & MK_LBUTTON) != 0)
	{
		
	}

	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::Game::OnMouseLBtnDown(WPARAM w_par, int x, int y)
{
	SetCapture(main_wnd_);
	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::Game::OnMouseRBtnDown(WPARAM w_par, int x, int y)
{
	SetCapture(main_wnd_);
	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::Game::OnMouseLBtnUp(WPARAM w_par, int x, int y)
{
	ReleaseCapture();
}

void ba::Game::OnMouseRBtnUp(WPARAM w_par, int x, int y)
{
	ReleaseCapture();
}
