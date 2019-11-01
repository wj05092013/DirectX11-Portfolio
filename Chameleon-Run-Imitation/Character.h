#pragma once

/*
PCH: No
*/

#include "PhysicsModel.h"
#include "Particle.h"

namespace ba
{
	class Timer;
	class ModelData;

	class Character : public physics::PhysicsModel
	{
	public:
		Character(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer);
		~Character() override;

		bool InitParticle(ID3D11Device* device);

		void Update() override;

		void OnCollision(const physics::CollisionInfo& info) override;

		void UpdateOnKeyInput(bool key_pressed[256], bool key_down[256], bool key_up[256]);

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

		float jump_started_time_;
		int rest_jump_count_;

		Particle* smoke_particle_;
	};
}
