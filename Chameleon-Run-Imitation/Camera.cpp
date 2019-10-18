#include "stdafx.h"

const DirectX::XMFLOAT3 ba::Camera::kInitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
const DirectX::XMFLOAT3 ba::Camera::kInitRightW = XMFLOAT3(1.0f, 0.0f, 0.0f);
const DirectX::XMFLOAT3 ba::Camera::kInitUpW = XMFLOAT3(0.0f, 1.0f, 0.0f);
const DirectX::XMFLOAT3 ba::Camera::kInitLookW = XMFLOAT3(0.0f, 0.0f, 1.0f);

const float ba::Camera::kInitNearZ = 1.0f;
const float ba::Camera::kInitFarZ = 1000.0f;
const float ba::Camera::kInitFovY = 0.25f * XM_PI;
const float ba::Camera::kInitAspectRatio = 1.0f;

ba::Camera::Camera() :
	pos_w_(kInitPosW),
	right_w_(kInitRightW),
	up_w_(kInitUpW),
	look_w_(kInitLookW),

	near_z_(kInitNearZ),
	far_z_(kInitFarZ),
	near_window_height_(0.0f),
	far_window_height_(0.0f),
	fov_y_(kInitFovY),
	aspect_ratio_(kInitAspectRatio)
{
	XMMATRIX identity = XMMatrixIdentity();
	XMStoreFloat4x4(&view_, identity);
	XMStoreFloat4x4(&proj_, identity);
}

ba::Camera::~Camera()
{
}

DirectX::XMVECTOR ba::Camera::position_w_xv() const
{
	return XMLoadFloat3(&pos_w_);
}

DirectX::XMFLOAT3 ba::Camera::position_w_xf() const
{
	return pos_w_;
}

void ba::Camera::set_position_w(float x, float y, float z)
{
	set_position_w(XMFLOAT3(x, y, z));
}

void ba::Camera::set_position_w(const XMFLOAT3& pos)
{
	pos_w_ = pos;
}

DirectX::XMVECTOR ba::Camera::right_w_xv() const
{
	return XMLoadFloat3(&right_w_);
}

DirectX::XMFLOAT3 ba::Camera::right_w_xf() const
{
	return right_w_;
}

DirectX::XMVECTOR ba::Camera::up_w_xv() const
{
	return XMLoadFloat3(&up_w_);
}

DirectX::XMFLOAT3 ba::Camera::up_w_xf() const
{
	return up_w_;
}

DirectX::XMVECTOR ba::Camera::look_w_xv() const
{
	return XMLoadFloat3(&look_w_);
}

DirectX::XMFLOAT3 ba::Camera::look_w_xf() const
{
	return look_w_;
}

float ba::Camera::near_z() const
{
	return near_z_;
}

float ba::Camera::far_z() const
{
	return far_z_;
}

float ba::Camera::near_window_width() const
{
	return aspect_ratio_ * near_window_height_;
}

float ba::Camera::near_window_height() const
{
	return near_window_height_;
}

float ba::Camera::far_window_width() const
{
	return aspect_ratio_ * far_window_height_;
}

float ba::Camera::far_window_height() const
{
	return far_window_height_;
}

float ba::Camera::aspect_ratio() const
{
	return aspect_ratio_;
}

float ba::Camera::fov_x() const
{
	float half_width = 0.5f * near_window_width();
	return 2.0f * atan(half_width / near_z_);
}

float ba::Camera::fov_y() const
{
	return fov_y_;
}

DirectX::XMMATRIX ba::Camera::view() const
{
	return XMLoadFloat4x4(&view_);
}

DirectX::XMMATRIX ba::Camera::proj() const
{
	return XMLoadFloat4x4(&proj_);
}

void ba::Camera::LookAt(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& up)
{
	XMVECTOR w = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(up, w));
	XMVECTOR v = XMVector3Cross(w, u);

	XMStoreFloat3(&pos_w_, pos);
	XMStoreFloat3(&right_w_, u);
	XMStoreFloat3(&up_w_, v);
	XMStoreFloat3(&look_w_, w);
}

void ba::Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	LookAt(XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void ba::Camera::MoveCameraX(float d)
{
	XMVECTOR x = XMVectorScale(XMLoadFloat3(&right_w_), d);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&pos_w_), x);
	XMStoreFloat3(&pos_w_, pos);
}

void ba::Camera::MoveCameraZ(float d)
{
	XMVECTOR z = XMVectorScale(XMLoadFloat3(&look_w_), d);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&pos_w_), z);
	XMStoreFloat3(&pos_w_, pos);
}

void ba::Camera::MoveCameraY(float d)
{
	XMVECTOR y = XMVectorScale(XMLoadFloat3(&up_w_), d);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&pos_w_), y);
	XMStoreFloat3(&pos_w_, pos);
}

void ba::Camera::MoveWorldY(float d)
{
	XMVECTOR world_y = XMVectorSet(0.0f, d, 0.0f, 0.0f);
	XMVECTOR pos = XMVectorAdd(XMLoadFloat3(&pos_w_), world_y);
	XMStoreFloat3(&pos_w_, pos);
}

void ba::Camera::Roll(float radian)
{
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&look_w_), radian);

	XMStoreFloat3(&right_w_, XMVector3TransformNormal(XMLoadFloat3(&right_w_), rot));
	XMStoreFloat3(&up_w_, XMVector3TransformNormal(XMLoadFloat3(&up_w_), rot));
}

void ba::Camera::Pitch(float radian)
{
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&right_w_), radian);

	XMStoreFloat3(&up_w_, XMVector3TransformNormal(XMLoadFloat3(&up_w_), rot));
	XMStoreFloat3(&look_w_, XMVector3TransformNormal(XMLoadFloat3(&look_w_), rot));
}

void ba::Camera::Yaw(float radian)
{
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&up_w_), radian);

	XMStoreFloat3(&right_w_, XMVector3TransformNormal(XMLoadFloat3(&right_w_), rot));
	XMStoreFloat3(&look_w_, XMVector3TransformNormal(XMLoadFloat3(&look_w_), rot));
}

void ba::Camera::RotateWorldX(float radian)
{
	XMMATRIX rot = XMMatrixRotationX(radian);

	XMStoreFloat3(&right_w_, XMVector3TransformNormal(XMLoadFloat3(&right_w_), rot));
	XMStoreFloat3(&up_w_, XMVector3TransformNormal(XMLoadFloat3(&up_w_), rot));
	XMStoreFloat3(&look_w_, XMVector3TransformNormal(XMLoadFloat3(&look_w_), rot));
}

void ba::Camera::RotateWorldY(float radian)
{
	XMMATRIX rot = XMMatrixRotationY(radian);

	XMStoreFloat3(&right_w_, XMVector3TransformNormal(XMLoadFloat3(&right_w_), rot));
	XMStoreFloat3(&up_w_, XMVector3TransformNormal(XMLoadFloat3(&up_w_), rot));
	XMStoreFloat3(&look_w_, XMVector3TransformNormal(XMLoadFloat3(&look_w_), rot));
}

void ba::Camera::RotateWorldZ(float radian)
{
	XMMATRIX rot = XMMatrixRotationZ(radian);

	XMStoreFloat3(&right_w_, XMVector3TransformNormal(XMLoadFloat3(&right_w_), rot));
	XMStoreFloat3(&up_w_, XMVector3TransformNormal(XMLoadFloat3(&up_w_), rot));
	XMStoreFloat3(&look_w_, XMVector3TransformNormal(XMLoadFloat3(&look_w_), rot));
}

void ba::Camera::UpdateViewMatrix()
{
	XMVECTOR p = XMLoadFloat3(&pos_w_);
	XMVECTOR u = XMLoadFloat3(&right_w_);
	XMVECTOR v = XMLoadFloat3(&up_w_);
	XMVECTOR w = XMLoadFloat3(&look_w_);

	w = XMVector3Normalize(w);
	u = XMVector3Normalize(XMVector3Cross(v, w));
	v = XMVector3Cross(w, u);

	XMStoreFloat3(&right_w_, u);
	XMStoreFloat3(&up_w_, v);
	XMStoreFloat3(&look_w_, w);

	float x = -XMVectorGetX(XMVector3Dot(p, u));
	float y = -XMVectorGetY(XMVector3Dot(p, v));
	float z = -XMVectorGetZ(XMVector3Dot(p, w));

	view_(0, 0) = right_w_.x;
	view_(1, 0) = right_w_.y;
	view_(2, 0) = right_w_.z;
	view_(3, 0) = x;

	view_(0, 1) = up_w_.x;
	view_(1, 1) = up_w_.y;
	view_(2, 1) = up_w_.z;
	view_(3, 1) = y;

	view_(0, 2) = look_w_.x;
	view_(1, 2) = look_w_.y;
	view_(2, 2) = look_w_.z;
	view_(3, 2) = z;

	view_(0, 3) = 0.0f;
	view_(1, 3) = 0.0f;
	view_(2, 3) = 0.0f;
	view_(3, 3) = 1.0f;
}

void ba::Camera::SetLens(float fov_y, float aspect_ratio, float near_z, float far_z)
{
	near_z_ = near_z;
	far_z_ = far_z;

	fov_y_ = fov_y;
	aspect_ratio_ = aspect_ratio;

	near_window_height_ = 2.0f * tan(0.5f * fov_y_) * near_z_;
	far_window_height_ = 2.0f * tan(0.5f * fov_y_) * far_z_;

	XMMATRIX proj = XMMatrixPerspectiveFovLH(fov_y_, aspect_ratio_, near_z_, far_z_);
	XMStoreFloat4x4(&proj_, proj);
}