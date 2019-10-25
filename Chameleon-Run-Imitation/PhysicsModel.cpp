#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		PhysicsModel::PhysicsModel() :
			collider_(nullptr)
		{
		}

		PhysicsModel::~PhysicsModel()
		{
		}

		void PhysicsModel::CreateCollider(Collider::EMovementType movement_type, Collider::EPrimitiveType primitive_type, const float* restitutions)
		{
			collider_ = CollisionManager::GetInstance().CreateCollider(movement_type, primitive_type, restitutions, this);
		}

		void PhysicsModel::OnCollision(const CollisionInfo& info)
		{
			translation += info.overlapped * info.normal;
			velocity_ = velocity_ - (1.0f + info.restitution)* XMVector3Reflect(velocity_, info.normal);
		}

		void PhysicsModel::Update()
		{
			if (!collider_)
				return;

			if (collider_->movement_type() == Collider::kDynamic)
			{
				// 중력 적용

				// 움직임 갱신
			}
		}
	}
}
