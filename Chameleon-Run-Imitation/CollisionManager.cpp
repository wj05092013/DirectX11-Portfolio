#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		CollisionManager::CollisionManager()
		{
		}

		CollisionManager& CollisionManager::GetInstance()
		{
			static CollisionManager instance;
			return instance;
		}

		CollisionManager::~CollisionManager()
		{
		}

		void CollisionManager::CreateCollider(const Collider::ColliderDesc& desc, PhysicsModel* physics_model)
		{
			Collider* collider = new Collider;

			collider->desc_ = desc;

			int domain_idx_0 = -1;
			int domain_idx_1 = -1;

			collider->CalcDomainIndex();

			static_colliders_.insert({ domain_idx_0, collider });

			if (domain_idx_1 != -1)
				static_colliders_.insert({ domain_idx_1, collider });
		}
	}
}
