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
			XMFLOAT3 center_pos;
			float view_theta;
			float view_phi;
			bool b_clamp_view_phi;
			float view_phi_clamp;
			float rotation_rate;
			float view_radius;
			float min_view_radius;
			float max_view_radius;
			float approach_rate;
		};

		RotationalCamera(const CameraDesc& basic_desc, const RotationalCameraDesc& desc);
		~RotationalCamera() override;


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

		void set_center_pos(const XMFLOAT3& pos);
		void set_view_theta(float radian);
		void set_view_phi(float radian);
		void set_b_clamp_view_phi(bool b_clamp);
		void set_view_phi_clamp(float radian);
		void set_rotation_rate(float rate);
		void set_view_radius(float radius);
		void set_min_view_radius(float radius);
		void set_max_view_radius(float radius);
		void set_approach_rate(float rate);


		//
		// Accessors.
		//

		const XMFLOAT3& center_pos() const;
		float view_theta() const;
		float view_phi() const;
		bool b_clamp_view_phi() const;
		float view_phi_clamp() const;
		float rotation_rate() const;
		float view_radius() const;
		float min_view_radius() const;
		float max_view_radius() const;
		float approach_rate() const;

	private:
		RotationalCameraDesc desc_;
	};
}
