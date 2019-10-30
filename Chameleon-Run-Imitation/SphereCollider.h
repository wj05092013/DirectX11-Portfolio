#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace physics
	{
		class SphereCollider : public Collider
		{
		protected:
			// Only the class 'CollisionManager' can call this constructor.
			SphereCollider();
			friend class CollisionManager;

		public:
			~SphereCollider() override;

			void UpdateDomainIndices() override;

		private:
			float restitution_;						// Restituion factor([0.0f, 1.0f]).
			BoundingSphere dx_bounding_sphere_;		// Based on local space.
		};
	}
}
