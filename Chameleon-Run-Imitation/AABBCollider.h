#pragma once

/*
PCH: Yes
*/

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
				XMVECTOR plane_eq;
			};

		protected:
			// Only the class 'CollisionManager' can call this constructor.
			AABBCollider();
			friend class CollisionManager;

		public:
			~AABBCollider() override;

			void CalcDomainIndices() override;

		private:
			// Plane order: +x, +y, +z, -x, -y, -z
			AABBPlane planes_[6];
			BoundingBox dx_bounding_box_;
		};
	}
}
