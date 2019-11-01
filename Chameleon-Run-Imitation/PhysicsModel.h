#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace physics
	{
		class PhysicsModel : public Model
		{
		public:
			PhysicsModel(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer);
			~PhysicsModel() override;

			void OnCollision(const physics::CollisionInfo& info) override;

			// UpdateParticles this models position using velocity and net force.
			//  Also update the world transform of this model, automatically.
			//  Integration method used is Modified Euler method.
			void Update() override;

			void AccumulateVelocity(const XMFLOAT3& velocity);
			void AccumulateVelocity(const XMVECTOR& velocity);
			void AccumulateForce(const XMFLOAT3& force);
			void AccumulateForce(const XMVECTOR& force);

			// Default is gravity disabled.
			void EnableGravity(bool enable);

			float mass() const;
			const XMFLOAT3& velocity_xf() const;
			const XMVECTOR velocity_xv() const;
			const XMFLOAT3& net_force_xf() const;
			const XMVECTOR net_force_xv() const;

			void set_mass(float mass);
			void set_velocity(const XMFLOAT3& velocity);
			void set_velocity(const XMVECTOR& velocity);

		private:
			static const XMVECTOR kGravityAcceleration;

			float mass_;
			XMFLOAT3 velocity_;
			XMFLOAT3 net_force_;

			bool b_gravity_;
		};
	}
}
