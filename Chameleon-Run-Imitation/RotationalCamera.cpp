#include "stdafx.h"

ba::RotationalCamera::RotationalCamera() :
	desc_{}
{
}

ba::RotationalCamera::~RotationalCamera()
{
}

void ba::RotationalCamera::Init(const CameraDesc& base_desc, const RotationalCameraDesc& derived_desc)
{
	Camera::Init(base_desc);
	desc_ = derived_desc;
}

void ba::RotationalCamera::Destroy()
{
	// Do nothing.
}

void ba::RotationalCamera::Rotate(const POINT& start, const POINT& end)
{
	float d_theta = -XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.x - end.x));
	float d_phi = -XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.y - end.y));

	desc_.view_theta += d_theta;
	desc_.view_phi += d_phi;

	if (desc_.view_theta < 0.0f)
		desc_.view_theta += XM_2PI;
	else if (desc_.view_theta >= XM_2PI)
		desc_.view_theta -= XM_2PI;

	if(desc_.b_clamp_view_phi)
		desc_.view_phi = mathhelper::Clamp(desc_.view_phi, desc_.view_phi_clamp, XM_PI - desc_.view_phi_clamp);

	float x = desc_.view_radius * sinf(desc_.view_phi) * cosf(desc_.view_theta);
	float z = desc_.view_radius * sinf(desc_.view_phi) * sinf(desc_.view_theta);
	float y = desc_.view_radius * cosf(desc_.view_phi);
	
	XMVECTOR target = XMLoadFloat3(&desc_.center_pos);
	XMVECTOR pos = XMVectorAdd(target, desc_.view_radius * DirectX::XMVectorSet(x, y, z, 0.0f));
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	LookAt(pos, target, up);
}

void ba::RotationalCamera::Approach(float delta_time)
{
	float dist = desc_.approach_rate * delta_time;

	desc_.view_radius -= dist;
	if (desc_.view_radius < desc_.min_view_radius)
		desc_.view_radius = desc_.min_view_radius;

	MoveCameraZ(dist);
}

void ba::RotationalCamera::StepBack(float delta_time)
{
	float dist = desc_.approach_rate * delta_time;

	desc_.view_radius += dist;
	if (desc_.view_radius > desc_.max_view_radius)
		desc_.view_radius = desc_.max_view_radius;

	MoveCameraZ(-dist);
}

void ba::RotationalCamera::set_center_pos(const XMFLOAT3& pos)
{
	desc_.center_pos = pos;
}

void ba::RotationalCamera::set_view_theta(float radian)
{
	desc_.view_theta = radian;
}

void ba::RotationalCamera::set_view_phi(float radian)
{
	desc_.view_phi = radian;
}

void ba::RotationalCamera::set_b_clamp_view_phi(bool b_clamp)
{
	desc_.b_clamp_view_phi = b_clamp;
}

void ba::RotationalCamera::set_view_phi_clamp(float radian)
{
	desc_.view_phi_clamp = radian;
}

void ba::RotationalCamera::set_rotation_rate(float rate)
{
	desc_.rotation_rate = rate;
}

void ba::RotationalCamera::set_view_radius(float radius)
{
	desc_.view_radius = radius;
}

void ba::RotationalCamera::set_min_view_radius(float radius)
{
	desc_.min_view_radius = radius;
}

void ba::RotationalCamera::set_max_view_radius(float radius)
{
	desc_.max_view_radius = radius;
}

void ba::RotationalCamera::set_approach_rate(float rate)
{
	desc_.approach_rate = rate;
}

const DirectX::XMFLOAT3& ba::RotationalCamera::center_pos() const
{
	return desc_.center_pos;
}

float ba::RotationalCamera::view_theta() const
{
	return desc_.view_theta;
}

float ba::RotationalCamera::view_phi() const
{
	return desc_.view_phi;
}

bool ba::RotationalCamera::b_clamp_view_phi() const
{
	return desc_.b_clamp_view_phi;
}

float ba::RotationalCamera::view_phi_clamp() const
{
	return desc_.view_phi_clamp;
}

float ba::RotationalCamera::rotation_rate() const
{
	return desc_.rotation_rate;
}

float ba::RotationalCamera::view_radius() const
{
	return desc_.view_radius;
}

float ba::RotationalCamera::min_view_radius() const
{
	return desc_.min_view_radius;
}

float ba::RotationalCamera::max_view_radius() const
{
	return desc_.max_view_radius;
}

float ba::RotationalCamera::approach_rate() const
{
	return desc_.approach_rate;
}
