#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		PhysicsModel::PhysicsModel()
		{
		}

		PhysicsModel::~PhysicsModel()
		{
		}

		void PhysicsModel::CreateCollider(Collider::EMovementType movement_type, Collider::EPrimitiveType primitive_type, const float* restitutions)
		{
			CollisionManager::GetInstance().CreateCollider(movement_type, primitive_type, restitutions, this);
		}

		void PhysicsModel::OnCollision()
		{
		}

		void PhysicsModel::Update()
		{
		}
	}
}
