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
			XMVECTOR center_pos;
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


		//
		// Mutators.
		//

		void set_center_pos(const XMVECTOR& pos);
		void set_rotation_rate(float rate);
		void set_min_view_radius(float radius);
		void set_max_view_radius(float radius);
		void set_approach_rate(float rate);


		//
		// Accessors.
		//

		const XMVECTOR& center_pos() const;
		float rotation_rate() const;
		float min_view_radius() const;
		float max_view_radius() const;
		float approach_rate() const;

	private:
		RotationalCameraDesc desc_;
	};
}
