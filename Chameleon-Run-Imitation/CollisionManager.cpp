#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		//
		// Helper Functions
		//
		
		void SetPlaneEquations(const BoundingBox& box, AABBCollider::AABBPlane out_planes[6])
		{
			XMVECTOR normal[3] = {
				XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),	// Plane normal of positive x.
				XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),	// Plane normal of positive y.
				XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)		// Plane normal of positive z.
			};

			float d = 0.0f;

			XMVECTOR center = XMLoadFloat3(&box.Center);
			XMVECTOR extents = XMLoadFloat3(&box.Extents);

			// Planes of positive direction.
			XMVECTOR p0 = center + extents;
			for (int i = 0; i < 3; ++i)
			{
				d = -XMVectorGetX(XMVector3Dot(p0, normal[i]));		// d = dot(p0,n)
				out_planes[i].plane_eq = XMVectorSetW(normal[i], d);			// plane: ax + by + cz + d = 0
			}

			// Planes of negative direction.
			p0 = center - extents;
			for (int i = 3; i < 6; ++i)
			{
				d = -XMVectorGetX(XMVector3Dot(p0, -normal[i]));	// d = dot(p0,n)
				out_planes[i].plane_eq = XMVectorSetW(-normal[i], d);		// plane: ax + by + cz + d = 0
			}
		}


		//
		// CollisionManager Class
		//

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

		void CollisionManager::CreateCollider(
			Collider::EMovementType movement_type,
			Collider::EPrimitiveType primitive_type,
			const float* restitutions,
			PhysicsModel* in_physics_model
		)
		{
			Collider* collider = nullptr;

			auto& vertices = in_physics_model->model_data->mesh.vertices();
			UINT stride = in_physics_model->model_data->mesh.vertex_stride();

			if (primitive_type == Collider::kSphere)
			{
				SphereCollider* sphere_collider = new SphereCollider;

				// Create a bounding sphere for this SphereCollider.
				BoundingSphere::CreateFromPoints(sphere_collider->dx_bounding_sphere_, vertices.size(), reinterpret_cast<const XMFLOAT3*>(&vertices[0]), stride);

				// Set restitution factor.
				sphere_collider->restitution_ = restitutions[0];

				collider = sphere_collider;
			}
			else if (primitive_type == Collider::kAxisAlignedBox)
			{
				AABBCollider* aabb_collider = new AABBCollider;

				// Create a bounding box for this AABBCollider.
				BoundingBox::CreateFromPoints(aabb_collider->dx_bounding_box_, vertices.size(), reinterpret_cast<const XMFLOAT3*>(&vertices[0]), stride);

				// Compute and store each planes' plane equations.
				SetPlaneEquations(aabb_collider->dx_bounding_box_, aabb_collider->planes_);

				// Set restitution factors for each planes.
				for (int i = 0; i < 6; ++i)
				{
					aabb_collider->planes_[i].restitution = restitutions[i];
				}

				collider = aabb_collider;
			}

			collider->CalcDomainIndices();

			// 생성한 collider를 static, dynamic 배열에 넣기

			int domain_idx_0 = -1;
			int domain_idx_1 = -1;

			collider->CalcDomainIndices();

			static_colliders_.insert({ domain_idx_0, collider });

			if (domain_idx_1 != -1)
				static_colliders_.insert({ domain_idx_1, collider });
		}
	}
}
