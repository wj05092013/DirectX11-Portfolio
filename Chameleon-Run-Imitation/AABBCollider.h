#pragma once

/*
PCH: Yes
*/

#include "Collider.h"

namespace ba
{
	namespace collision
	{
		class AABBCollider : public Collider
		{
		public:
			struct AABBPlane
			{
				float restitution;	// Restituion factor([0.0f, 1.0f]).
				XMVECTOR plane_eq;	// Plane equation in local space.
			};

		protected:
			// Only the class 'CollisionManager' can call this constructor.
			AABBCollider();
			friend class CollisionManager;

		public:
			~AABBCollider() override;

			void UpdateDomainIndices() override;

		private:
			// These variables are based on local space.
			//  Plane order: +x, +y, +z, -x, -y, -z
			AABBPlane planes_[6];
			BoundingBox dx_bounding_box_;
		};
	}
}
