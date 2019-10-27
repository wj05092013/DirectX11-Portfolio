#include "stdafx.h"

namespace ba
{
	namespace collision
	{
		static XMVECTOR kBoxNormal[3] = {
			XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),	// Plane normal of positive x.
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),	// Plane normal of positive y.
			XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)		// Plane normal of positive z.
		};

		void SetPlaneEquations(const BoundingBox& box, AABBCollider::AABBPlane out_planes[6])
		{
			XMVECTOR center = XMLoadFloat3(&box.Center);
			XMVECTOR extents = XMLoadFloat3(&box.Extents);

			// Planes of positive direction.
			XMVECTOR p0 = center + extents;
			for (int i = 0; i < 3; ++i)
			{
				XMPlaneFromPointNormal(p0, kBoxNormal[i]);
			}

			// Planes of negative direction.
			p0 = center - extents;
			for (int i = 3; i < 6; ++i)
			{
				XMPlaneFromPointNormal(p0, kBoxNormal[i]);
			}
		}

		XMVECTOR CalcVectorFromTo(const XMVECTOR& plane, const XMVECTOR& point)
		{
			float k = -XMVectorGetX(XMVector3Dot(plane, point)) - XMVectorGetW(plane);
			return XMVectorSetW(-k * plane, 0.0f);
		}
	}
}
