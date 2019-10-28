#include "stdafx.h"
#include "Character.h"

namespace ba
{
	Character::Character(ModelData* model_data) :
		PhysicsModel(model_data),
		acc_z_(0.0f),
		max_vel_z_(0.0f),
		jump_vel_(0.0f, 0.0f, 0.0f)
	{
	}

	Character::~Character()
	{
	}

	void Character::Update(float delta_time)
	{
		PhysicsModel::Update(delta_time);

		XMFLOAT3 vel = velocity_xf();

		vel.z += delta_time * acc_z_;

		if (vel.z > max_vel_z_)
		{
			vel.z = max_vel_z_;
		}

		set_velocity(vel);
	}

	void Character::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
	{
		if (key_pressed[VK_SPACE])
		{

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
