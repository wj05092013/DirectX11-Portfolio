#include "stdafx.h"
#include "Constants.h"
#include "Character.h"

namespace ba
{
	Character::Character(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer) :
		PhysicsModel(name, model_data, scene, timer),
		acc_z_(0.0f),
		max_vel_z_(0.0f),
		jump_vel_(0.0f, 0.0f, 0.0f),
		jump_started_time_(0.0f),
		rest_jump_count_(2),
		smoke_particle_(nullptr)
	{
	}

	Character::~Character()
	{
		ParticleManager::GetInstance().DestroyParticle(smoke_particle_);
	}

	bool Character::InitParticle(ID3D11Device* device)
	{
		if (smoke_particle_)
			return false;

		std::vector<std::wstring> file_names;
		game::GetCharacterParticleFileNames(file_names);

		ID3D11ShaderResourceView* tex_arr_srv = nullptr;
		ID3D11ShaderResourceView* rand_tex_srv = nullptr;
		if (!TextureManager::GetInstance().CreateTex2DArrSRV(file_names, &tex_arr_srv))
		{
			MessageBox(nullptr, L"fail: CreateTex2DArrSRV", nullptr, 0);
			return false;
		}
		if (!TextureManager::GetInstance().CreateRandomTex1DSRV(game::kRandomTexSize, &rand_tex_srv))
		{
			MessageBox(nullptr, L"fail: CreateRandomTex1DSRV", nullptr, 0);
			return false;
		}

		if(!ParticleManager::GetInstance().CreateParticle(
			&effects::kSmokeParticleEffect, game::kCharacterMaxParticleCount,
			tex_arr_srv, file_names.size(), rand_tex_srv, &smoke_particle_
		))
		{
			MessageBox(nullptr, L"fail: CreateParticle", nullptr, 0);
			return false;
		}

		return true;
	}

	void Character::Update()
	{
		PhysicsModel::Update();

		// Accelerate character.
		//
		XMFLOAT3 vel = velocity_xf();
		vel.z += static_cast<float>(timer_->get_delta_time()) * acc_z_;
		if (vel.z > max_vel_z_)
		{
			vel.z = max_vel_z_;
		}

		set_velocity(vel);
		//__

		// UpdateParticles particle's emittion position.
		smoke_particle_->set_emit_pos_xv(translation_xv() + XMLoadFloat3(&game::kCharacterParticleEmitPos));
	}

	void Character::OnCollision(const physics::CollisionInfo& info)
	{
		PhysicsModel::OnCollision(info);

		// Set the x-value of translation and velocity to 0.
		//
		XMFLOAT3 translation = translation_xf();
		XMFLOAT3 velocity = velocity_xf();

		translation.x = 0.0f;
		velocity.x = 0.0f;
		set_translation(translation);
		set_velocity(velocity);
		//__

		if (info.opponent->model()->color_type() == color_type_)
		{
			// Restore the jump count if the character collided with a box having same color.
			rest_jump_count_ = game::kCharacterMaxJumpCount;
		}
		else
		{
			// Destroy this character.
			scene_->set_scene_state(Scene::kGameOver);
		}
	}

	void Character::UpdateOnKeyInput(bool key_pressed[256], bool key_down[256], bool key_up[256])
	{
		 // Change color.
		if (key_down['A'])
		{
			if(color_type_ == kYellow)
				set_color_type(kRed);
			else
				set_color_type(kYellow);
		}

		// Do jump.
		if (rest_jump_count_ > 0 && key_down[VK_SPACE])
		{
			--rest_jump_count_;

			XMFLOAT3 vel = velocity_xf();
			set_velocity(XMFLOAT3(vel.x, jump_vel_.y, vel.z));
		}
	}

	void Character::set_acceleration_z(float acc_z)
	{
		acc_z_ = acc_z;
	}

	void Character::set_max_velocity_z(float vel_z)
	{
		max_vel_z_ = vel_z;
	}

	void Character::set_jump_velocity(const XMFLOAT3& vel)
	{
		jump_vel_ = vel;
	}
}
