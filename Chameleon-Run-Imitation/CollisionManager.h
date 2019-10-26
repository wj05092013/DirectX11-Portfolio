#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
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

			// If 'primitive_type' is 'kSphere', just one element of the array 'restitutions' will be used.
			//  If it is 'kAxisAlignedBox', the array must contain 6 elements.
			void CreateCollider(
				Collider::EPrimitiveType primitive_type,
				const float* restitutions,
				Model* model
			);

			void CheckCollision();

		private:
			// 'main' collider must be physics model's collider.
			//  'target' can be any type of model's collider.
			void Collision(Collider* main, Collider* target);

		private:
			// The key(type 'int') is index of a collision domain.
			//  This multimap just handles the pointers to the actual colliders.
			std::multimap<int, Collider*> static_model_colliders_;

			// This vector just handles the pointers to the actual colliders.
			std::vector<Collider*> physics_model_collider;

			// Each actual created colliders is included in this vector, only once.
			std::vector<Collider*> all_colliders_;
		};
	}
}
