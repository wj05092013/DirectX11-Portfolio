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

			// 트랜스폼 갱신
			void Update();

		private:
			Collider* collider_;

			float mass_;
			XMVECTOR velocity_;
			XMVECTOR res_force_;
		};
	}
}
