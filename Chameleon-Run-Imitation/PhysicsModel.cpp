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

		void PhysicsModel::CreateCollider(Collider::EMovementType movement_type, Collider::EPrimitiveType primitive_type)
		{
			auto& vertices = model_data->mesh.vertices();
			UINT stride = model_data->mesh.vertex_stride;

			Collider::ColliderDesc desc;
			desc.movement_type_ = movement_type;
			desc.primitive_type_ = primitive_type;

			if (primitive_type == Collider::kSphere)
			{
				BoundingSphere::CreateFromPoints(desc.sphere, vertices.size(), reinterpret_cast<const XMFLOAT3*>(&vertices[0]), stride);
			}
			else if (primitive_type == Collider::kAxisAlignedBox)
			{
				BoundingBox::CreateFromPoints(desc.box, vertices.size(), reinterpret_cast<const XMFLOAT3*>(&vertices[0]), stride);
			}

			CollisionManager::GetInstance().CreateCollider(desc, this);
		}

		void PhysicsModel::OnCollision()
		{
		}

		void PhysicsModel::Update()
		{
		}
	}
}
