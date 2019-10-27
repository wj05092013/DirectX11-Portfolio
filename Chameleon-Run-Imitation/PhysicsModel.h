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
			PhysicsModel();
			~PhysicsModel() override;

			void OnCollision(const collision::CollisionInfo& info) override;

			// Update this models position using velocity and net force.
			//  Also update the world transform of this model, automatically.
			//  Integration method used is Modified Euler method.
			void Update(float delta_time);

			void AccumulateVelocity(const XMVECTOR& velocity);
			void AccumulateForce(const XMVECTOR& force);

			// Default is gravity disabled.
			void SetGravity(bool enable);

			float mass() const;
			const XMVECTOR& velocity() const;
			const XMVECTOR& net_force() const;

			void set_mass(float mass);

		private:
			static const XMVECTOR kGravityAcceleration;

			float mass_;
			XMVECTOR velocity_;
			XMVECTOR net_force_;

			bool b_gravity_;
		};
	}
}
