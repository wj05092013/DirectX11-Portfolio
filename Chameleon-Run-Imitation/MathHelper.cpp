#include "stdafx.h"

const DirectX::XMMATRIX ba::mathhelper::kRhToLh = DirectX::XMMATRIX(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

float ba::mathhelper::AngleFromXY(float x, float y)
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

DirectX::XMMATRIX ba::mathhelper::InverseTranspose(DirectX::CXMMATRIX m)
{
	// Inverse-transpose is just applied to normals.
	//  So zero out translation row so that it doesn't get into our inverse-transpose
	//  calculation(we don't want the inverse-transpose of the translation).
	XMMATRIX a = m;
	a.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR det = XMMatrixDeterminant(a);
	return XMMatrixTranspose(XMMatrixInverse(&det, a));
}

DirectX::XMVECTOR ba::mathhelper::RandUnitVec3()
{
	XMVECTOR one = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(mathhelper::RandF(-1.0f, 1.0f), mathhelper::RandF(-1.0f, 1.0f), mathhelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		//  over the unit sphere.  Otherwise points will clump more on the sphere near 
		//  the corners of the cube.
		if (XMVector3Greater(XMVector3LengthSq(v), one))
			continue;

		return XMVector3Normalize(v);
	}
}

DirectX::XMVECTOR ba::mathhelper::RandHemisphereUnitVec3(DirectX::XMVECTOR n)
{
	XMVECTOR one = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(mathhelper::RandF(-1.0f, 1.0f), mathhelper::RandF(-1.0f, 1.0f), mathhelper::RandF(-1.0f, 1.0f), 0.0f);

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
