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

		void CollisionManager::Init()
		{
			Destroy();
		}

		void CollisionManager::Destroy()
		{
			for (auto iter = all_colliders_.begin(); iter != all_colliders_.end(); ++iter)
			{
				delete *iter;
			}
			all_colliders_.clear();
			static_colliders_.clear();
			dynamic_colliders_.clear();
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

			collider->physics_model_ = in_physics_model;
			collider->movement_type_ = movement_type;
			collider->primitive_type_ = primitive_type;

			collider->CalcDomainIndices();

			if (collider->movement_type_ == Collider::EMovementType::kStatic)
			{
				static_colliders_.insert({ collider->center_domain_idx, collider });

				if (collider->center_domain_idx != collider->spread_domain_idx)
					static_colliders_.insert({ collider->spread_domain_idx, collider });
			}
			else if (collider->movement_type_ == Collider::EMovementType::kDynamic)
			{
				dynamic_colliders_.push_back(collider);
			}

			all_colliders_.push_back(collider);
		}

		void CollisionManager::Collision()
		{
			// Perform collision check on all dynamic colliders.
			for (auto main_iter = dynamic_colliders_.begin(); main_iter != dynamic_colliders_.end(); ++main_iter)
			{
				Collider* main = *main_iter;

				// Check collision with other dynamic colliders in the array, from the next one of the 'main' to the last one.
				for (auto target_iter = main_iter + 1; target_iter != dynamic_colliders_.end(); ++target_iter)
				{
					Collider* target = *target_iter;

					// Check collision only when the two colliders' centers are in the same domain or in an domain next to each other.
					//  Because all colliders cannot be larger than domain size.
					if (std::abs(main->center_domain_idx - target->center_domain_idx) <= 1)
					{
						if (
							main->center_domain_idx == target->center_domain_idx		// They are in the same domain.
							|| main->center_domain_idx == target->spread_domain_idx		// 'target' collider is spread over two domains.
							|| main->spread_domain_idx == target->center_domain_idx		// 'main' collider is spread over two domains.
							)
						{
							Collision(main, target);
						}
					}
				}

				// Check collision with static colliders in the domain of which index is same as the 'main->center_domain_idx'.
				auto lower_iter = static_colliders_.lower_bound(main->center_domain_idx);
				if (lower_iter != static_colliders_.end())
				{
					auto upper_iter = static_colliders_.upper_bound(main->center_domain_idx);

					for (auto target_iter = lower_iter; target_iter != upper_iter; ++target_iter)
					{
						Collider* target = target_iter->second;

						Collision(main, target);
					}
				}
				
				// Check collision with static colliders in the domain of which index is same as the 'main->spread_domain_idx'.
				if (main->center_domain_idx != main->spread_domain_idx)
				{
					auto lower_iter = static_colliders_.lower_bound(main->spread_domain_idx);
					if (lower_iter != static_colliders_.end())
					{
						auto upper_iter = static_colliders_.upper_bound(main->spread_domain_idx);

						for (auto target_iter = lower_iter; target_iter != upper_iter; ++target_iter)
						{
							Collider* target = target_iter->second;

							Collision(main, target);
						}
					}
				}
			}
		}

		void CollisionManager::Collision(Collider* main, Collider* target)
		{
			// At the moment, perform nothing if any dynamic collider's primitive is box.
			if (
				main->primitive_type_ == Collider::kAxisAlignedBox
				|| (target->movement_type_ == Collider::kDynamic && target->primitive_type_ == Collider::kAxisAlignedBox)
				)
				return;

			// Dynamic collider VS Dynamic collider
			//  == Sphere collider VS Sphere collider
			if (target->movement_type_ == Collider::kDynamic)
			{
				SphereCollider* main_sphere = reinterpret_cast<SphereCollider*>(main);
				SphereCollider* target_sphere = reinterpret_cast<SphereCollider*>(target);

				CollisionInfo collision_info;

				// Calculate restitution factor.
				float restitution = main_sphere->restitution_ * target_sphere->restitution_;
				collision_info.restitution = restitution;

				// Set main collider's normal vector.
				XMVECTOR normal = XMLoadFloat3(&main_sphere->dx_bounding_sphere_.Center) - XMLoadFloat3(&target_sphere->dx_bounding_sphere_.Center);
				XMStoreFloat3(&collision_info.normal, normal);

				// Deliver collision information to the main collider.
				main_sphere->OnCollision(collision_info);

				// Set target collider's normal vector.
				XMStoreFloat3(&collision_info.normal, -normal);

				// Deliver collision information to the target collider.
				target_sphere->OnCollision(collision_info);
			}
			// Dynamic collider VS Static collider
			else
			{
				// Sphere collider VS Sphere collider
				if (target->primitive_type_ == Collider::kSphere)
				{

				}
				// Sphere collider VS AABB collider
				else
				{

				}
			}
		}
	}
}
