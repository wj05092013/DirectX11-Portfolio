#include "stdafx.h"

namespace ba
{
	namespace physics
	{
		//
		// Helpers
		//

		struct PhysicsSphere
		{
			float mass;
			float restitution;
			XMVECTOR center;
			XMVECTOR velocity;
		};

		void CalcAfterCollisionVelocity(const PhysicsSphere& s0, const PhysicsSphere& s1, XMVECTOR& out_v0, XMVECTOR& out_v1)
		{
			// Regard the vector from center of s0 to center of s1 as the normal vector of this collision.
			XMVECTOR n = XMVector3Normalize(s1.center - s0.center);

			// Get velocities in the direction of the normal vector.
			XMVECTOR v0_n = XMVector3Dot(s0.velocity, n) * n;
			XMVECTOR v1_n = XMVector3Dot(s1.velocity, n) * n;

			// Get velocities in the direction of the tangent vector of this collision.
			XMVECTOR v0_t = XMVectorSubtract(s0.velocity, v0_n);
			XMVECTOR v1_t = XMVectorSubtract(s1.velocity, v1_n);

			// Get the final coefficient of restitution.
			float e = s0.restitution * s1.restitution;

			// Caclulate the velocities in the direction of the normal vector after collision.
			//  (Using the law of conservation of momentum and the equation of the coefficient of restitution.)
			XMVECTOR out_v0_n = ((s0.mass - s1.mass * e) * v0_n + (1.0f + e) * s1.mass * v1_n) / (s0.mass + s1.mass);
			XMVECTOR out_v1_n = e * (v0_n - v1_n) + out_v0_n;

			// The result vector is sum of tangent and normal vecters.
			out_v0 = v0_t + out_v0_n;
			out_v1 = v1_t + out_v1_n;
		}

		void CalcAfterCollisionVelocity(const XMVECTOR& v, const XMVECTOR normal, float e, XMVECTOR& out_v)
		{
			out_v = -(e + 1) * XMVector3Dot(v, normal) * normal + v;
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
			static_model_colliders_.clear();
			physics_model_collider.clear();
		}

		bool CollisionManager::CreateCollider(
			Collider::EPrimitiveType primitive_type,
			const float* restitutions,
			Model* model
		)
		{
			if (restitutions == nullptr || model == nullptr || model->model_data() == nullptr)
				return false;

			Collider* collider = nullptr;

			auto& vertices = model->model_data()->mesh.vertices();
			UINT stride = model->model_data()->mesh.vertex_stride();

			if (primitive_type == Collider::kSphere)
			{
				SphereCollider* sphere_collider = new SphereCollider;

				// Create a bounding sphere for this SphereCollider.
				BoundingSphere::CreateFromPoints(sphere_collider->dx_bounding_sphere_, vertices.size(), reinterpret_cast<const XMFLOAT3*>(&vertices[0]), stride);

				// Set restitution factor.
				sphere_collider->restitution_ = *restitutions;

				collider = sphere_collider;
			}
			else if (primitive_type == Collider::kAxisAlignedBox)
			{
				AABBCollider* aabb_collider = new AABBCollider;

				// Create a bounding box for this AABBCollider.
				BoundingBox::CreateFromPoints(aabb_collider->dx_bounding_box_, vertices.size(), reinterpret_cast<const XMFLOAT3*>(&vertices[0]), stride);

				// Compute and store each planes' equations in local space.
				XMVECTOR plane_eq[6];
				mathhelper::SetAABBPlaneEquations(aabb_collider->dx_bounding_box_, plane_eq);
				for (int i = 0; i < 6; ++i)
				{
					aabb_collider->planes_[i].plane_eq = plane_eq[i];
				}
				
				// Set restitution factors for each planes.
				for (int i = 0; i < 6; ++i)
				{
					aabb_collider->planes_[i].restitution = restitutions[i];
				}

				collider = aabb_collider;
			}

			// Set member variables of 'Collider'.
			//
			collider->model_ = model;
			collider->primitive_type_ = primitive_type;

			//  Set 'center_domain_idx_', 'spread_domain_idx_'.
			//   The model must have intended transform matrix.
			collider->UpdateDomainIndices();
			//__

			if (model->model_type() == Model::kStatic)
			{
				static_model_colliders_.insert({ collider->center_domain_idx_, collider });

				if (collider->center_domain_idx_ != collider->spread_domain_idx_)
					static_model_colliders_.insert({ collider->spread_domain_idx_, collider });
			}
			else if (model->model_type() == Model::kPhysics)
			{
				physics_model_collider.push_back(collider);
			}

			all_colliders_.push_back(collider);

			return true;
		}

		void CollisionManager::CheckCollision()
		{
			// UpdateParticles domain indices of physics model's colliders.
			for (auto iter = physics_model_collider.begin(); iter != physics_model_collider.end(); ++iter)
			{
				(*iter)->UpdateDomainIndices();
			}

			// Perform collision check on all colliders of physics models.
			for (auto main_iter = physics_model_collider.begin(); main_iter != physics_model_collider.end(); ++main_iter)
			{
				Collider* main = *main_iter;

				// Check collision with other physics model colliders in the array, from the next one of the 'main' to the last one.
				for (auto target_iter = main_iter + 1; target_iter != physics_model_collider.end(); ++target_iter)
				{
					Collider* target = *target_iter;

					// Check collision only when the two colliders' centers are in the same domain or in an domain next to each other.
					//  Because all colliders cannot be larger than domain size.
					if (std::abs(main->center_domain_idx_ - target->center_domain_idx_) <= 1)
					{
						if (
							main->center_domain_idx_ == target->center_domain_idx_		// They are in the same domain.
							|| main->center_domain_idx_ == target->spread_domain_idx_		// 'target' collider is spread over two domains.
							|| main->spread_domain_idx_ == target->center_domain_idx_		// 'main' collider is spread over two domains.
							)
						{
							Collision(main, target);
						}
					}
				}

				// Check collision with static model colliders in the domain of which index is same as the 'main->center_domain_idx_'.
				auto lower_iter = static_model_colliders_.lower_bound(main->center_domain_idx_);
				if (lower_iter != static_model_colliders_.end())
				{
					auto upper_iter = static_model_colliders_.upper_bound(main->center_domain_idx_);

					for (auto target_iter = lower_iter; target_iter != upper_iter; ++target_iter)
					{
						Collider* target = target_iter->second;

						Collision(main, target);
					}
				}
				
				// Check collision with static model colliders in the domain of which index is same as the 'main->spread_domain_idx_'.
				if (main->center_domain_idx_ != main->spread_domain_idx_)
				{
					auto lower_iter = static_model_colliders_.lower_bound(main->spread_domain_idx_);
					if (lower_iter != static_model_colliders_.end())
					{
						auto upper_iter = static_model_colliders_.upper_bound(main->spread_domain_idx_);

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
			if (target->model_->model_type() == Model::kPhysics)
			{
				if (main->primitive_type_ == Collider::kSphere)
				{
					if (target->primitive_type_ == Collider::kSphere)
					{
						SphereCollider* main_collider = reinterpret_cast<SphereCollider*>(main);
						SphereCollider* target_collider = reinterpret_cast<SphereCollider*>(target);

						// Transform the bounding shapes to world space.
						//
						BoundingSphere main_bounding_sphere;
						main_collider->dx_bounding_sphere_.Transform(main_bounding_sphere, main_collider->model_->local_world());

						BoundingSphere target_bounding_sphere;
						target_collider->dx_bounding_sphere_.Transform(target_bounding_sphere, target_collider->model_->local_world());
						//__

						if (main_bounding_sphere.Intersects(target_bounding_sphere))
						{
							PhysicsModel* main_model = reinterpret_cast<PhysicsModel*>(main_collider->model_);
							PhysicsModel* target_model = reinterpret_cast<PhysicsModel*>(target_collider->model_);

							PhysicsSphere main_sphere{
								main_model->mass(), main_collider->restitution_,
								XMLoadFloat3(&main_bounding_sphere.Center), main_model->velocity_xv()
							};
							PhysicsSphere target_sphere{
								target_model->mass(), target_collider->restitution_,
								XMLoadFloat3(&target_bounding_sphere.Center), target_model->velocity_xv()
							};

							// Calculate the velocities of the models after this collision.
							XMVECTOR main_vel =XMVectorZero();
							XMVECTOR target_vel = XMVectorZero();
							CalcAfterCollisionVelocity(main_sphere, target_sphere, main_vel, target_vel);

							// UpdateParticles the velocities.
							main_model->set_velocity(main_vel);
							target_model->set_velocity(target_vel);

							XMVECTOR center_to_center = XMLoadFloat3(&target_bounding_sphere.Center) - XMLoadFloat3(&main_bounding_sphere.Center);
							XMVECTOR target_normal = XMVector3Normalize(center_to_center);
							XMVECTOR main_normal = -target_normal;

							// Calculate the overlapped distance between the two colliders
							//  and translate the models in the direction of the normal vector as half of the overlapped distance.
							float overlapped = main_bounding_sphere.Radius + target_bounding_sphere.Radius - XMVectorGetX(XMVector3Length(center_to_center));
							main_model->set_translation(main_model->translation_xv() + 0.5f * overlapped * main_normal);
							target_model->set_translation(target_model->translation_xv() + 0.5f * overlapped * target_normal);

							CollisionInfo collision_info;

							// Deliver collision information to the main collider.
							collision_info.opponent = target_collider;
							main_collider->OnCollision(collision_info);

							// Deliver collision information to the target collider.
							collision_info.opponent = main_collider;
							target_collider->OnCollision(collision_info);
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
			else if (target->model_->model_type() == Model::kStatic)
			{
				if (main->primitive_type_ == Collider::kSphere)
				{
					if (target->primitive_type_ == Collider::kSphere)
					{
						SphereCollider* main_collider = reinterpret_cast<SphereCollider*>(main);
						SphereCollider* target_collider = reinterpret_cast<SphereCollider*>(target);

						// Transform the bounding shapes to world space.
						//
						BoundingSphere main_bounding_sphere;
						main_collider->dx_bounding_sphere_.Transform(main_bounding_sphere, main_collider->model_->local_world());

						BoundingSphere target_bounding_sphere;
						target_collider->dx_bounding_sphere_.Transform(target_bounding_sphere, target_collider->model_->local_world());
						//__

						if (main_bounding_sphere.Intersects(target_bounding_sphere))
						{
							PhysicsModel* main_model = reinterpret_cast<PhysicsModel*>(main_collider->model_);

							// Calculate coefficient of restitution.
							float e = main_collider->restitution_ * target_collider->restitution_;

							// Regard the vector from center of target sphere to center of main sphere as the normal vector of this collision.
							XMVECTOR center_to_center = XMLoadFloat3(&main_bounding_sphere.Center) - XMLoadFloat3(&target_bounding_sphere.Center);
							XMVECTOR normal = XMVector3Normalize(center_to_center);

							// Velocity of main sphere.
							XMVECTOR vel = main_model->velocity_xv();
							
							// Calculate and update the velocity after this collision.
							CalcAfterCollisionVelocity(vel, normal, e, vel);
							main_model->set_velocity(vel);

							// Calculate the overlapped distance between the two colliders.
							float overlapped = main_bounding_sphere.Radius + target_bounding_sphere.Radius - XMVectorGetX(XMVector3Length(center_to_center));
							main_model->set_translation(main_model->translation_xv() + overlapped * normal);

							CollisionInfo collision_info;

							// Deliver collision information to the main collider.
							collision_info.opponent = target_collider;
							main_collider->OnCollision(collision_info);

							// Deliver collision information to the target collider.
							collision_info.opponent = main_collider;
							target_collider->OnCollision(collision_info);
						}
					}
					else if (target->primitive_type_ == Collider::kAxisAlignedBox)
					{
						SphereCollider* main_collider = reinterpret_cast<SphereCollider*>(main);
						AABBCollider* target_collider = reinterpret_cast<AABBCollider*>(target);

						// Transform the bounding shapes to world space.
						//
						BoundingSphere main_bounding_sphere;
						main_collider->dx_bounding_sphere_.Transform(main_bounding_sphere, main_collider->model_->local_world());

						BoundingBox target_bounding_box;
						target_collider->dx_bounding_box_.Transform(target_bounding_box, target_collider->model_->local_world());

						XMVECTOR target_planes[6];
						mathhelper::SetAABBPlaneEquations(target_bounding_box, target_planes);
						//__

						if (main_bounding_sphere.Intersects(target_bounding_box))
						{
							PhysicsModel* main_model = reinterpret_cast<PhysicsModel*>(main_collider->model_);

							// Front planes' count cannot bigger than 3.
							int front_plane_indices[3];
							int front_plane_count = 0;

							float front_plane_dist[3];
							float dist_sum = 0.0f;
							float dist_square_sum = 0.0f;

							// Get planes' indices which the sphere's center is in front of.
							for (int i = 0; i < 6; ++i)
							{
								XMVECTOR to_main_center = mathhelper::CalcVectorFromTo(target_planes[i], XMLoadFloat3(&main_bounding_sphere.Center));

								// The sphere's center point is in front of the plane.
								if (XMVectorGetX(XMVector3Dot(to_main_center, target_planes[i])) >= 0.0f)
								{
									front_plane_indices[front_plane_count] = i;
									
									float dist = XMVectorGetX(XMVector3Length(to_main_center));
									front_plane_dist[front_plane_count] = dist;

									dist_sum += dist;
									dist_square_sum += dist * dist;

									++front_plane_count;
								}
							}

							float target_restitution = 0.0f;
							XMVECTOR normal = XMVectorZero();

							// Interpolate normals, coefficients of restitution between multiple planes of the target box collider.
							for (int i = 0; i < front_plane_count; ++i)
							{
								// 'front_plane_indices[i]' is current plane's index.

								float intp_factor = front_plane_dist[i] / dist_sum;

								normal += intp_factor * target_planes[front_plane_indices[i]];
								target_restitution += intp_factor * target_collider->planes_[front_plane_indices[i]].restitution;
							}
							normal = XMVector3Normalize(normal);

							// Final coefficients of restitution of this collision.
							float e = main_collider->restitution_ * target_restitution;

							// Calculate and update the velocity after this collision.
							XMVECTOR vel = main_model->velocity_xv();
							CalcAfterCollisionVelocity(vel, normal, e, vel);
							main_model->set_velocity(vel);

							// Calculate the overlapped distance in the main sphere collider.
							float overlapped = main_bounding_sphere.Radius - std::sqrt(dist_square_sum);
							main_model->set_translation(main_model->translation_xv() + overlapped * normal);

							CollisionInfo collision_info;

							// Deliver collision information to the main collider.
							collision_info.opponent = target_collider;
							main_collider->OnCollision(collision_info);

							// Deliver collision information to the target collider.
							collision_info.opponent = main_collider;
							target_collider->OnCollision(collision_info);
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
