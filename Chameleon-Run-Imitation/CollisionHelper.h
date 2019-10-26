#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		class AABBCollider;

		// Calculate equations of all planes of an axis aligned box, and store in the member of instances
		//  of AABBCollider::AABBPlane structure.
		void SetPlaneEquations(const BoundingBox& box, AABBCollider::AABBPlane out_planes[6]);

		// Calculate a vector from 'plane' to 'point', using the intersection point between the 'plane'
		//  and the line which passes the 'plane' perpendicularly.
		XMVECTOR CalcVectorFromTo(const XMVECTOR& plane, const XMVECTOR& point);
	}
}