#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		SphereCollider::SphereCollider() :
			restitution_(0.0f)
		{
		}

		SphereCollider::~SphereCollider()
		{
		}

		void SphereCollider::UpdateDomainIndices()
		{
			// Transform the bounding sphere to world space.
			XMVECTOR center = XMVector3TransformCoord(XMLoadFloat3(&dx_bounding_sphere_.Center), model_->local_world());
			float center_z = XMVectorGetZ(center);

			int center_idx = static_cast<int>(center_z / kDomainSizeZ);
			int near_idx = static_cast<int>((center_z - dx_bounding_sphere_.Radius) / kDomainSizeZ);
			int far_idx = static_cast<int>((center_z + dx_bounding_sphere_.Radius) / kDomainSizeZ);

			center_domain_idx_ = center_idx;

			// The collider is spread over two domains.
			if (near_idx != far_idx)
			{
				if (near_idx == center_idx)
					spread_domain_idx_ = far_idx;
				else
					spread_domain_idx_ = near_idx;
			}
			// The collider is in just one domain.
			else
			{
				spread_domain_idx_ = center_idx;
			}
		}
	}
}
