#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		class Collider;

		struct CollisionInfo
		{
			float overlapped;
			float restitution;
			XMVECTOR normal;
			Collider* opponent;
		};

		// Calculate equations of all planes of an axis aligned box.
		void SetAABBPlaneEquations(const BoundingBox& box, XMVECTOR out_planes[6]);

		// Calculate a vector from 'plane' to 'point', using the intersection point between the 'plane'
		//  and the line which passes the 'plane' perpendicularly.
		XMVECTOR CalcVectorFromTo(const XMVECTOR& plane, const XMVECTOR& point);
	}
}