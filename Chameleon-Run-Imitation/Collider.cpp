#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		Collider::Collider() :
			model_(nullptr),
			primitive_type_(kSphere),
			center_domain_idx_(0),
			spread_domain_idx_(0)
		{
		}

		Collider::~Collider()
		{
		}

		void Collider::OnCollision(const CollisionInfo& info)
		{
			model_->OnCollision(info);
		}
	}
}