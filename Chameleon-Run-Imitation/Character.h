#pragma once

/*
PCH: No
*/

#include "Model.h"
#include "PhysicsModel.h"

namespace ba
{
	class Character : public physics::PhysicsModel
	{
	public:
		Character(ModelData* model_data);
		~Character() override;

		void Update(float delta_time) override;

		void UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256]);

		//
		// Mutators
		//

		void set_acceleration_z(float acc_z);
		void set_max_velocity_z(float vel_z);
		void set_jump_velocity(const XMFLOAT3& vel);

	private:
		void Jump();

		float acc_z_;
		float max_vel_z_;
		XMFLOAT3 jump_vel_;
	};
}
