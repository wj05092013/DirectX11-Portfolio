#include "stdafx.h"

ba::RotationalCamera::RotationalCamera() :
	desc_{},
	center_pos_(0.0f, 0.0f, 0.0f)
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
	float d_theta = XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.x - end.x));
	float d_phi = -XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.y - end.y));

	RotateTheta(center_pos_xv(), d_theta);
	RotatePhi(center_pos_xv(), d_phi);
}

void ba::RotationalCamera::Approach(float delta_time)
{
	float dist = desc_.approach_rate * delta_time;

	float view_radius = XMVectorGetX(XMVector3Length(position_w_xv() - center_pos_xv()));

	if (view_radius - dist < desc_.min_view_radius)
	{
		dist = view_radius - desc_.min_view_radius;
	}

	MoveCameraZ(dist);
}

void ba::RotationalCamera::StepBack(float delta_time)
{
	float dist = desc_.approach_rate * delta_time;

	float view_radius = XMVectorGetX(XMVector3Length(position_w_xv() - center_pos_xv()));

	if (view_radius + dist > desc_.max_view_radius)
	{
		dist = desc_.max_view_radius - view_radius;
	}

	MoveCameraZ(-dist);
}

void ba::RotationalCamera::UpdateCenterPos(const XMFLOAT3& pos)
{
	UpdateCenterPos(XMLoadFloat3(&pos));
}

void ba::RotationalCamera::UpdateCenterPos(const XMVECTOR& pos)
{
	XMVECTOR diff = pos - center_pos_xv();

	XMStoreFloat3(&center_pos_, pos);
	LookAt(position_w_xv() + diff, pos, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void ba::RotationalCamera::set_center_pos(const XMFLOAT3& pos)
{
	center_pos_ = pos;
}

void ba::RotationalCamera::set_center_pos(const XMVECTOR& pos)
{
	XMStoreFloat3(&center_pos_, pos);
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

const DirectX::XMFLOAT3& ba::RotationalCamera::center_pos_xf() const
{
	return center_pos_;
}

const DirectX::XMVECTOR ba::RotationalCamera::center_pos_xv() const
{
	return XMLoadFloat3(&center_pos_);
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
