#include "stdafx.h"

//
// Mesh
//

ba::Mesh::Mesh() :
	vb_(nullptr),
	vertex_stride_(0),
	ib_(nullptr),
	idx_count_(0),
	local_transform_(XMMatrixIdentity()),
	material_{}
{
}

ba::Mesh::~Mesh()
{
	ReleaseCOM(vb_);
	ReleaseCOM(ib_);
}

void ba::Mesh::Draw(ID3D11DeviceContext* dc) const
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &vb_, &vertex_stride_, &offset);
	dc->IASetIndexBuffer(ib_, DXGI_FORMAT_R32_UINT, 0);

	dc->DrawIndexed(idx_count_, 0, 0);
}

bool ba::Mesh::BuildIndexBuffer(ID3D11Device* device, const std::vector<UINT> indices)
{
	ID3D11Buffer* old_ib = ib_;

	D3D11_BUFFER_DESC ib_desc{};
	ib_desc.ByteWidth = indices.size() * sizeof(UINT);
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA ib_init{};
	ib_init.pSysMem = &indices[0];

	HRESULT res = device->CreateBuffer(&ib_desc, &ib_init, &ib_);
	if (FAILED(res))
	{
		ib_ = old_ib;
		return false;
	}

	ReleaseCOM(old_ib);
	idx_count_ = indices.size();

	return true;
}

void ba::Mesh::set_transform(const XMMATRIX& matrix)
{
	local_transform_ = matrix;
}

const DirectX::XMMATRIX& ba::Mesh::transform() const
{
	return local_transform_;
}

void ba::Mesh::set_material(const light::Material& material)
{
	material_ = material;
}

const ba::light::Material& ba::Mesh::material() const
{
	return material_;
}
