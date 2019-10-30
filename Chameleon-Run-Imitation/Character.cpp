#include "stdafx.h"
#include "Constants.h"
#include "Character.h"

namespace ba
{
	Character::Character(const std::string& name, ModelData* model_data, Timer* timer) :
		PhysicsModel(name, model_data, timer),
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

		if (static_cast<float>(timer_->get_total_time()) - jump_started_time_ > scene01::kJumpEnableTime)
		{
			b_jump_btn_enable_ = true;
		}
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

		rest_jump_count_ = scene01::kMaxJumpCount;
	}

	void Character::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
	{
		if (key_switch['A'])
		{
			set_color_type(kYellow);
		}
		else
		{
			set_color_type(kRed);
		}

		if (rest_jump_count_ > 0 && b_jump_btn_enable_ && key_pressed[VK_SPACE])
		{
			--rest_jump_count_;

			b_jump_btn_enable_ = false;

			// Do jump.
			XMFLOAT3 vel = velocity_xf();
			set_velocity(XMFLOAT3(vel.x, jump_vel_.y, vel.z));
			//AccumulateVelocity(jump_vel_);
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
