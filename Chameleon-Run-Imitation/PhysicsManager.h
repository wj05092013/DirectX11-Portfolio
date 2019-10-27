#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace physics
	{
		class PhysicsManager
		{
		private:
			PhysicsManager();

		public:
			static PhysicsManager& GetInstance();

			~PhysicsManager();

			void Update(float delta_time);

			// Default is turned off.
			void SetGravity(bool enable);

			void AddPhysicsModel(PhysicsModel* model);

		private:
			std::vector<PhysicsModel*> models_;
		};
	}
}
