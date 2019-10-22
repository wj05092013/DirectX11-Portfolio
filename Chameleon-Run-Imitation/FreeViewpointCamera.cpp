#include "stdafx.h"

ba::FreeViewpointCamera::FreeViewpointCamera() :
	desc_{}
{
}

ba::FreeViewpointCamera::~FreeViewpointCamera()
{
}

void ba::FreeViewpointCamera::Init(const CameraDesc& base_desc, const FreeViewpointCameraDesc& derived_desc)
{
	Camera::Init(base_desc);
	desc_ = derived_desc;
}

void ba::FreeViewpointCamera::Destroy()
{
	// Do nothing.
}

void ba::FreeViewpointCamera::MoveFront(float delta_time)
{
	MoveCameraZ(desc_.forward_move_rate * delta_time);
}

void ba::FreeViewpointCamera::MoveBack(float delta_time)
{
	MoveCameraZ(-desc_.forward_move_rate * delta_time);
}

void ba::FreeViewpointCamera::MoveLeft(float delta_time)
{
	MoveCameraX(-desc_.right_move_rate * delta_time);
}

void ba::FreeViewpointCamera::MoveRight(float delta_time)
{
	MoveCameraX(desc_.right_move_rate * delta_time);
}

void ba::FreeViewpointCamera::MoveUp(float delta_time)
{
	MoveWorldY(desc_.upper_move_rate * delta_time);
}

void ba::FreeViewpointCamera::MoveDown(float delta_time)
{
	MoveWorldY(-desc_.upper_move_rate * delta_time);
}

void ba::FreeViewpointCamera::Rotate(const POINT& start, const POINT& end)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.x - end.x));
	float dy = XMConvertToRadians(desc_.rotation_rate * static_cast<float>(start.y - end.y));

	Pitch(dy);
	RotateWorldY(dx);
}

void ba::FreeViewpointCamera::set_forward_move_rate(float rate)
{
	desc_.forward_move_rate = rate;
}

void ba::FreeViewpointCamera::set_right_move_rate(float rate)
{
	desc_.right_move_rate = rate;
}

void ba::FreeViewpointCamera::set_upper_move_rate(float rate)
{
	desc_.upper_move_rate = rate;
}

void ba::FreeViewpointCamera::set_rotation_rate(float rate)
{
	desc_.rotation_rate = rate;
}

float ba::FreeViewpointCamera::forward_move_rate() const
{
	return desc_.forward_move_rate;
}

float ba::FreeViewpointCamera::right_move_rate() const
{
	return desc_.right_move_rate;
}

float ba::FreeViewpointCamera::upper_move_rate() const
{
	return desc_.upper_move_rate;
}

float ba::FreeViewpointCamera::rotation_rate() const
{
	return desc_.rotation_rate;
}


