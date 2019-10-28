#include "stdafx.h"
#include "Character.h"

namespace ba
{
	Character::Character(ModelData* model_data, Timer* timer) :
		PhysicsModel(model_data, timer),
		acc_z_(0.0f),
		max_vel_z_(0.0f),
		jump_vel_(0.0f, 0.0f, 0.0f),
		b_jump_enable_(true),
		jump_started_time_(0.0f)
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

		if (static_cast<float>(timer_->get_total_time()) - jump_started_time_ > 0.1f)
		{
			b_jump_enable_ = true;
		}
	}

	void Character::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
	{
		if (b_jump_enable_ && key_pressed[VK_SPACE])
		{
			b_jump_enable_ = false;

			jump_started_time_ = static_cast<float>(timer_->get_total_time());

			Jump();
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

	void Character::Jump()
	{
		AccumulateVelocity(jump_vel_);
	}
}
