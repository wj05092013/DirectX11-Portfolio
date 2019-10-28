#pragma once

/*
PCH: No
*/

#include "PhysicsModel.h"

namespace ba
{
	class Timer;
	class ModelData;

	class Character : public physics::PhysicsModel
	{
	public:
		Character(ModelData* model_data, Timer* timer);
		~Character() override;

		void Update() override;

		void OnCollision(const collision::CollisionInfo& info) override;

		void UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256]);

		//
		// Mutators
		//

		void set_acceleration_z(float acc_z);
		void set_max_velocity_z(float vel_z);
		void set_jump_velocity(const XMFLOAT3& vel);

	private:
		float acc_z_;
		float max_vel_z_;
		XMFLOAT3 jump_vel_;

		bool b_jump_btn_enable_;
		float jump_started_time_;
		int rest_jump_count_;
	};
}
