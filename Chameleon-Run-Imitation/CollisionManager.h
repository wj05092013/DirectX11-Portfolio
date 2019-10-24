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

		};

		// 모든 Collider들의 생성/관리를 담당
		// DynamicCollider 기준으로 같은 영역 내의 모든 Collider들과 충돌 검사 수행
		class CollisionManager
		{
		private:
			CollisionManager();

		public:
			static CollisionManager& GetInstance();

			~CollisionManager();

			void CreateCollider(const Collider::ColliderDesc& desc, PhysicsModel* physics_model);

		private:
			// The key(type 'int') is index of a collision domain.
			std::multimap<int, Collider*> static_colliders_;

			std::vector<Collider*> dynamic_colliders_;
		};
	}
}
