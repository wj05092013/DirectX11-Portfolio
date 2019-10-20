#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class FreeViewpointCamera : public Camera
	{
	public:
		FreeViewpointCamera(
			const XMFLOAT3& 
			float forward_move_rate, float right_move_rate, float upper_move_rate, float rotation_rate);
		~FreeViewpointCamera() override;

		// Moduate camera transform.
		void MoveFront(float delta_time);
		void MoveBack(float delta_time);
		void MoveLeft(float delta_time);
		void MoveRight(float delta_time);
		void MoveUp(float delta_time);
		void MoveDown(float delta_time);
		void Rotate(const POINT& head, const POINT& tail);

		// Mutators.
		void set_forward_move_rate(float rate);
		void set_right_move_rate(float rate);
		void set_upper_move_rate(float rate);

		// Accessors.
		float forward_move_rate() const;
		float right_move_rate() const;
		float upper_move_rate() const;

	private:
		float forward_move_rate_;
		float right_move_rate_;
		float upper_move_rate_;
		float rotation_rate_;
	};
}
