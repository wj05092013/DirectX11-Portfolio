#include "stdafx.h"
#include "Constants.h"
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
		shadow_map_(nullptr),
		ssao_map_(nullptr),
		rt_camera_(nullptr),

		client_width_(0),
		client_height_(0),
		aspect_ratio_(0.0f),

		evb_change_rarely_{},
		evb_on_start_and_resize_{},
		evb_per_frame_{},

		bounding_sphere_{ XMFLOAT3(0.0f, 0.0f, 0.0f), game::kSceneBoundsRadius },
		last_mouse_pos_{},

		sphere_(nullptr),
		box_(nullptr),
		ground_(nullptr),
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

		return true;
	}

	bool scene01::Scene01::InitImpl()
	{
		// Initialize the directional lights.
		InitLights(lights_);

		// Create grahic components.
		shadow_map_ = GraphicComponentManager::GetInstance().CreateComponent<ShadowMap>("SHADOW_MAP");
		ssao_map_ = GraphicComponentManager::GetInstance().CreateComponent<SSAOMap>("SSAO_MAP");
		rt_camera_ = GraphicComponentManager::GetInstance().CreateComponent<RotationalCamera>("RT_CAMERA");

		// Initialize the ssao system.
		if (!ssao_map_->Init(device_, dc_, client_width_, client_height_, kCamFovY, kCamFarZ))
		{
			MessageBox(nullptr, L"ssao_map_->Init", nullptr, 0);
			return false;
		}

		// Initialize the shadow map.
		if (!shadow_map_->Init(device_, game::kShadowMapSize, game::kShadowMapSize))
		{
			MessageBox(nullptr, L"shadow_map_->Init", nullptr, 0);
			return false;
		}
		shadow_map_->set_directional_light(lights_);
		shadow_map_->set_bounding_sphere(&bounding_sphere_);

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

		// Initialize the camera and calculate projection matrix.
		rt_camera_->Init(kRTCamInitDesc);
		rt_camera_->SetLens(kCamFovY, aspect_ratio_, kCamNearZ, kCamFarZ);

		// Initialize ParticleManager.
		ParticleManager::GetInstance().Init(device_, dc_, timer_);

		// Initialize camera and models.
		if (!InitForRestart())
			return false;

		return true;
	}

	bool scene01::Scene01::InitForRestart()
	{
		// Calculate view matrix.
		rt_camera_->LookAt(kRTCamInitPos, kRTCamInitTarget, kRTCamInitUp);

		// Set effect variables' values to be used on start and resizing screen.
		renderer_->SetEffectVariablesOnStartAndResize(evb_on_start_and_resize_);

		// Initialize the collision system.
		physics::CollisionManager::GetInstance().Init();

		// Load models.
		if (!LoadModels())
			return false;

		// The z-value of the rotational camera's target is always same as that of the character.
		XMFLOAT3 center = kRTCamInitTarget;
		center.z = character_->translation_xf().z + kRTCamInitTargetDiffZ;
		rt_camera_->set_center_pos(center);

		// Reset emittion of particles.
		ParticleManager::GetInstance().ResetParticles();
		
		return true;
	}

	void scene01::Scene01::DestroyForRestart()
	{
		DestroyModels();
		physics::CollisionManager::GetInstance().Destroy();
	}

	void scene01::Scene01::Destroy()
	{
		ParticleManager::GetInstance().Destroy();

		DestroyModels();

		physics::CollisionManager::GetInstance().Destroy();

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

		// Draw particles.
		ParticleManager::GetInstance().DrawParticles(rt_camera_);
	}

	void scene01::Scene01::Update()
	{
		// The z-value of the rotational camera's target is always same as that of the character.
		XMFLOAT3 center = kRTCamInitTarget;
		center.z = character_->translation_xf().z + kRTCamInitTargetDiffZ;
		rt_camera_->UpdateCenterPos(center);

		// UpdateParticles the members for rendering.
		bounding_sphere_.center = rt_camera_->center_pos_xf();
		shadow_map_->BuildShadowTransform();
		rt_camera_->UpdateViewMatrix();

		// Manage the scene state.
		if (scene_state_ == kRunning)
		{
			// Update all models and particles.
			for (UINT i = 0; i < models_.size(); ++i)
			{
				models_[i]->Update();
			}
			physics::CollisionManager::GetInstance().CheckCollision();
			ParticleManager::GetInstance().UpdateParticles();

			// Game cleared.
			XMFLOAT3 pos = character_->translation_xf();
			if (pos.z > kClearPosZ && pos.y > kClearPosY)
				scene_state_ = kCleared;
		}
	}

	bool scene01::Scene01::OnResize(int client_width, int client_height)
	{
		// UpdateParticles the aspect ratio.
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

	void scene01::Scene01::UpdateOnKeyInput(bool key_pressed[256], bool key_down[256], bool key_up[256])
	{
		switch (scene_state_)
		{
		case ba::Scene::kRunning:
		{
			character_->UpdateOnKeyInput(key_pressed, key_down, key_up);

			if (key_down[VK_ESCAPE])
				scene_state_ = kPaused;

			break;
		}
		case ba::Scene::kLoaded:
		{
			if (key_down[VK_RETURN])
				scene_state_ = kRunning;

			break;
		}
		case ba::Scene::kPaused:
		{
			if (key_down[VK_ESCAPE])
				scene_state_ = kRunning;

			break;
		}
		case ba::Scene::kCleared:
		case ba::Scene::kGameOver:
		{
			if (key_down[VK_RETURN])
			{
				DestroyForRestart();
				InitForRestart();
				scene_state_ = kLoaded;
			}
			break;
		}
		}

		if (key_pressed['W'])
			rt_camera_->Approach(static_cast<float>(timer_->get_delta_time()));
		if (key_pressed['S'])
			rt_camera_->StepBack(static_cast<float>(timer_->get_delta_time()));
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
		character_ = new Character(kCharacterName, sphere_, this, timer_);

		// Model class.
		character_->set_scale(scene01::kCharacterInitScale);
		character_->set_rotation(scene01::kCharacterInitRotation);
		character_->set_translation(scene01::kCharacterInitTranslation);
		character_->RecalculateWorldTransform();
		character_->set_color_type(Model::kRed);

		// PhysicsModel class.
		character_->set_mass(game::kCharacterInitMass);
		character_->EnableGravity(game::kCharacterInitGravityEnable);

		// Character class.
		if (!character_->InitParticle(device_))
			return false;
		character_->set_acceleration_z(game::kCharacterInitAccelerationZ);
		character_->set_max_velocity_z(game::kCharacterInitMaxVelocityZ);
		character_->set_jump_velocity(game::kCharacterJumpVelocity);

		if (!physics::CollisionManager::GetInstance().CreateCollider(physics::Collider::kSphere, &game::kCharacterInitRestitution, character_))
			return false;

		models_.push_back(character_);

		return true;
	}

	bool scene01::Scene01::LoadModels()
	{
		geometrygenerator::Geometry sphere_geo;
		geometrygenerator::CreateGeosphere(game::kSphereRadius, game::kSphereSubdivisionCount, sphere_geo);

		geometrygenerator::Geometry box_geo;
		geometrygenerator::CreateBox(game::kBoxSize.x, game::kBoxSize.y, game::kBoxSize.z, box_geo);

		//
		// Create 'ModelData'
		//

		// Model data for sphere.
		sphere_ = new ModelData;
		if (!sphere_->Init(device_, sphere_geo, game::kSphereLocalTransform))
			return false;

		// Model data for box.
		box_ = new ModelData;
		if (!box_->Init(device_, box_geo, game::kBoxLocalTransform))
			return false;

		// Model data for ground.
		ground_ = new ModelData;
		if (!ground_->Init(device_, box_geo, game::kGroundLocalTransform))
			return false;

		if (!LoadCharacter())
			return false;

		//
		// Create Models
		//

		// Ground model.
		//
		Model* model = new Model(kGroundName, ground_, this, timer_);

		model->set_scale(kGroundInitScale);
		model->set_rotation(kGroundInitRotation);
		model->set_translation(kGroundInitTranslation);
		model->RecalculateWorldTransform();
		model->set_color_type(kGroundInitColorType);
		if (!physics::CollisionManager::GetInstance().CreateCollider(physics::Collider::kAxisAlignedBox, game::kBoxRestitutions, model))
			return false;

		models_.push_back(model);
		//__

		// Sphere models.
		//
		for (int i = 0; i < kSphereCount; ++i)
		{
			physics::PhysicsModel* model = new physics::PhysicsModel(kSphereNames[i], sphere_, this, timer_);

			// Model class.
			model->set_scale(kSphereInitScales[i]);
			model->set_rotation(kSphereInitRotations[i]);
			model->set_translation(kSphereInitTranslations[i]);
			model->RecalculateWorldTransform();
			model->set_color_type(kSphereInitColorType[i]);

			// PhysicsModel class.
			model->set_mass(1.0f);
			model->EnableGravity(true);

			if (!physics::CollisionManager::GetInstance().CreateCollider(physics::Collider::kSphere, &game::kSphereRestitution, model))
				return false;

			models_.push_back(model);
		}
		//__

		// Box models.
		//
		for (int i = 0; i < kBoxCount; ++i)
		{
			model = new Model(kBoxNames[i], box_, this, timer_);

			model->set_scale(kBoxInitScales[i]);
			model->set_rotation(kBoxInitRotations[i]);
			model->set_translation(kBoxInitTranslations[i]);
			model->RecalculateWorldTransform();

			model->set_color_type(kBoxInitColorType[i]);

			if (!physics::CollisionManager::GetInstance().CreateCollider(physics::Collider::kAxisAlignedBox, game::kBoxRestitutions, model))
				return false;

			models_.push_back(model);
		}
		//__

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
		if (box_)
		{
			delete box_;
			box_ = nullptr;
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