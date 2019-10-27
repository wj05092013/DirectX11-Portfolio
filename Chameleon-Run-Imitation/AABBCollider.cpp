#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		AABBCollider::AABBCollider() :
			planes_{}
		{
		}

		AABBCollider::~AABBCollider()
		{
		}

		void AABBCollider::UpdateDomainIndices()
		{
			// Transform the bounding box to world space.
			XMVECTOR center = XMVector3TransformCoord(XMLoadFloat3(&dx_bounding_box_.Center), model_->local_world);
			XMVECTOR extents = XMVector3Transform(XMLoadFloat3(&dx_bounding_box_.Extents), model_->local_world);
			float center_z = XMVectorGetZ(center);
			float extents_z = XMVectorGetZ(extents);

			int center_idx = static_cast<int>(center_z / kDomainSizeZ);
			int near_idx = static_cast<int>((center_z - extents_z) / kDomainSizeZ);
			int far_idx = static_cast<int>((center_z + extents_z) / kDomainSizeZ);

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
