#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		AABBCollider::AABBCollider()
		{
		}

		AABBCollider::~AABBCollider()
		{
		}

		void AABBCollider::CalcDomainIndices()
		{
			int center_idx = static_cast<int>(dx_bounding_box_.Center.z / kDomainSizeZ);
			int near_idx = static_cast<int>((dx_bounding_box_.Center.z - dx_bounding_box_.Extents.z) / kDomainSizeZ);
			int far_idx = static_cast<int>((dx_bounding_box_.Center.z + dx_bounding_box_.Extents.z) / kDomainSizeZ);

			center_domain_idx = center_idx;

			// The collider is spread over two domains.
			if (near_idx != far_idx)
			{
				if (near_idx == center_idx)
					spread_domain_idx = far_idx;
				else
					spread_domain_idx = near_idx;
			}
			// The collider is in just one domain.
			else
			{
				spread_domain_idx = center_idx;
			}
		}
	}
}
