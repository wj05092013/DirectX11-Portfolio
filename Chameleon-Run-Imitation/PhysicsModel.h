#pragma once

// Check here later.
#include "Collider.h"		
#include "CollisionManager.h"

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		class Collider;

		class PhysicsModel : public Model
		{
		public:
			PhysicsModel();
			~PhysicsModel() override;

			// If 'primitive_type' is 'kSphere', just one element of the array 'restitutions' will be used.
			//  If it is 'kAxisAlignedBox', the array must contain 6 elements.
			void CreateCollider(Collider::EMovementType movement_type, Collider::EPrimitiveType primitive_type, const float* restitutions);

			void OnCollision(const CollisionInfo& info);

			// Update this models position using velocity and net force.
			//  Also update the world transform of this model, automatically.
			//  Integration method is Modified Euler method.
			void Update(float delta_time);

			void AccumulateVelocity(const XMVECTOR& velocity);
			void AccumulateForce(const XMVECTOR& force);

			// Default is turned off.
			void ToggleGravity();


			//
			// Mutators
			//

			void set_mass(float mass);

		private:
			static const XMVECTOR kGravityAcceleration;

			Collider* collider_;

			float mass_;
			XMVECTOR velocity_;
			XMVECTOR net_force_;

			bool b_gravity_;
		};
	}
}
