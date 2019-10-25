#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		class PhysicsModel;

		class Collider
		{
		public:
			enum EMovementType
			{
				kStatic,
				kDynamic
			};
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

			// 현재 속한 영역의 인덱스 갱신
			//  Dynamic type인 경우
			void Update();

			void OnCollision(const CollisionInfo& info);

			// The 'center_domain_idx' is the index of the domain which the bounding shape's center is in.
			//  If the shape is in just one domain, the 'spread_domain_idx' is same as the 'center_domain_idx',
			//  and if not, it is the other index different from the center index.
			virtual void CalcDomainIndices() = 0;


			//
			// Accessors.
			//

			EMovementType movement_type() const;
			EPrimitiveType primitive_type() const;

		protected:
			PhysicsModel* physics_model_;

			EMovementType movement_type_;
			EPrimitiveType primitive_type_;

			// Collider can be in two domain.
			int center_domain_idx;
			int spread_domain_idx;
		};
	}
}
