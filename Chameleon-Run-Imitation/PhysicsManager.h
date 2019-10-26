#pragma once

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
			void ToggleGravity();
		};
	}
}
