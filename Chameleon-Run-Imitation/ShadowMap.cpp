#include "stdafx.h"

ba::ShadowMap::ShadowMap() :
	device_(nullptr),
	width_(0),
	height_(0),
	viewport_{},
	srv_(nullptr),
	dsv_(nullptr)
{
}

ba::ShadowMap::~ShadowMap()
{
}

bool ba::ShadowMap::Init(ID3D11Device* device, UINT width, UINT height)
{
	Release();

	device_ = device;

	width_ = width;
	height_ = height;

	if (!BuildShadowMapViews()) { Release(); return false; }
	BuildShadowMapViewport();

	return true;
}

void ba::ShadowMap::Release()
{
	ReleaseCOM(srv_);
	ReleaseCOM(dsv_);
}

void ba::ShadowMap::BuildShadowTransform(const ba::light::DirectionalLight& lit, const ba::ShadowMap::BoundingSphere& scene_bounds)
{
	// Build view matrix of the directional light.
	//
	XMVECTOR lit_dir = XMVector3Normalize(XMLoadFloat3(&lit.direction));
	XMVECTOR lit_pos = -2.0f * scene_bounds.radius * lit_dir;
	XMVECTOR lit_target = XMLoadFloat3(&scene_bounds.center);
	XMVECTOR lit_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(lit_pos, lit_target, lit_up);

	// Transform bounding sphere to light space.
	XMFLOAT3 center_lh;
	XMStoreFloat3(&center_lh, XMVector3TransformCoord(lit_target, view));

	// Build orthographic projection matrix.
	//
	float l = center_lh.x - scene_bounds.radius;
	float r = center_lh.x + scene_bounds.radius;
	float b = center_lh.y - scene_bounds.radius;
	float t = center_lh.y + scene_bounds.radius;
	float n = center_lh.z - scene_bounds.radius;
	float f = center_lh.z + scene_bounds.radius;
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Build a matrix for transforming NDC space to texture space.
	//
	XMMATRIX to_tex(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	XMStoreFloat4x4(&view_, view);
	XMStoreFloat4x4(&proj_, proj);
	XMStoreFloat4x4(&world_to_tex_, view * proj * to_tex);
}

DirectX::XMMATRIX ba::ShadowMap::view() const
{
	return XMLoadFloat4x4(&view_);
}

DirectX::XMMATRIX ba::ShadowMap::proj() const
{
	return XMLoadFloat4x4(&proj_);
}

DirectX::XMMATRIX ba::ShadowMap::world_to_tex() const
{
	return XMLoadFloat4x4(&world_to_tex_);
}

const D3D11_VIEWPORT& ba::ShadowMap::viewport()
{
	return viewport_;
}

ID3D11ShaderResourceView* ba::ShadowMap::srv()
{
	return srv_;
}

ID3D11DepthStencilView* ba::ShadowMap::dsv()
{
	return dsv_;
}

bool ba::ShadowMap::BuildShadowMapViews()
{
	// Create a shadow map texture.
	//
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width = width_;
	tex_desc.Height = height_;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;

	ID3D11Texture2D* tex = nullptr;
	HRESULT res = device_->CreateTexture2D(&tex_desc, nullptr, &tex);
	if (FAILED(res))
		return false;

	// Create SRV for the texture.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	srv_desc.Texture2D.MostDetailedMip = 0;

	res = device_->CreateShaderResourceView(tex, &srv_desc, &srv_);
	if (FAILED(res))
	{
		ReleaseCOM(tex);
		return false;
	}

	// Create DSV for the texture.
	//
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Flags = 0;
	dsv_desc.Texture2D.MipSlice = 0;

	res = device_->CreateDepthStencilView(tex, &dsv_desc, &dsv_);
	ReleaseCOM(tex);
	if (FAILED(res))
		return false;

	return true;
}

void ba::ShadowMap::BuildShadowMapViewport()
{
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.Width = static_cast<float>(width_);
	viewport_.Height = static_cast<float>(height_);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}
