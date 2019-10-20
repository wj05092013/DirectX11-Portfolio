#include "stdafx.h"

ba::Camera::Camera(const CameraDesc& init_desc) :
	desc_(init_desc),
	near_window_height_(0.0f),
	far_window_height_(0.0f)
{
	XMMATRIX identity = XMMatrixIdentity();
	XMStoreFloat4x4(&view_, identity);
	XMStoreFloat4x4(&proj_, identity);
}

ba::Camera::~Camera()
{
}

void ba::Camera::UpdateViewMatrix()
{
	XMVECTOR p = XMLoadFloat3(&desc_.pos_w);
	XMVECTOR u = XMLoadFloat3(&desc_.right_w);
	XMVECTOR v = XMLoadFloat3(&desc_.up_w);
	XMVECTOR w = XMLoadFloat3(&desc_.look_w);

	w = XMVector3Normalize(w);
	u = XMVector3Normalize(XMVector3Cross(v, w));
	v = XMVector3Cross(w, u);

	XMStoreFloat3(&desc_.right_w, u);
	XMStoreFloat3(&desc_.up_w, v);
	XMStoreFloat3(&desc_.look_w, w);

	float x = -XMVectorGetX(XMVector3Dot(p, u));
	float y = -XMVectorGetY(XMVector3Dot(p, v));
	float z = -XMVectorGetZ(XMVector3Dot(p, w));

	view_(0, 0) = desc_.right_w.x;
	view_(1, 0) = desc_.right_w.y;
	view_(2, 0) = desc_.right_w.z;
	view_(3, 0) = x;

	view_(0, 1) = desc_.up_w.x;
	view_(1, 1) = desc_.up_w.y;
	view_(2, 1) = desc_.up_w.z;
	view_(3, 1) = y;

	view_(0, 2) = desc_.look_w.x;
	view_(1, 2) = desc_.look_w.y;
	view_(2, 2) = desc_.look_w.z;
	view_(3, 2) = z;

	view_(0, 3) = 0.0f;
	view_(1, 3) = 0.0f;
	view_(2, 3) = 0.0f;
	view_(3, 3) = 1.0f;
}

void ba::Camera::SetLens(float fov_y, float aspect_ratio, float near_z, float far_z)
{
	desc_.near_z = near_z;
	desc_.far_z = far_z;

	desc_.fov_y = fov_y;
	desc_.aspect_ratio = aspect_ratio;

	near_window_height_ = 2.0f * tan(0.5f * desc_.fov_y) * desc_.near_z;
	far_window_height_ = 2.0f * tan(0.5f * desc_.fov_y) * desc_.far_z;

	XMMATRIX proj = XMMatrixPerspectiveFovLH(desc_.fov_y, desc_.aspect_ratio, desc_.near_z, desc_.far_z);
	XMStoreFloat4x4(&proj_, proj);
}

void ba::Camera::LookAt(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& up)
{
	XMVECTOR w = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(up, w));
	XMVECTOR v = XMVector3Cross(w, u);

	XMStoreFloat3(&desc_.pos_w, pos);
	XMStoreFloat3(&desc_.right_w, u);
	XMStoreFloat3(&desc_.up_w, v);
	XMStoreFloat3(&desc_.look_w, w);
}

void ba::Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	LookAt(XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void ba::Camera::MoveCameraX(float d)
{
	XMVECTOR x = XMVectorScale(XMLoadFloat3(&desc_.right_w), d);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&desc_.pos_w), x);
	XMStoreFloat3(&desc_.pos_w, pos);
}

void ba::Camera::MoveCameraZ(float d)
{
	XMVECTOR z = XMVectorScale(XMLoadFloat3(&desc_.look_w), d);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&desc_.pos_w), z);
	XMStoreFloat3(&desc_.pos_w, pos);
}

void ba::Camera::MoveCameraY(float d)
{
	XMVECTOR y = XMVectorScale(XMLoadFloat3(&desc_.up_w), d);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&desc_.pos_w), y);
	XMStoreFloat3(&desc_.pos_w, pos);
}

void ba::Camera::MoveWorldY(float d)
{
	XMVECTOR world_y = XMVectorSet(0.0f, d, 0.0f, 0.0f);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&desc_.pos_w), world_y);
	XMStoreFloat3(&desc_.pos_w, pos);
}

void ba::Camera::Roll(float radian)
{
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&desc_.look_w), radian);

	XMStoreFloat3(&desc_.right_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.right_w), rot));
	XMStoreFloat3(&desc_.up_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.up_w), rot));
}

void ba::Camera::Pitch(float radian)
{
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&desc_.right_w), radian);

	XMStoreFloat3(&desc_.up_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.up_w), rot));
	XMStoreFloat3(&desc_.look_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.look_w), rot));
}

void ba::Camera::Yaw(float radian)
{
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&desc_.up_w), radian);

	XMStoreFloat3(&desc_.right_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.right_w), rot));
	XMStoreFloat3(&desc_.look_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.look_w), rot));
}

void ba::Camera::RotateWorldX(float radian)
{
	XMMATRIX rot = XMMatrixRotationX(radian);

	XMStoreFloat3(&desc_.right_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.right_w), rot));
	XMStoreFloat3(&desc_.up_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.up_w), rot));
	XMStoreFloat3(&desc_.look_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.look_w), rot));
}

void ba::Camera::RotateWorldY(float radian)
{
	XMMATRIX rot = XMMatrixRotationY(radian);

	XMStoreFloat3(&desc_.right_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.right_w), rot));
	XMStoreFloat3(&desc_.up_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.up_w), rot));
	XMStoreFloat3(&desc_.look_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.look_w), rot));
}

void ba::Camera::RotateWorldZ(float radian)
{
	XMMATRIX rot = XMMatrixRotationZ(radian);

	XMStoreFloat3(&desc_.right_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.right_w), rot));
	XMStoreFloat3(&desc_.up_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.up_w), rot));
	XMStoreFloat3(&desc_.look_w, XMVector3TransformNormal(XMLoadFloat3(&desc_.look_w), rot));
}

void ba::Camera::set_position_w(float x, float y, float z)
{
	set_position_w(XMFLOAT3(x, y, z));
}

void ba::Camera::set_position_w(const XMFLOAT3& pos)
{
	desc_.pos_w = pos;
}

const DirectX::XMVECTOR& ba::Camera::position_w_xv() const
{
	return XMLoadFloat3(&desc_.pos_w);
}

const DirectX::XMFLOAT3& ba::Camera::position_w_xf() const
{
	return desc_.pos_w;
}

const DirectX::XMVECTOR& ba::Camera::right_w_xv() const
{
	return XMLoadFloat3(&desc_.right_w);
}

const DirectX::XMFLOAT3& ba::Camera::right_w_xf() const
{
	return desc_.right_w;
}

const DirectX::XMVECTOR& ba::Camera::up_w_xv() const
{
	return XMLoadFloat3(&desc_.up_w);
}

const DirectX::XMFLOAT3& ba::Camera::up_w_xf() const
{
	return desc_.up_w;
}

const DirectX::XMVECTOR& ba::Camera::look_w_xv() const
{
	return XMLoadFloat3(&desc_.look_w);
}

const DirectX::XMFLOAT3& ba::Camera::look_w_xf() const
{
	return desc_.look_w;
}

float ba::Camera::near_z() const
{
	return desc_.near_z;
}

float ba::Camera::far_z() const
{
	return desc_.far_z;
}

float ba::Camera::near_window_width() const
{
	return desc_.aspect_ratio * near_window_height_;
}

float ba::Camera::near_window_height() const
{
	return near_window_height_;
}

float ba::Camera::far_window_width() const
{
	return desc_.aspect_ratio * far_window_height_;
}

float ba::Camera::far_window_height() const
{
	return far_window_height_;
}

float ba::Camera::aspect_ratio() const
{
	return desc_.aspect_ratio;
}

float ba::Camera::fov_x() const
{
	float half_width = 0.5f * near_window_width();
	return 2.0f * atan(half_width / desc_.near_z);
}

float ba::Camera::fov_y() const
{
	return desc_.fov_y;
}

const DirectX::XMMATRIX& ba::Camera::view() const
{
	return XMLoadFloat4x4(&view_);
}

const DirectX::XMMATRIX& ba::Camera::proj() const
{
	return XMLoadFloat4x4(&proj_);
}
