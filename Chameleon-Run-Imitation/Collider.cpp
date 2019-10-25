#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		Collider::Collider() :
			physics_model_(nullptr),
			movement_type_(kStatic),
			primitive_type_(kSphere),
			center_domain_idx_(0),
			spread_domain_idx_(0)
		{
			
		}

		Collider::~Collider()
		{
		}

		void Collider::Update()
		{
			transform_ = physics_model_->model_data->mesh.local_transform() * physics_model_->world_transform;
		}

		void Collider::OnCollision(const CollisionInfo& info)
		{
			physics_model_->OnCollision(info);
		}

		Collider::EMovementType Collider::movement_type() const
		{
			return movement_type_;
		}

		Collider::EPrimitiveType Collider::primitive_type() const
		{
			return primitive_type_;
		}
	}
}