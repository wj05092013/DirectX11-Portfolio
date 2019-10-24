#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		Collider::Collider() :
			physics_model_(nullptr),
			movement_type_(kStatic),
			primitive_type_(kSphere),
			center_domain_idx(0),
			spread_domain_idx(0)
		{
			
		}

		Collider::~Collider()
		{
		}

		void Collider::Update()
		{
		}

		void Collider::OnCollision(const CollisionInfo& info)
		{
		}
	}
}