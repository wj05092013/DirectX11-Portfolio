#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Model;

	namespace physics
	{
		struct CollisionInfo;
		
		class Collider
		{
		public:
			enum EPrimitiveType
			{
				kSphere,
				kAxisAlignedBox
			};

		protected:
			Collider();
			friend class CollisionManager;

		public:
			virtual ~Collider();

			void OnCollision(const CollisionInfo& info);

			// The 'center_domain_idx_' is the index of the domain which the bounding shape's center is in.
			//  If the shape is in just one domain, the 'spread_domain_idx_' is same as the 'center_domain_idx_',
			//  and if not, it is the other index different from the center index.
			virtual void UpdateDomainIndices() = 0;

			//
			// Accessors
			//

			Model* model();

		protected:
			Model* model_;
			EPrimitiveType primitive_type_;

			// Collider can be in two domain.
			int center_domain_idx_;
			int spread_domain_idx_;
		};
	}
}
