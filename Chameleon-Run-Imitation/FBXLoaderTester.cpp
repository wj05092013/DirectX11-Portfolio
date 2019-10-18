#include "stdafx.h"
#include "Game.h"

const float ba::FBXLoaderTester::kShadowMapSize = 2048.0f;

const DirectX::XMVECTOR ba::FBXLoaderTester::kInitCamPos = XMVectorSet(0.0f, 1.0f, -15.0f, 1.0f);
const DirectX::XMVECTOR ba::FBXLoaderTester::kInitCamTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR ba::FBXLoaderTester::kInitCamUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

const float ba::FBXLoaderTester::kFovY = 0.25f * XM_PI;
const float ba::FBXLoaderTester::kNearZ = 0.1f;
const float ba::FBXLoaderTester::kFarZ = 1000.0f;

const float ba::FBXLoaderTester::kForwardMovingRate = 100.0f;
const float ba::FBXLoaderTester::kRightMovingRate = 100.0f;
const float ba::FBXLoaderTester::kUpperMovingRate = 100.0f;
const float ba::FBXLoaderTester::kRotationRate = 0.15f;

ba::FBXLoaderTester::FBXLoaderTester() :
	scene_bounds_{},

	wolf_model_(nullptr),
	
	last_mouse_pos_{}
{
}

ba::FBXLoaderTester::~FBXLoaderTester()
{
}

void ba::FBXLoaderTester::Render()
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

void ba::FBXLoaderTester::UpdateDirectX()
{
	cam_.UpdateViewMatrix();
	shadow_map_.BuildShadowTransform(lights_[0], scene_bounds_);
}

bool ba::FBXLoaderTester::OnResize()
{
	if (!Application::OnResize())
		return false;

	cam_.SetLens(kFovY, aspect_ratio(), kNearZ, kFarZ);
	ssao_map_.OnResize(client_width_, client_height_, kFovY, kFarZ);

	renderer_.SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);

	return true;
}

bool ba::FBXLoaderTester::InitDirectX()
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

	FBXLoader::GetInstance().Init();
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

void ba::FBXLoaderTester::ReleaseDirectX()
{
	renderer_.Release();
	shadow_map_.Release();
	ssao_map_.Release();
	ReleaseModels();

	FBXLoader::GetInstance().Release();
	TextureManager::GetInstance().Release();

	inputvertex::ReleaseAll();

	Application::ReleaseDirectX();
}

void ba::FBXLoaderTester::InitSceneBounds()
{
	scene_bounds_.center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scene_bounds_.radius = 100.0f;
}

void ba::FBXLoaderTester::InitLights()
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

bool ba::FBXLoaderTester::InitModels()
{
	// Create an wolf model and insert it into the 'model_instances'.
	//
	std::string file_name = "Model/Dragon 2.5_fbx.fbx";

	FBXLoader::FBXLoaderModel fbx_model;
	if (!FBXLoader::GetInstance().Load(file_name, fbx_model))
		return false;

	wolf_model_ = new Model;
	wolf_model_->Init(device_, fbx_model);

	ModelInstance wolf_instance;
	wolf_instance.model = wolf_model_;
	//wolf_instance.scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	model_instances_.push_back(wolf_instance);
	//__

	// Createa floor model.
	//
	std::vector<inputvertex::PosNormalTexTangent::Vertex> floor_vertices(6);

	floor_vertices[0].pos = XMFLOAT3(-50.0f, 0.0f, 50.0f);
	floor_vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	floor_vertices[0].uv = XMFLOAT2(0.0f, 0.0f);
	floor_vertices[0].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

	floor_vertices[1].pos = XMFLOAT3(50.0f, 0.0f, 50.0f);
	floor_vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	floor_vertices[1].uv = XMFLOAT2(1.0f, 0.0f);
	floor_vertices[1].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

	floor_vertices[2].pos = XMFLOAT3(-50.0f, 0.0f, -50.0f);
	floor_vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	floor_vertices[2].uv = XMFLOAT2(0.0f, 1.0f);
	floor_vertices[2].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
	
	floor_vertices[3].pos = XMFLOAT3(-50.0f, 0.0f, -50.0f);
	floor_vertices[3].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	floor_vertices[3].uv = XMFLOAT2(0.0f, 1.0f);
	floor_vertices[3].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

	floor_vertices[4].pos = XMFLOAT3(50.0f, 0.0f, 50.0f);
	floor_vertices[4].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	floor_vertices[4].uv = XMFLOAT2(1.0f, 0.0f);
	floor_vertices[4].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

	floor_vertices[5].pos = XMFLOAT3(50.0f, 0.0f, -50.0f);
	floor_vertices[5].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	floor_vertices[5].uv = XMFLOAT2(1.0f, 1.0f);
	floor_vertices[5].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

	// Simple material for the floor.
	light::Material material;
	material.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	material.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 4.0f);
	material.reflection = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	
	floor_model_ = new Model;
	floor_model_->meshes.resize(1);
	floor_model_->meshes[0].BuildVertexBuffer(device_, floor_vertices);
	floor_model_->meshes[0].set_materials(material);

	ModelInstance floor_instance;
	floor_instance.model = floor_model_;
	model_instances_.push_back(floor_instance);
	//__

	return true;
}

void ba::FBXLoaderTester::ReleaseModels()
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

void ba::FBXLoaderTester::UpdateOnKeyInput()
{
	// Control the camera.
	//
	if (key_pressed_['W'])
		cam_.MoveCameraZ(kForwardMovingRate * (float)timer_.get_delta_time());
	if (key_pressed_['S'])
		cam_.MoveCameraZ(-kForwardMovingRate * (float)timer_.get_delta_time());
	if (key_pressed_['D'])
		cam_.MoveCameraX(kRightMovingRate * (float)timer_.get_delta_time());
	if (key_pressed_['A'])
		cam_.MoveCameraX(-kRightMovingRate * (float)timer_.get_delta_time());
	if (key_pressed_['E'])
		cam_.MoveWorldY(kUpperMovingRate * (float)timer_.get_delta_time());
	if (key_pressed_['Q'])
		cam_.MoveWorldY(-kUpperMovingRate * (float)timer_.get_delta_time());
	//__
}

void ba::FBXLoaderTester::OnMouseMove(WPARAM w_par, int x, int y)
{
	if ((w_par & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(kRotationRate * static_cast<float>(x - last_mouse_pos_.x));
		float dy = XMConvertToRadians(kRotationRate * static_cast<float>(y - last_mouse_pos_.y));

		cam_.Pitch(dy);
		cam_.RotateWorldY(dx);
	}

	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::FBXLoaderTester::OnMouseLBtnDown(WPARAM w_par, int x, int y)
{
	SetCapture(main_wnd_);
	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::FBXLoaderTester::OnMouseRBtnDown(WPARAM w_par, int x, int y)
{
	SetCapture(main_wnd_);
	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::FBXLoaderTester::OnMouseLBtnUp(WPARAM w_par, int x, int y)
{
	ReleaseCapture();
}

void ba::FBXLoaderTester::OnMouseRBtnUp(WPARAM w_par, int x, int y)
{
	ReleaseCapture();
}
