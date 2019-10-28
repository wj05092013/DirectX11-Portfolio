#include "stdafx.h"
#include "Scene01.h"

namespace ba
{
	//
	// Helper Functions
	//

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

	//
	// Scene01 Members
	//

	scene01::Scene01::Scene01() :
		scene_state_(kPause),

		shadow_map_(nullptr),
		ssao_map_(nullptr),
		rt_camera_(nullptr),

		client_width_(0),
		client_height_(0),
		aspect_ratio_(0.0f),

		evb_change_rarely_{},
		evb_on_start_and_resize_{},
		evb_per_frame_{},

		bounding_sphere_{ XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0f },
		last_mouse_pos_{},

		sphere_(nullptr),
		red_box_(nullptr),
		ylw_box_(nullptr),
		character_(nullptr)
	{
	}

	scene01::Scene01::~Scene01()
	{
	}

	bool scene01::Scene01::Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer, int client_width, int client_height)
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

	bool scene01::Scene01::InitImpl()
	{
		// Initialize the directional lights.
		InitLights(lights_);

		// Initialize grahic components.
		//
		shadow_map_ = GraphicComponentManager::GetInstance().CreateComponent<ShadowMap>("SHADOW_MAP");
		ssao_map_ = GraphicComponentManager::GetInstance().CreateComponent<SSAOMap>("SSAO_MAP");
		rt_camera_ = GraphicComponentManager::GetInstance().CreateComponent<RotationalCamera>("RT_CAMERA");

		if (!shadow_map_->Init(device_, game::kShadowMapSize, game::kShadowMapSize))
			return false;
		if (!ssao_map_->Init(device_, dc_, client_width_, client_height_, kCamFovY, kCamFarZ))
			return false;

		shadow_map_->set_directional_light(lights_);
		shadow_map_->set_bounding_sphere(&bounding_sphere_);

		rt_camera_->Init(kRTCamInitDesc);
		rt_camera_->SetLens(kCamFovY, aspect_ratio_, kCamNearZ, kCamFarZ);
		rt_camera_->LookAt(kRTCamInitPos, kRTCamInitTarget, kRTCamInitUp);
		//__

		// Renderer setting.
		renderer_->set_shadow_map(shadow_map_);
		renderer_->set_ssao_map(ssao_map_);
		renderer_->set_camera(rt_camera_);

		// Set effect variables' values to be used for all time.
		evb_change_rarely_.directional_lights = lights_;
		evb_change_rarely_.fog_start = game::kFogStart;
		evb_change_rarely_.fog_range = game::kFogRange;
		evb_change_rarely_.fog_color = game::kFogColor;
		evb_change_rarely_.shadow_map_size = static_cast<float>(game::kShadowMapSize);
		evb_change_rarely_.to_tex = game::kToTex;
		renderer_->SetEffectVariablesChangeRarely(evb_change_rarely_);

		// Set effect variables' values to be used on start and resizing screen.
		renderer_->SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);

		collision::CollisionManager::GetInstance().Init();

		// Load models.
		if (!LoadModels())
			return false;

		return true;
	}

	void scene01::Scene01::Destroy()
	{
		scene_state_ = kFinish;

		DestroyModels();

		collision::CollisionManager::GetInstance().Destroy();

		renderer_->Destroy();

		GraphicComponentManager::GetInstance().Destroy();

		Scene::Destroy();
	}

	void scene01::Scene01::Render()
	{
		// Render on shadow map.
		renderer_->RenderShadowMap(models_, evb_per_frame_);

		// Render on normal-depth map.
		renderer_->RenderNormalDepthMap(models_, evb_per_frame_);

		// Build ssao map with the normal-depth map and blur it.
		ssao_map_->BuildSSAOMap(rt_camera_->proj());
		ssao_map_->BlurSSAOMap(4);

		// Render on normal render targets.
		renderer_->RenderScene(models_, evb_per_frame_);
	}

	void scene01::Scene01::Update()
	{
		// The z-value of the rotational camera's target is always same as that of the character.
		XMFLOAT3 pos = kRTCamInitTarget;
		pos.z = character_->translation_xf().z;
		rt_camera_->set_center_pos(pos);

		shadow_map_->BuildShadowTransform();
		rt_camera_->UpdateViewMatrix();

		if (scene_state_ == kRun)
		{
			for (UINT i = 0; i < models_.size(); ++i)
			{
				models_[i]->Update();
			}
			collision::CollisionManager::GetInstance().CheckCollision();
		}
	}

	bool scene01::Scene01::OnResize(int client_width, int client_height)
	{
		// Update the aspect ratio.
		client_width_ = client_width;
		client_height_ = client_height;
		aspect_ratio_ = aspect_ratio_ = static_cast<float>(client_width_) / static_cast<float>(client_height_);

		rt_camera_->SetLens(kCamFovY, aspect_ratio_, kCamNearZ, kCamFarZ);
		if (!ssao_map_->OnResize(client_width_, client_height_, kCamFovY, kCamFarZ))
		{
			Destroy();
			return false;
		}

		renderer_->SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);

		return true;
	}

	void scene01::Scene01::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
	{
		character_->UpdateOnKeyInput(key_pressed, key_switch);

		if (key_pressed['W'])
			rt_camera_->Approach(static_cast<float>(timer_->get_delta_time()));
		if (key_pressed['S'])
			rt_camera_->StepBack(static_cast<float>(timer_->get_delta_time()));

		if (key_switch[VK_ESCAPE])
			scene_state_ = kPause;
		else
			scene_state_ = kRun;

		if (scene_state_ == kRun)
		{
		}
	}

	void scene01::Scene01::OnMouseMove(HWND wnd, WPARAM w_par, int x, int y)
	{
		if ((w_par & MK_LBUTTON) != 0)
		{
			rt_camera_->Rotate({ x, y }, last_mouse_pos_);
		}

		last_mouse_pos_.x = x;
		last_mouse_pos_.y = y;
	}

	void scene01::Scene01::OnMouseLBtnDown(HWND wnd, WPARAM w_par, int x, int y)
	{
		SetCapture(wnd);
		last_mouse_pos_.x = x;
		last_mouse_pos_.y = y;
	}

	void scene01::Scene01::OnMouseRBtnDown(HWND wnd, WPARAM w_par, int x, int y)
	{
		SetCapture(wnd);
		last_mouse_pos_.x = x;
		last_mouse_pos_.y = y;
	}

	void scene01::Scene01::OnMouseLBtnUp(HWND wnd, WPARAM w_par, int x, int y)
	{
		ReleaseCapture();
	}

	void scene01::Scene01::OnMouseRBtnUp(HWND wnd, WPARAM w_par, int x, int y)
	{
		ReleaseCapture();
	}

	bool scene01::Scene01::LoadCharacter()
	{
		GeometryGenerator::Geometry geo;
		GeometryGenerator::CreateGeosphere(game::kSphereRadius, game::kSphereSubdivisionCount, geo);

		sphere_ = new ModelData;
		if (!sphere_->Init(device_, geo, game::kSphereLocalTransform, ModelData::kRed))
			return false;

		character_ = new Character(sphere_, timer_);

		character_->set_scale(game::kCharacterInitScale);
		character_->set_rotation(game::kCharacterInitRotation);
		character_->set_translation(game::kCharacterInitTranslation);
		character_->RecalculateWorldTransform();
		character_->set_mass(game::kCharacterInitMass);
		character_->SetGravity(game::kCharacterInitGravityEnable);
		character_->set_acceleration_z(game::kCharacterInitAccelerationZ);
		character_->set_max_velocity_z(game::kCharacterInitMaxVelocityZ);
		character_->set_jump_velocity(game::kCharacterJumpVelocity);

		if (!collision::CollisionManager::GetInstance().CreateCollider(collision::Collider::kSphere, &game::kCharacterInitRestitution, character_))
			return false;

		models_.push_back(character_);

		return true;
	}

	bool scene01::Scene01::LoadModels()
	{
		if (!LoadCharacter())
			return false;

		// Create a red box 'ModelData'.
		//
		GeometryGenerator::Geometry geo;
		GeometryGenerator::CreateBox(game::kBoxSize.x, game::kBoxSize.y, game::kBoxSize.z, geo);

		red_box_ = new ModelData;

		if (!red_box_->Init(device_, geo, game::kBoxLocalTransform, ModelData::kRed))
			return false;
		//__

		// Create a yellow box 'ModelData'.
		//
		ylw_box_ = new ModelData;

		if (!ylw_box_->Init(device_, geo, game::kBoxLocalTransform, ModelData::kYellow))
			return false;
		//__

		// Create a black box 'ModelData'.
		//
		blk_box_ = new ModelData;

		if (!blk_box_->Init(device_, geo, game::kBoxLocalTransform, ModelData::kBlack))
			return false;
		//__


		Model* model = new Model(red_box_, timer_);

		model->set_scale(kBox01InitScale);
		model->set_rotation(kBox01InitRotation);
		model->set_translation(kBox01InitTranslation);
		model->RecalculateWorldTransform();
		if (!collision::CollisionManager::GetInstance().CreateCollider(collision::Collider::kAxisAlignedBox, game::kBoxRestitutions, model))
			return false;

		models_.push_back(model);


		model = new Model(ylw_box_, timer_);

		model->set_scale(kBox02InitScale);
		model->set_rotation(kBox02InitRotation);
		model->set_translation(kBox02InitTranslation);
		model->RecalculateWorldTransform();
		if (!collision::CollisionManager::GetInstance().CreateCollider(collision::Collider::kAxisAlignedBox, game::kBoxRestitutions, model))
			return false;

		models_.push_back(model);


		model = new Model(blk_box_, timer_);

		model->set_scale(kBox03InitScale);
		model->set_rotation(kBox03InitRotation);
		model->set_translation(kBox03InitTranslation);
		model->RecalculateWorldTransform();
		if (!collision::CollisionManager::GetInstance().CreateCollider(collision::Collider::kAxisAlignedBox, game::kBoxRestitutions, model))
			return false;

		models_.push_back(model);


		model = new Model(red_box_, timer_);

		model->set_scale(kBox04InitScale);
		model->set_rotation(kBox04InitRotation);
		model->set_translation(kBox04InitTranslation);
		model->RecalculateWorldTransform();
		if (!collision::CollisionManager::GetInstance().CreateCollider(collision::Collider::kAxisAlignedBox, game::kBoxRestitutions, model))
			return false;

		models_.push_back(model);

		return true;
	}

	void scene01::Scene01::DestroyModels()
	{
		// Destroy model data.
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
		if (blk_box_)
		{
			delete blk_box_;
			blk_box_ = nullptr;
		}

		// Destroy models.
		for (UINT i = 0; i < models_.size(); ++i)
		{
			delete models_[i];
		}
		models_.clear();
		character_ = nullptr;
	}
}