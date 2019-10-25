#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		class Collider;
		enum Collider::EMovementType;
		enum Collider::EPrimitiveType;
		class PhysicsModel;

		struct CollisionInfo
		{
			float overlapped;
			float restitution;
			XMVECTOR normal;
		};

		class CollisionManager
		{
		private:
			CollisionManager();

		public:
			static CollisionManager& GetInstance();

			~CollisionManager();

			void Init();
			void Destroy();

			Collider* CreateCollider(
				Collider::EMovementType movement_type,
				Collider::EPrimitiveType primitive_type,
				const float* restitutions,
				PhysicsModel* in_physics_model
			);

			void CheckCollision();

		private:
			// 'main' collider must be dynamic collider.
			//  'target' can have any type of movement.
			void Collision(Collider* main, Collider* target);

		private:
			// The key(type 'int') is index of a collision domain.
			//  This multimap just handles the pointers to the actual colliders.
			std::multimap<int, Collider*> static_colliders_;

			// This vector just handles the pointers to the actual colliders.
			std::vector<Collider*> dynamic_colliders_;

			// Each actual created colliders is included in this vector, only once.
			std::vector<Collider*> all_colliders_;
		};
	}
}
