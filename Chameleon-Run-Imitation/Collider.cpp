#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		Collider::Collider()
		{
			BoundingBox
		}

		Collider::~Collider()
		{
		}

		void Collider::Update()
		{
		}

		void Collider::CalcDomainIndex()
		{
			int center_idx;
			int left_idx = -1;
			int right_idx = -1;

			if (desc_.primitive_type_ == kSphere)
			{
				center_idx = static_cast<int>(desc_.sphere.Center.z / kDomainSizeZ);
				left_idx = static_cast<int>((desc_.sphere.Center.z - desc_.sphere.Radius) / kDomainSizeZ);
				right_idx = static_cast<int>((desc_.sphere.Center.z + desc_.sphere.Radius) / kDomainSizeZ);
			}
			else if (desc_.primitive_type_ == kAxisAlignedBox)
			{
				center_idx = static_cast<int>(desc_.box.Center.z / kDomainSizeZ);
				left_idx = static_cast<int>((desc_.box.Center.z - desc_.box.Extents.z) / kDomainSizeZ);
				right_idx = static_cast<int>((desc_.box.Center.z + desc_.box.Extents.z) / kDomainSizeZ);
			}

			out_first_idx = center_idx;

			if (left_idx != right_idx)
			{
				if (left_idx == center_idx)
					out_second_idx = right_idx;
				else
					out_second_idx = left_idx;
			}
			else
				out_second_idx = -1;
		}
	}
}