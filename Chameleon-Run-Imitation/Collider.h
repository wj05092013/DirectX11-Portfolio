#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		// CollisionManager에 의해 생성, 관리
		// CollisionManager에게 충돌 정보를 전달받고 받은 정보를 통해
		//  PhysicsModel 갱신
		// Collider의 크기는 Collision Domain보다 클 수 없음
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
