#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class FreeViewpointCamera : public Camera
	{
	public:
		struct FreeViewpointCameraDesc
		{
			float forward_move_rate;
			float right_move_rate;
			float upper_move_rate;
			float rotation_rate;
		};

		FreeViewpointCamera(const CameraDesc& basic_desc, const FreeViewpointCameraDesc& desc);
		~FreeViewpointCamera() override;

		// Moduate camera transform.
		void MoveFront(float delta_time);
		void MoveBack(float delta_time);
		void MoveLeft(float delta_time);
		void MoveRight(float delta_time);
		void MoveUp(float delta_time);
		void MoveDown(float delta_time);
		void Rotate(const POINT& start, const POINT& end);

		// Mutators.
		void set_forward_move_rate(float rate);
		void set_right_move_rate(float rate);
		void set_upper_move_rate(float rate);
		void set_rotation_rate(float rate);

		// Accessors.
		float forward_move_rate() const;
		float right_move_rate() const;
		float upper_move_rate() const;
		float rotation_rate() const;

	private:
		FreeViewpointCameraDesc desc_;
	};
}
