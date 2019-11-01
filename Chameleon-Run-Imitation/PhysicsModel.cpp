#include "stdafx.h"

namespace ba
{
	namespace physics
	{
		const XMVECTOR PhysicsModel::kGravityAcceleration = XMVectorSet(0.0f, -9.8f, 0.0f, 0.0f);

		PhysicsModel::PhysicsModel(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer) :
			Model(name, model_data, scene, timer, kPhysics),
			mass_(0.0f),
			velocity_(0.0f, 0.0f, 0.0f),
			net_force_(0.0f, 0.0f, 0.0f),
			b_gravity_(false)
		{
		}

		PhysicsModel::~PhysicsModel()
		{
		}

		void PhysicsModel::OnCollision(const physics::CollisionInfo& info)
		{
			Model::OnCollision(info);
		}

		void PhysicsModel::Update()
		{
			Model::Update();

			XMVECTOR velocity = velocity_xv();
			XMVECTOR translation = translation_xv();

			// Apply the Modified Euler Method.
			//
			velocity += static_cast<float>(timer_->get_delta_time()) * XMLoadFloat3(&net_force_) / mass_;

			translation += static_cast<float>(timer_->get_delta_time()) * velocity;
			//__
			
			set_velocity(velocity);
			set_translation(translation);

			// UpdateParticles the world transform of this model.
			RecalculateWorldTransform();
		}

		void PhysicsModel::AccumulateVelocity(const XMFLOAT3& velocity)
		{
			AccumulateVelocity(XMLoadFloat3(&velocity));
		}

		void PhysicsModel::AccumulateVelocity(const XMVECTOR& velocity)
		{
			set_velocity(velocity_xv() + velocity);
		}

		void PhysicsModel::AccumulateForce(const XMFLOAT3& force)
		{
			AccumulateForce(XMLoadFloat3(&force));
		}

		void PhysicsModel::AccumulateForce(const XMVECTOR& force)
		{
			XMStoreFloat3(&net_force_, net_force_xv() + force);
		}

		void PhysicsModel::EnableGravity(bool enable)
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

		const XMFLOAT3& PhysicsModel::velocity_xf() const
		{
			return velocity_;
		}

		const XMVECTOR PhysicsModel::velocity_xv() const
		{
			return XMLoadFloat3(&velocity_);
		}

		const XMFLOAT3& PhysicsModel::net_force_xf() const
		{
			return net_force_;
		}

		const XMVECTOR PhysicsModel::net_force_xv() const
		{
			return XMLoadFloat3(&net_force_);
		}

		void PhysicsModel::set_mass(float mass)
		{
			mass_ = mass;
		}

		void PhysicsModel::set_velocity(const XMFLOAT3& velocity)
		{
			velocity_ = velocity;
		}

		void PhysicsModel::set_velocity(const XMVECTOR& velocity)
		{
			XMStoreFloat3(&velocity_, velocity);
		}
	}
}
