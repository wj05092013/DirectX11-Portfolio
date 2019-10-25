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

		void SphereCollider::Update()
		{
			Collider::Update();

			XMVECTOR center = XMVector3Transform(XMLoadFloat3(&dx_bounding_sphere_.Center), transform_);
			XMStoreFloat3(&dx_bounding_sphere_.Center, center);
		}

		void SphereCollider::CalcDomainIndices()
		{
			int center_idx = static_cast<int>(dx_bounding_sphere_.Center.z / kDomainSizeZ);
			int near_idx = static_cast<int>((dx_bounding_sphere_.Center.z - dx_bounding_sphere_.Radius) / kDomainSizeZ);
			int far_idx = static_cast<int>((dx_bounding_sphere_.Center.z + dx_bounding_sphere_.Radius) / kDomainSizeZ);

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
