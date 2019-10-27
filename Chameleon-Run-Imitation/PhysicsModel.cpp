#include "stdafx.h"

namespace ba
{
	namespace physics
	{
		const XMVECTOR PhysicsModel::kGravityAcceleration = XMVectorSet(0.0f, -9.8f, 0.0f, 0.0f);

		PhysicsModel::PhysicsModel() :
			Model(kPhysics),
			mass_(0.0f),
			velocity_(XMVectorZero()),
			net_force_(XMVectorZero()),
			b_gravity_(false)
		{
		}

		PhysicsModel::~PhysicsModel()
		{
		}

		void PhysicsModel::OnCollision(const collision::CollisionInfo& info)
		{
			translation_ += info.overlapped * info.normal;
			velocity_ = velocity_ - (1.0f + info.restitution) * XMVector3Reflect(velocity_, info.normal);
		}

		void PhysicsModel::Update(float delta_time)
		{
			// Apply the Modified Euler Method.
			velocity_ += delta_time * net_force_ / mass_;
			translation_ += delta_time * velocity_;

			// Maintain homogeneous coordinates of translation.
			translation_ = XMVectorSetW(translation_, 1.0f);

			// Update the world transform of this model.
			UpdateWorldTransform();
		}

		void PhysicsModel::AccumulateVelocity(const XMVECTOR& velocity)
		{
			velocity_ += XMVectorSetW(velocity, 0.0f);
		}

		void PhysicsModel::AccumulateForce(const XMVECTOR& force)
		{
			net_force_ += XMVectorSetW(force, 0.0f);
		}

		void PhysicsModel::SetGravity(bool enable)
		{
			if (b_gravity_ == enable)
				return;

			b_gravity_ = enable;
			if (b_gravity_ == true)
				AccumulateForce(mass_ * kGravityAcceleration);
			else
				AccumulateForce(-mass_ * kGravityAcceleration);
		}

		float PhysicsModel::mass() const
		{
			return mass_;
		}

		const XMVECTOR& PhysicsModel::velocity() const
		{
			return velocity_;
		}

		const XMVECTOR& PhysicsModel::net_force() const
		{
			return net_force_;
		}

		void PhysicsModel::set_mass(float mass)
		{
			mass_ = mass;
		}
	}
}
