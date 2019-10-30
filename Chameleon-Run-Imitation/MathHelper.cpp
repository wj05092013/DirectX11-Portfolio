#include "stdafx.h"

namespace ba
{
	namespace mathhelper
	{
		const XMMATRIX kRhToLh = XMMATRIX(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		float AngleFromXY(float x, float y)
		{
			float theta = 0.0f;

			// Quadrant I or IV
			if (x >= 0.0f)
			{
				// If x = 0, then atanf(y/x) = +pi/2 if y > 0
				//                atanf(y/x) = -pi/2 if y < 0
				theta = atanf(y / x); // in [-pi/2, +pi/2]

				if (theta < 0.0f)
					theta += 2.0f * XM_PI; // in [0, 2*pi).
			}
			// Quadrant II or III
			else
				theta = atanf(y / x) + XM_PI; // in [0, 2*pi).

			return theta;
		}

		XMMATRIX InverseTranspose(CXMMATRIX m)
		{
			// Inverse-transpose is just applied to normals.
			//  So zero out translation row so that it doesn't get into our inverse-transpose
			//  calculation(we don't want the inverse-transpose of the translation).
			XMMATRIX a = m;
			a.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMVECTOR det = XMMatrixDeterminant(a);
			return XMMatrixTranspose(XMMatrixInverse(&det, a));
		}

		XMVECTOR RandUnitVec3()
		{
			XMVECTOR one = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
			XMVECTOR zero = XMVectorZero();

			// Keep trying until we get a point on/in the hemisphere.
			while (true)
			{
				// Generate random point in the cube [-1,1]^3.
				XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);

				// Ignore points outside the unit sphere in order to get an even distribution 
				//  over the unit sphere.  Otherwise points will clump more on the sphere near 
				//  the corners of the cube.
				if (XMVector3Greater(XMVector3LengthSq(v), one))
					continue;

				return XMVector3Normalize(v);
			}
		}

		XMVECTOR RandHemisphereUnitVec3(XMVECTOR n)
		{
			XMVECTOR one = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
			XMVECTOR zero = XMVectorZero();

			// Keep trying until we get a point on/in the hemisphere.
			while (true)
			{
				// Generate random point in the cube [-1,1]^3.
				XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);

				// Ignore points outside the unit sphere in order to get an even distribution 
				//  over the unit sphere.  Otherwise points will clump more on the sphere near 
				//  the corners of the cube.
				if (XMVector3Greater(XMVector3LengthSq(v), one))
					continue;

				// Ignore points in the bottom hemisphere.
				if (XMVector3Less(XMVector3Dot(n, v), zero))
					continue;

				return XMVector3Normalize(v);
			}
		}

		static XMVECTOR kBoxNormal[3] = {
			XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),	// Plane normal of positive x.
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),	// Plane normal of positive y.
			XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)		// Plane normal of positive z.
		};

		void SetAABBPlaneEquations(const BoundingBox& box, XMVECTOR out_planes[6])
		{
			XMVECTOR center = XMLoadFloat3(&box.Center);
			XMVECTOR extents = XMLoadFloat3(&box.Extents);

			// Planes of positive direction.
			XMVECTOR p0 = center + extents;
			for (int i = 0; i < 3; ++i)
			{
				out_planes[i] = XMPlaneFromPointNormal(p0, kBoxNormal[i]);
			}

			// Planes of negative direction.
			p0 = center - extents;
			for (int i = 0; i < 3; ++i)
			{
				out_planes[i + 3] = XMPlaneFromPointNormal(p0, -kBoxNormal[i]);
			}
		}

		XMVECTOR CalcVectorFromTo(const XMVECTOR& plane, const XMVECTOR& point)
		{
			float k = (-XMVectorGetX(XMVector3Dot(plane, point)) - XMVectorGetW(plane));
			k /= XMVectorGetX(XMVector3Dot(plane, plane));
			return XMVectorSetW(-k * plane, 0.0f);
		}
	}
}
