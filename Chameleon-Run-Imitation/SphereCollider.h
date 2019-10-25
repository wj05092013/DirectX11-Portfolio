#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		class SphereCollider : public Collider
		{
		protected:
			// Only the class 'CollisionManager' can call this constructor.
			SphereCollider();
			friend class CollisionManager;

		public:
			~SphereCollider() override;

			void Update() override;

			void CalcDomainIndices() override;

		private:
			float restitution_;						// Restituion factor([0.0f, 1.0f]).
			BoundingSphere dx_bounding_sphere_;
		};
	}
}
