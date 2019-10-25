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

		void AABBCollider::Update()
		{
			Collider::Update();

			XMVECTOR center = XMVector3Transform(XMLoadFloat3(&dx_bounding_box_.Center), transform_);
			XMStoreFloat3(&dx_bounding_box_.Center, center);

			// Build new equations of planes for the transformed box collider.
			SetPlaneEquations(dx_bounding_box_, planes_);
		}

		void AABBCollider::CalcDomainIndices()
		{
			int center_idx = static_cast<int>(dx_bounding_box_.Center.z / kDomainSizeZ);
			int near_idx = static_cast<int>((dx_bounding_box_.Center.z - dx_bounding_box_.Extents.z) / kDomainSizeZ);
			int far_idx = static_cast<int>((dx_bounding_box_.Center.z + dx_bounding_box_.Extents.z) / kDomainSizeZ);

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
