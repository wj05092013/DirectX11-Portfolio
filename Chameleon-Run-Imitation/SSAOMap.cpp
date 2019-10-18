#include "stdafx.h"

const UINT ba::SSAOMap::kRandomVectorMapSize = 256;

ba::SSAOMap::SSAOMap() :
	device_(nullptr),
	dc_(nullptr),
	normal_depth_map_srv_(nullptr),
	normal_depth_map_rtv_(nullptr),
	dsv_(nullptr),
	ssao_map_0_srv_(nullptr),
	ssao_map_1_srv_(nullptr),
	ssao_map_0_rtv_(nullptr),
	ssao_map_1_rtv_(nullptr),
	random_vector_map_srv_(nullptr),
	quad_vb_(nullptr),
	quad_ib_(nullptr),
	width_(0),
	height_(0),
	fov_y_(0.0f),
	far_z_(0.0f),
	ssao_viewport_{}
{
}

ba::SSAOMap::~SSAOMap()
{
	Release();
}

bool ba::SSAOMap::Init(
	ID3D11Device* device, ID3D11DeviceContext* dc,
	UINT width, UINT height, float fov_y, float far_z
)
{
	Release();

	device_ = device;
	dc_ = dc;

	width_ = width;
	height_ = height;
	fov_y_ = fov_y;
	far_z_ = far_z;

	if (!BuildFullScreenQuad()) { Release(); return false; }
	if (!BuildNormalDepthMapViews()) { Release(); return false; }
	if (!BuildDepthStencilView()) { Release(); return false; }
	if (!BuildSSAOMapViews()) { Release(); return false; }
	if (!BuildRandomVectorMapView()) { Release(); return false; }

	BuildFarPlaneCornersVectors();
	BuildOffsetVectors();
	BuildSSAOViewport();

	return true;
}

void ba::SSAOMap::Release()
{
	ReleaseFullScreenQuad();
	ReleaseNormalDepthMapViews();
	ReleaseDepthStencilView();
	ReleaseSSAOMapViews();
	ReleaseRandomVectorMapView();
}

bool ba::SSAOMap::OnResize(UINT width, UINT height, float fov_y, float far_z)
{
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;
	far_z_ = far_z;

	ReleaseNormalDepthMapViews();
	ReleaseSSAOMapViews();

	if (!BuildNormalDepthMapViews()) { Release(); return false; }
	if (!BuildDepthStencilView()) { Release(); return false; }
	if (!BuildSSAOMapViews()) { Release(); return false; }

	BuildFarPlaneCornersVectors();
	BuildSSAOViewport();

	return true;
}

void ba::SSAOMap::BuildSSAOMap(const Camera& cam)
{
	// Set render target and viewport.
	//
	ID3D11RenderTargetView* rtv[1] = { ssao_map_0_rtv_ };
	dc_->OMSetRenderTargets(1, rtv, nullptr);

	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dc_->ClearRenderTargetView(ssao_map_0_rtv_, clear_color);

	dc_->RSSetViewports(1, &ssao_viewport_);
	//__

	// Set IA stage.
	//
	dc_->IASetInputLayout(inputvertex::PosNormalTex::kInputLayout);
	dc_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT strides = sizeof(inputvertex::PosNormalTex::Vertex);
	UINT offsets = 0;
	dc_->IASetVertexBuffers(0, 1, &quad_vb_, &strides, &offsets);
	dc_->IASetIndexBuffer(quad_ib_, DXGI_FORMAT_R16_UINT, 0);
	//__

	// Set effect.
	//
	effects::kSSAOMapEffect.SetFarPlaneCorners(far_plane_corners_);
	effects::kSSAOMapEffect.SetOffsetVectors(offset_vectors_);

	XMMATRIX to_tex(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);
	effects::kSSAOMapEffect.SetProj(cam.proj());
	effects::kSSAOMapEffect.SetToTex(to_tex);
	effects::kSSAOMapEffect.SetNormalDepthMap(normal_depth_map_srv_);
	effects::kSSAOMapEffect.SetRandomVectorMap(random_vector_map_srv_);
	//__

	// Render SSAO map.
	//
	ID3DX11EffectTechnique* tech = effects::kSSAOMapEffect.tech(SSAOMapEffect::kSSAOMap);
	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);

	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, dc_);
		dc_->DrawIndexed(6, 0, 0);
	}
	//__
}

void ba::SSAOMap::BlurSSAOMap(UINT blur_count)
{
	for (UINT i = 0; i < blur_count; ++i)
	{
		BlurSSAOMap(ssao_map_0_srv_, ssao_map_1_rtv_, BlurSSAOEffect::TechType::kHorizontalBlur);
		BlurSSAOMap(ssao_map_1_srv_, ssao_map_0_rtv_, BlurSSAOEffect::TechType::kVerticalBlur);
	}
}

ID3D11ShaderResourceView* ba::SSAOMap::normal_depth_map_srv()
{
	return normal_depth_map_srv_;
}

ID3D11RenderTargetView* ba::SSAOMap::normal_depth_map_rtv()
{
	return normal_depth_map_rtv_;
}

ID3D11DepthStencilView* ba::SSAOMap::dsv()
{
	return dsv_;
}

ID3D11ShaderResourceView* ba::SSAOMap::ssao_map_srv()
{
	return ssao_map_0_srv_;
}

void ba::SSAOMap::BlurSSAOMap(ID3D11ShaderResourceView* input_image_srv, ID3D11RenderTargetView* output_image_rtv, BlurSSAOEffect::TechType tech_type)
{
	// Set render target and viewport.
	//
	ID3D11RenderTargetView* rtv[1] = { output_image_rtv };
	dc_->OMSetRenderTargets(1, rtv, nullptr);

	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dc_->ClearRenderTargetView(output_image_rtv, clear_color);

	dc_->RSSetViewports(1, &ssao_viewport_);
	//__

	// Set IA stage.
	//
	dc_->IASetInputLayout(inputvertex::kPosNormalTex.kInputLayout);
	dc_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT strides = sizeof(inputvertex::PosNormalTex::Vertex);
	UINT offsets = 0;
	dc_->IASetVertexBuffers(0, 1, &quad_vb_, &strides, &offsets);
	dc_->IASetIndexBuffer(quad_ib_, DXGI_FORMAT_R16_UINT, 0);
	//__

	// Set effect.
	effects::kBlurSSAOEffect.SetTexelWidth(2.0f / width_);
	effects::kBlurSSAOEffect.SetTexelHeight(2.0f / height_);
	effects::kBlurSSAOEffect.SetNormalDepthMap(normal_depth_map_srv_);
	effects::kBlurSSAOEffect.SetInputImage(input_image_srv);

	// Blur the SSAO map.
	//
	ID3DX11EffectTechnique* tech = effects::kBlurSSAOEffect.tech(tech_type);
	D3DX11_TECHNIQUE_DESC tech_desc;
	tech->GetDesc(&tech_desc);

	for (UINT p = 0; p < tech_desc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, dc_);
		dc_->DrawIndexed(6, 0, 0);

		// Unbind the input SRV as it is going to be an output in the next blur.
		effects::kBlurSSAOEffect.SetInputImage(nullptr);
		tech->GetPassByIndex(p)->Apply(0, dc_);
	}
	//__
}

bool ba::SSAOMap::BuildFullScreenQuad()
{
	inputvertex::PosNormalTex::Vertex vertices[4];

	vertices[0].pos = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[1].pos = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[2].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[3].pos = XMFLOAT3(1.0f, -1.0f, 0.0f);

	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertices[1].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[2].normal = XMFLOAT3(2.0f, 0.0f, 0.0f);
	vertices[3].normal = XMFLOAT3(3.0f, 0.0f, 0.0f);

	vertices[0].uv = XMFLOAT2(0.0f, 0.0f);
	vertices[1].uv = XMFLOAT2(1.0f, 0.0f);
	vertices[2].uv = XMFLOAT2(0.0f, 1.0f);
	vertices[3].uv = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vb_desc{};
	vb_desc.ByteWidth = sizeof(inputvertex::PosNormalTex::Vertex) * 4;
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_init{};
	vb_init.pSysMem = vertices;

	HRESULT res = device_->CreateBuffer(&vb_desc, &vb_init, &quad_vb_);
	if (FAILED(res))
		return false;


	USHORT indices[6] =
	{
		0, 1, 2,
		2, 1, 3
	};

	D3D11_BUFFER_DESC ib_desc{};
	ib_desc.ByteWidth = sizeof(USHORT) * 6;
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_init{};
	ib_init.pSysMem = indices;

	res = device_->CreateBuffer(&ib_desc, &ib_init, &quad_ib_);
	if (FAILED(res))
		return false;

	return true;
}

bool ba::SSAOMap::BuildNormalDepthMapViews()
{
	D3D11_TEXTURE2D_DESC tex_desc{};
	tex_desc.Width = width_;
	tex_desc.Height = height_;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	ID3D11Texture2D* tex = nullptr;
	HRESULT res = device_->CreateTexture2D(&tex_desc, nullptr, &tex);
	if (FAILED(res))
		return false;

	res = device_->CreateShaderResourceView(tex, nullptr, &normal_depth_map_srv_);
	if (FAILED(res))
	{
		ReleaseCOM(tex);
		return false;
	}

	res = device_->CreateRenderTargetView(tex, nullptr, &normal_depth_map_rtv_);
	ReleaseCOM(tex);
	if (FAILED(res))
		return false;

	return true;
}

bool ba::SSAOMap::BuildDepthStencilView()
{
	D3D11_TEXTURE2D_DESC tex_desc{};
	tex_desc.Width = width_;
	tex_desc.Height = height_;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* tex = nullptr;
	HRESULT res = device_->CreateTexture2D(&tex_desc, nullptr, &tex);
	if (FAILED(res))
		return false;

	res = device_->CreateDepthStencilView(tex, nullptr, &dsv_);
	ReleaseCOM(tex);
	if (FAILED(res))
		return false;

	return true;
}

bool ba::SSAOMap::BuildSSAOMapViews()
{
	D3D11_TEXTURE2D_DESC tex_desc{};
	tex_desc.Width = width_ / 2;
	tex_desc.Height = height_ / 2;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R16_FLOAT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;


	ID3D11Texture2D* tex_0 = nullptr;
	HRESULT res = device_->CreateTexture2D(&tex_desc, nullptr, &tex_0);
	if (FAILED(res))
		return false;

	res = device_->CreateShaderResourceView(tex_0, nullptr, &ssao_map_0_srv_);
	if (FAILED(res))
	{
		ReleaseCOM(tex_0);
		return false;
	}

	res = device_->CreateRenderTargetView(tex_0, nullptr, &ssao_map_0_rtv_);
	ReleaseCOM(tex_0);
	if (FAILED(res))
		return false;


	ID3D11Texture2D* tex_1 = nullptr;
	res = device_->CreateTexture2D(&tex_desc, nullptr, &tex_1);
	if (FAILED(res))
		return false;

	res = device_->CreateShaderResourceView(tex_1, nullptr, &ssao_map_1_srv_);
	if (FAILED(res))
	{
		ReleaseCOM(tex_1);
		return false;
	}

	res = device_->CreateRenderTargetView(tex_1, nullptr, &ssao_map_1_rtv_);
	ReleaseCOM(tex_1);
	if (FAILED(res))
		return false;

	return true;
}

bool ba::SSAOMap::BuildRandomVectorMapView()
{
	std::vector<XMVECTOR> vec(kRandomVectorMapSize * kRandomVectorMapSize);

	for (UINT i = 0; i < kRandomVectorMapSize; ++i)
	{
		for (UINT j = 0; j < kRandomVectorMapSize; ++j)
		{
			vec[i * kRandomVectorMapSize + j]
				= XMVectorSet(mathhelper::RandF(), mathhelper::RandF(), mathhelper::RandF(), 0.0f);
		}
	}

	D3D11_TEXTURE2D_DESC tex_desc{};
	tex_desc.Width = kRandomVectorMapSize;
	tex_desc.Height = kRandomVectorMapSize;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA init_tex{};
	init_tex.pSysMem = &vec[0];
	init_tex.SysMemPitch = sizeof(XMVECTOR) * kRandomVectorMapSize;

	ID3D11Texture2D* tex = nullptr;
	HRESULT res = device_->CreateTexture2D(&tex_desc, &init_tex, &tex);
	if (FAILED(res))
		return false;

	res = device_->CreateShaderResourceView(tex, nullptr, &random_vector_map_srv_);
	ReleaseCOM(tex);
	if (FAILED(res))
		return false;

	return true;
}

void ba::SSAOMap::BuildFarPlaneCornersVectors()
{
	float aspect = float(width_) / float(height_);

	float half_height = far_z_ * tanf(0.5f * fov_y_);
	float half_width = half_height * aspect;

	far_plane_corners_[0] = XMFLOAT4(-half_width, half_height, far_z_, 0.0f);
	far_plane_corners_[1] = XMFLOAT4(half_width, half_height, far_z_, 0.0f);
	far_plane_corners_[2] = XMFLOAT4(-half_width, -half_height, far_z_, 0.0f);
	far_plane_corners_[3] = XMFLOAT4(half_width, -half_height, far_z_, 0.0f);
}

void ba::SSAOMap::BuildOffsetVectors()
{
	// 8 cube corners
	offset_vectors_[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	offset_vectors_[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

	offset_vectors_[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	offset_vectors_[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

	offset_vectors_[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	offset_vectors_[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

	offset_vectors_[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	offset_vectors_[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6 centers of cube faces
	offset_vectors_[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	offset_vectors_[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

	offset_vectors_[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	offset_vectors_[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

	offset_vectors_[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	offset_vectors_[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);

	for (int i = 0; i < 14; ++i)
	{
		float s = mathhelper::RandF(0.25f, 1.0f);

		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&offset_vectors_[i]));

		XMStoreFloat4(&offset_vectors_[i], v);
	}
}

void ba::SSAOMap::BuildSSAOViewport()
{
	ssao_viewport_.TopLeftX = 0.0f;
	ssao_viewport_.TopLeftY = 0.0f;
	ssao_viewport_.Width = 0.5f * width_;
	ssao_viewport_.Height = 0.5f * height_;
	ssao_viewport_.MinDepth = 0.0f;
	ssao_viewport_.MaxDepth = 1.0f;
}

void ba::SSAOMap::ReleaseFullScreenQuad()
{
	ReleaseCOM(quad_vb_);
	ReleaseCOM(quad_ib_);
}

void ba::SSAOMap::ReleaseNormalDepthMapViews()
{
	ReleaseCOM(normal_depth_map_srv_);
	ReleaseCOM(normal_depth_map_rtv_);
}

void ba::SSAOMap::ReleaseDepthStencilView()
{
	ReleaseCOM(dsv_);
}

void ba::SSAOMap::ReleaseSSAOMapViews()
{
	ReleaseCOM(ssao_map_0_srv_);
	ReleaseCOM(ssao_map_1_srv_);
	ReleaseCOM(ssao_map_0_rtv_);
	ReleaseCOM(ssao_map_1_rtv_);
}

void ba::SSAOMap::ReleaseRandomVectorMapView()
{
	ReleaseCOM(random_vector_map_srv_);
}
