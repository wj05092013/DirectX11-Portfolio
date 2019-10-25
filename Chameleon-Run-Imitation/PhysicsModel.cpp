#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		const XMVECTOR PhysicsModel::kGravityAcceleration = XMVectorSet(0.0f, -9.8f, 0.0f, 0.0f);

		PhysicsModel::PhysicsModel() :
			collider_(nullptr),
			mass_(0.0f),
			velocity_(XMVectorZero()),
			net_force_(XMVectorZero()),
			b_gravity_(false)
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
			velocity_ = velocity_ - (1.0f + info.restitution) * XMVector3Reflect(velocity_, info.normal);
		}

		void PhysicsModel::Update(float delta_time)
		{
			if (!collider_)
				return;

			if (collider_->movement_type() == Collider::kDynamic)
			{
				// Apply the Modified Euler Method.
				velocity_ += delta_time * net_force_ / mass_;
				translation += delta_time * velocity_;

				// Maintain homogeneous coordinates of translation.
				translation = XMVectorSetW(translation, 1.0f);
			}

			// Update the world transform of this model.
			CalcWorldTransform();
		}

		void PhysicsModel::AccumulateVelocity(const XMVECTOR& velocity)
		{
			velocity_ += XMVectorSetW(velocity, 0.0f);
		}

		void PhysicsModel::AccumulateForce(const XMVECTOR& force)
		{
			net_force_ += XMVectorSetW(force, 0.0f);
		}

		void PhysicsModel::ToggleGravity()
		{
			b_gravity_ = !b_gravity_;

			if (b_gravity_ == true)
				AccumulateForce(mass_ * kGravityAcceleration);
		}

		void PhysicsModel::set_mass(float mass)
		{
			mass_ = mass;
		}
	}
}
