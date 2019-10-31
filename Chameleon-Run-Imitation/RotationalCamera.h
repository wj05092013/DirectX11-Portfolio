#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class RotationalCamera : public Camera
	{
	public:
		struct RotationalCameraDesc
		{
			float rotation_rate;
			float min_view_radius;
			float max_view_radius;
			float approach_rate;
		};

		RotationalCamera();
		~RotationalCamera() override;

		void Init(const RotationalCameraDesc& desc);
		void Destroy() override;

		//
		// Modulate camera transform.
		//

		// Always rotate around Y axis.
		void Rotate(const POINT& start, const POINT& end);
		void Approach(float delta_time);
		void StepBack(float delta_time);

		// If this camera's center position is moving, this function must be called on per frame.
		void UpdateCenterPos(const XMFLOAT3& pos);
		void UpdateCenterPos(const XMVECTOR& pos);

		//
		// Mutators.
		//

		void set_center_pos(const XMFLOAT3& pos);
		void set_center_pos(const XMVECTOR& pos);
		void set_rotation_rate(float rate);
		void set_min_view_radius(float radius);
		void set_max_view_radius(float radius);
		void set_approach_rate(float rate);

		//
		// Accessors.
		//

		const XMFLOAT3& center_pos_xf() const;
		const XMVECTOR center_pos_xv() const;
		float rotation_rate() const;
		float min_view_radius() const;
		float max_view_radius() const;
		float approach_rate() const;

	private:
		RotationalCameraDesc desc_;
		XMFLOAT3 center_pos_;
	};
}
