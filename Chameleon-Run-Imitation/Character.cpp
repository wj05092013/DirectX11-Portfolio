#include "stdafx.h"
#include "Character.h"

namespace ba
{
	Character::Character(ModelData* model_data, Timer* timer) :
		PhysicsModel(model_data, timer),
		acc_z_(0.0f),
		max_vel_z_(0.0f),
		jump_vel_(0.0f, 0.0f, 0.0f),
		b_jump_btn_enable_(true),
		jump_started_time_(0.0f),
		rest_jump_count_(2)
	{
	}

	Character::~Character()
	{
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

		if (static_cast<float>(timer_->get_total_time()) - jump_started_time_ > game::kJumpEnableTime)
		{
			b_jump_btn_enable_ = true;
		}
	}

	void Character::OnCollision(const collision::CollisionInfo& info)
	{
		PhysicsModel::OnCollision(info);

		rest_jump_count_ = game::kMaxJumpCount;
	}

	void Character::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
	{
		if (key_switch['A'])
		{
			model_data_->ChangeColor(ModelData::kYellow);
		}
		else
		{
			model_data_->ChangeColor(ModelData::kRed);
		}

		if (rest_jump_count_ > 0 && b_jump_btn_enable_ && key_pressed[VK_SPACE])
		{
			--rest_jump_count_;

			b_jump_btn_enable_ = false;

			// Do jump.
			AccumulateVelocity(jump_vel_);
			jump_started_time_ = static_cast<float>(timer_->get_total_time());
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
