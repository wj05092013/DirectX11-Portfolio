#include "stdafx.h"

ba::RotationalCamera::RotationalCamera() :
	desc_{}
{
}

ba::RotationalCamera::~RotationalCamera()
{
}

void ba::RotationalCamera::Init(const RotationalCameraDesc& desc)
{
	desc_ = desc;
}

void ba::RotationalCamera::Destroy()
{
	// Do nothing.
}

void ba::RotationalCamera::Rotate(const POINT& start, const POINT& end)
{
	float d_theta = -XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.x - end.x));
	float d_phi = -XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.y - end.y));

	RotateTheta(desc_.center_pos, d_theta);
	RotatePhi(desc_.center_pos, d_phi);
}

void ba::RotationalCamera::Approach(float delta_time)
{
	float dist = desc_.approach_rate * delta_time;

	float view_radius = XMVectorGetX(XMVector3Length(position_w_xv() - desc_.center_pos));

	if (view_radius - dist < desc_.min_view_radius)
	{
		dist = view_radius - desc_.min_view_radius;
	}

	MoveCameraZ(dist);
}

void ba::RotationalCamera::StepBack(float delta_time)
{
	float dist = desc_.approach_rate * delta_time;

	float view_radius = XMVectorGetX(XMVector3Length(position_w_xv() - desc_.center_pos));

	if (view_radius + dist > desc_.max_view_radius)
	{
		dist = desc_.max_view_radius - view_radius;
	}

	MoveCameraZ(-dist);
}

void ba::RotationalCamera::set_center_pos(const XMVECTOR& pos)
{
	desc_.center_pos = pos;
	LookAt(position_w_xv(), desc_.center_pos, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void ba::RotationalCamera::set_rotation_rate(float rate)
{
	desc_.rotation_rate = rate;
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

const DirectX::XMVECTOR& ba::RotationalCamera::center_pos() const
{
	return desc_.center_pos;
}

float ba::RotationalCamera::rotation_rate() const
{
	return desc_.rotation_rate;
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
