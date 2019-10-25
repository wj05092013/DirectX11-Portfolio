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
				d = -XMVectorGetX(XMVector3Dot(p0, normal[i]));			// d = dot(p0,n)
				out_planes[i].plane_eq = XMVectorSetW(normal[i], d);	// plane: ax + by + cz + d = 0
			}

			// Planes of negative direction.
			p0 = center - extents;
			for (int i = 3; i < 6; ++i)
			{
				d = -XMVectorGetX(XMVector3Dot(p0, -normal[i]));		// d = dot(p0,n)
				out_planes[i].plane_eq = XMVectorSetW(-normal[i], d);	// plane: ax + by + cz + d = 0
			}
		}

		// Calculate a vector from 'plane' to 'point', using the intersection point between the 'plane'
		//  and the line which passes the 'plane' perpendicularly.
		XMVECTOR CalcVectorFromTo(const XMVECTOR& plane, const XMVECTOR& point)
		{
			float k = -XMVectorGetX(XMVector3Dot(plane, point)) - XMVectorGetW(plane);
			return XMVectorSetW(-k * plane, 0.0f);
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

		Collider* CollisionManager::CreateCollider(
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

		void CollisionManager::CheckCollision()
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
			if (target->movement_type_ == Collider::kDynamic)
			{
				if (main->primitive_type_ == Collider::kSphere)
				{
					if (target->primitive_type_ == Collider::kSphere)
					{
						SphereCollider* main_sphere = reinterpret_cast<SphereCollider*>(main);
						SphereCollider* target_sphere = reinterpret_cast<SphereCollider*>(target);

						if (main_sphere->dx_bounding_sphere_.Intersects(target_sphere->dx_bounding_sphere_))
						{
							CollisionInfo collision_info;

							// Calculate restitution factor.
							float restitution = main_sphere->restitution_ * target_sphere->restitution_;
							collision_info.restitution = restitution;

							XMVECTOR center_to_center = XMLoadFloat3(&main_sphere->dx_bounding_sphere_.Center) - XMLoadFloat3(&target_sphere->dx_bounding_sphere_.Center);


							// Set main collider's normal vector.
							collision_info.normal = XMVector3Normalize(center_to_center);

							// Calculate the overlapped distance between the two colliders.
							collision_info.overlapped = main_sphere->dx_bounding_sphere_.Radius + target_sphere->dx_bounding_sphere_.Radius
								- XMVectorGetX(XMVector3Length(center_to_center));
							collision_info.overlapped *= 0.5f;

							// Deliver collision information to the main collider.
							main_sphere->OnCollision(collision_info);


							// Set target collider's normal vector.
							collision_info.normal = -collision_info.normal;

							// Deliver collision information to the target collider.
							target_sphere->OnCollision(collision_info);
						}
					}
					else if (target->primitive_type_ == Collider::kAxisAlignedBox)
					{
						// Do nothing.
					}
				}
				else if (main->primitive_type_ == Collider::kAxisAlignedBox)
				{
					if (target->primitive_type_ == Collider::kSphere)
					{
						// Do nothing.
					}
					else if (target->primitive_type_ == Collider::kAxisAlignedBox)
					{
						// Do nothing.
					}
				}
			}
			else if (target->movement_type_ == Collider::kStatic)
			{
				if (main->primitive_type_ == Collider::kSphere)
				{
					if (target->primitive_type_ == Collider::kSphere)
					{
						SphereCollider* main_sphere = reinterpret_cast<SphereCollider*>(main);
						SphereCollider* target_sphere = reinterpret_cast<SphereCollider*>(target);

						if (main_sphere->dx_bounding_sphere_.Intersects(target_sphere->dx_bounding_sphere_))
						{
							CollisionInfo collision_info;

							// Calculate restitution factor.
							float restitution = main_sphere->restitution_ * target_sphere->restitution_;
							collision_info.restitution = restitution;

							XMVECTOR center_to_center = XMLoadFloat3(&main_sphere->dx_bounding_sphere_.Center) - XMLoadFloat3(&target_sphere->dx_bounding_sphere_.Center);

							// Set main collider's normal vector.
							collision_info.normal = XMVector3Normalize(center_to_center);

							// Calculate the overlapped distance between the two colliders.
							collision_info.overlapped = main_sphere->dx_bounding_sphere_.Radius + target_sphere->dx_bounding_sphere_.Radius
								- XMVectorGetX(XMVector3Length(center_to_center));

							// Deliver collision information to the main collider.
							main_sphere->OnCollision(collision_info);
						}
					}
					else if (target->primitive_type_ == Collider::kAxisAlignedBox)
					{
						SphereCollider* main_sphere = reinterpret_cast<SphereCollider*>(main);
						AABBCollider* target_box = reinterpret_cast<AABBCollider*>(target);

						if (main_sphere->dx_bounding_sphere_.Intersects(target_box->dx_bounding_box_))
						{
							CollisionInfo collision_info;

							// Front planes' count cannot bigger than 3.
							int front_plane_indices[3];
							int front_plane_count = 0;

							float front_plane_dist[3];
							float dist_sum = 0.0f;
							float dist_square_sum = 0.0f;

							// Get planes' indices which the sphere's center is in front of.
							for (int i = 0; i < 6; ++i)
							{
								XMVECTOR to_center = CalcVectorFromTo(target_box->planes_[i].plane_eq, XMLoadFloat3(&main_sphere->dx_bounding_sphere_.Center));

								// The sphere's center point is in front of the plane.
								if (XMVectorGetX(XMVector3Dot(to_center, target_box->planes_[i].plane_eq)) >= 0.0f)
								{
									front_plane_indices[front_plane_count];
									
									float dist = XMVectorGetX(XMVector3Length(to_center));
									front_plane_dist[front_plane_count] = dist;

									dist_sum += dist;
									dist_square_sum += dist * dist;

									++front_plane_count;
								}
							}

							float result_restitution = 1.0f;
							collision_info.normal = XMVectorZero();

							// Interpolate normals, restitution factors.
							for (int i = 0; i < front_plane_count; ++i)
							{
								// 'front_plane_indices[i]' is current plane's index.

								float intp_factor = front_plane_dist[i] / dist_sum;

								collision_info.normal += intp_factor * target_box->planes_[front_plane_indices[i]].plane_eq;
								result_restitution += intp_factor * target_box->planes_[front_plane_indices[i]].restitution;
							}

							// Set collision info.
							collision_info.normal = XMVector3Normalize(collision_info.normal);
							collision_info.restitution = result_restitution;

							// Calculate the overlapped distance in the main spherer collider.
							collision_info.overlapped = main_sphere->dx_bounding_sphere_.Radius - std::sqrt(dist_square_sum);

							// Deliver collision information to the main collider.
							main_sphere->OnCollision(collision_info);
						}
					}
				}
				else if (main->primitive_type_ == Collider::kAxisAlignedBox)
				{
					if (target->primitive_type_ == Collider::kSphere)
					{
						// Do nothing.
					}
					else if (target->primitive_type_ == Collider::kAxisAlignedBox)
					{
						// Do nothing.
					}
				}
			}
		}
	}
}
