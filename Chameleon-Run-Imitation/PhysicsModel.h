#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace collision
	{
		class Collider;

		// Collider를 포함
		// 
		class PhysicsModel : public Model
		{
		public:
			PhysicsModel();
			~PhysicsModel() override;

			// 이 PhysicsModel의 충돌을 담당할 Collider 생성
			void CreateCollider(Collider::EMovementType movement_type, Collider::EPrimitiveType primitive_type);

			void OnCollision();

			// 트랜스폼 갱신
			void Update();

		private:
			bool gravity_enabled_;

			float mass_;
			XMVECTOR velocity_;
			XMVECTOR res_force_;
		};
	}
}
