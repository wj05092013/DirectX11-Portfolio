#include "stdafx.h"
#include "Scene01.h"

//
// Helper Functions
//

namespace ba
{
	void InitLights(light::DirectionalLight inout_lights[3])
	{
		inout_lights[0].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		inout_lights[0].diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		inout_lights[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		XMStoreFloat3(&inout_lights[0].direction, XMVector3Normalize(XMVectorSet(-1.0f, -1.0f, -1.0f, 0.0f)));

		inout_lights[1].ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		inout_lights[1].diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		inout_lights[1].specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
		XMStoreFloat3(&inout_lights[1].direction, XMVector3Normalize(XMVectorSet(-1.0f, -1.0f, 1.0f, 0.0f)));

		inout_lights[2].ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		inout_lights[2].diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		inout_lights[2].specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat3(&inout_lights[1].direction, XMVector3Normalize(XMVectorSet(1.0f, -1.0f, 1.0f, 0.0f)));
	}
}


//
// Scene01 Members
//

const float ba::Scene01::kCamFovY = 0.25f * XM_PI;
const float ba::Scene01::kCamNearZ = 0.1f;
const float ba::Scene01::kCamFarZ = 1000.0f;
const DirectX::XMFLOAT3 ba::Scene01::kCamInitPos = DirectX::XMFLOAT3(10.0f, 10.0f, -10.0f);
const DirectX::XMFLOAT3 ba::Scene01::kCamInitTarget = DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f);
const DirectX::XMFLOAT3 ba::Scene01::kCamInitUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
const ba::FreeViewpointCamera::FreeViewpointCameraDesc ba::Scene01::kCamInitDesc = { 20.0f, 20.0f, 20.0f, 0.1f };

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


ba::Scene01::Scene01() :
	scene_state_(kPause),

	shadow_map_(nullptr),
	ssao_map_(nullptr),
	camera_(nullptr),

	client_width_(0),
	client_height_(0),
	aspect_ratio_(0.0f),

	evb_change_rarely_{},
	evb_on_start_and_resize_{},
	evb_per_frame_{},

	bounding_sphere_{ XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0f },
	last_mouse_pos_{},

	character_(nullptr),
	red_box_(nullptr),
	ylw_box_(nullptr)
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

	scene_state_ = kRun;

	return true;
}

void ba::Scene01::Destroy()
{
	scene_state_ = kFinish;

	DestroyModels();

	renderer_->Destroy();

	GraphicComponentManager::GetInstance().DestroyComponent("SHADOW_MAP");
	GraphicComponentManager::GetInstance().DestroyComponent("SSAO_MAP");
	GraphicComponentManager::GetInstance().DestroyComponent("FREE_VIEW_CAMERA");

	Scene::Destroy();
}

void ba::Scene01::Render()
{
	// Render on shadow map.
	renderer_->RenderShadowMap(model_inst_, evb_per_frame_);

	// Render on normal-depth map.
	renderer_->RenderNormalDepthMap(model_inst_, evb_per_frame_);

	// Build ssao map with the normal-depth map and blur it.
	ssao_map_->BuildSSAOMap(camera_->proj());
	ssao_map_->BlurSSAOMap(4);

	// Render on normal render targets.
	renderer_->RenderScene(model_inst_, evb_per_frame_);
}

void ba::Scene01::Update()
{
	if (scene_state_ == kRun)
	{
		shadow_map_->BuildShadowTransform();
		camera_->UpdateViewMatrix();

		// Update the character.
		//
		
		//__
	}
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

	return true;
}

void ba::Scene01::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
{
	if (key_switch[VK_ESCAPE])
		scene_state_ = kPause;
	else
		scene_state_ = kRun;

	if (scene_state_ == kRun)
	{
		// Control the character.
		//
		character_->UpdateOnKeyInput(key_pressed, key_switch);
		//__

		// Camera control for debugging.
		//
		if (key_pressed['W'])
			camera_->MoveFront(static_cast<float>(timer_->get_delta_time()));
		if (key_pressed['S'])
			camera_->MoveBack(static_cast<float>(timer_->get_delta_time()));
		if (key_pressed['D'])
			camera_->MoveRight(static_cast<float>(timer_->get_delta_time()));
		if (key_pressed['A'])
			camera_->MoveLeft(static_cast<float>(timer_->get_delta_time()));
		if (key_pressed['E'])
			camera_->MoveUp(static_cast<float>(timer_->get_delta_time()));
		if (key_pressed['Q'])
			camera_->MoveDown(static_cast<float>(timer_->get_delta_time()));
		//__
	}
}

void ba::Scene01::OnMouseMove(HWND wnd, WPARAM w_par, int x, int y)
{
	if (scene_state_ == kRun)
	{
		if ((w_par & MK_RBUTTON) != 0)
		{
			camera_->Rotate({ x, y }, last_mouse_pos_);
		}

		last_mouse_pos_.x = x;
		last_mouse_pos_.y = y;
	}
}

void ba::Scene01::OnMouseLBtnDown(HWND wnd, WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseRBtnDown(HWND wnd, WPARAM w_par, int x, int y)
{
	if (scene_state_ == kRun)
	{
		SetCapture(wnd);
		last_mouse_pos_.x = x;
		last_mouse_pos_.y = y;
	}
}

void ba::Scene01::OnMouseLBtnUp(HWND wnd, WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseRBtnUp(HWND wnd, WPARAM w_par, int x, int y)
{
	ReleaseCapture();
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

	if (!shadow_map_->Init(device_, kShadowMapSize, kShadowMapSize))
		return false;
	if (!ssao_map_->Init(device_, dc_, client_width_, client_height_, kCamFovY, kCamFarZ))
		return false;

	shadow_map_->set_directional_light(lights_);
	shadow_map_->set_bounding_sphere(&bounding_sphere_);

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

	// Load models.
	if (!LoadModels())
		return false;

	return true;
}

bool ba::Scene01::LoadCharacter()
{
	GeometryGenerator::Geometry geo;
	GeometryGenerator::CreateGeosphere(0.5f, 3, geo);

	XMMATRIX local_transform = XMMatrixTranslation(0.5f, 0.5f, 0.5f);

	light::Material material;
	material.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 0.0f, 0.501960f, 1.0f);
	material.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f);
	material.reflection = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	sphere_ = new ModelData;
	if (!sphere_->Init(device_, geo, local_transform, material))
		return false;
	
	Character character_inst;

	character_inst.model_data = sphere_;
	character_inst.scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	character_inst.rotation = XMQuaternionIdentity();
	character_inst.translation = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	character_inst.CalcWorldTransform();

	model_inst_.push_back(character_inst);
	character_ = dynamic_cast<Character*>(&model_inst_[model_inst_.size() - 1]);

	return true;
}

bool ba::Scene01::LoadModels()
{
	LoadCharacter();

	XMVECTOR identity_quat = XMQuaternionIdentity();
	XMMATRIX local_transform;

	// Create a red box model_data.
	//
	GeometryGenerator::Geometry geo;
	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, geo);
	local_transform = XMMatrixTranslation(0.5f, 0.5f, 0.5f);

	light::Material material;
	material.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 0.0f, 0.501960f, 1.0f);
	material.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f);
	material.reflection = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	red_box_ = new ModelData;
	if (!red_box_->Init(device_, geo, local_transform, material))
		return false;
	//__

	// Create a yellow box model_data.
	//
	material.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 0.925490f, 0.015686f, 1.0f);
	material.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f);
	material.reflection = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	ylw_box_ = new ModelData;
	if (!ylw_box_->Init(device_, geo, local_transform, material))
		return false;
	//__

	Model instance;
	
	instance.model_data = red_box_;
	instance.scale = XMVectorSet(1.0f, 1.0f, 6.0f, 0.0f);
	instance.rotation = identity_quat;
	instance.translation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	instance.CalcWorldTransform();
	model_inst_.push_back(instance);

	instance.model_data = ylw_box_;
	instance.scale = XMVectorSet(1.0f, 1.0f, 3.0f, 0.0f);
	instance.rotation = identity_quat;
	instance.translation = XMVectorSet(0.0f, 1.0f, 9.0f, 0.0f);
	instance.CalcWorldTransform();
	model_inst_.push_back(instance);

	instance.model_data = ylw_box_;
	instance.scale = XMVectorSet(1.0f, 1.0f, 3.0f, 0.0f);
	instance.rotation = identity_quat;
	instance.translation = XMVectorSet(0.0f, 1.0f, 15.0f, 0.0f);
	instance.CalcWorldTransform();
	model_inst_.push_back(instance);

	instance.model_data = red_box_;
	instance.scale = XMVectorSet(1.0f, 1.0f, 2.0f, 0.0f);
	instance.rotation = identity_quat;
	instance.translation = XMVectorSet(0.0f, 0.0f, 20.0f, 0.0f);
	instance.CalcWorldTransform();
	model_inst_.push_back(instance);

	return true;
}

void ba::Scene01::DestroyModels()
{
	if (sphere_)
	{
		delete sphere_;
		sphere_ = nullptr;
	}
	if (red_box_)
	{
		delete red_box_;
		red_box_ = nullptr;
	}
	if (ylw_box_)
	{
		delete ylw_box_;
		ylw_box_ = nullptr;
	}

	character_ = nullptr;

	model_inst_.clear();
}
