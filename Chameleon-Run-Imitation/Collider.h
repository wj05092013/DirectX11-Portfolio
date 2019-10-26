#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
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

			// Update this collider's transform.
			//	If this collider is static type, calling this function is meaningless.
			virtual void Update();

			void OnCollision(const CollisionInfo& info);

			// The 'center_domain_idx_' is the index of the domain which the bounding shape's center is in.
			//  If the shape is in just one domain, the 'spread_domain_idx_' is same as the 'center_domain_idx_',
			//  and if not, it is the other index different from the center index.
			virtual void CalcDomainIndices() = 0;

		protected:
			Model* model_;
			EPrimitiveType primitive_type_;

			// Collider can be in two domain.
			int center_domain_idx_;
			int spread_domain_idx_;

			XMMATRIX transform_;
		};
	}
}
