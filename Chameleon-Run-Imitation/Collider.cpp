#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		Collider::Collider() :
			model_(nullptr),
			primitive_type_(kSphere),
			center_domain_idx_(0),
			spread_domain_idx_(0),
			transform_(XMMatrixIdentity())
		{
			
		}

		Collider::~Collider()
		{
		}

		void Collider::Update()
		{
			transform_ = model_->model_data->mesh.local_transform() * model_->world_transform;
		}

		void Collider::OnCollision(const CollisionInfo& info)
		{
			model_->OnCollision(info);
		}
	}
}