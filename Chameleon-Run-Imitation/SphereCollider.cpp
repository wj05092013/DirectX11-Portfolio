#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		SphereCollider::SphereCollider()
		{
		}

		SphereCollider::~SphereCollider()
		{
		}

		void SphereCollider::CalcDomainIndices()
		{
			int center_idx = static_cast<int>(dx_bounding_sphere_.Center.z / kDomainSizeZ);
			int near_idx = static_cast<int>((dx_bounding_sphere_.Center.z - dx_bounding_sphere_.Radius) / kDomainSizeZ);
			int far_idx = static_cast<int>((dx_bounding_sphere_.Center.z + dx_bounding_sphere_.Radius) / kDomainSizeZ);

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
