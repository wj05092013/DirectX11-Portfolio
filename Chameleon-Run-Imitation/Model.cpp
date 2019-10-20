#include "stdafx.h"


//
// Model class
//

ba::Model::Model() :
	diffuse_map(nullptr)
{
}

bool ba::Model::Init(ID3D11Device* device, const GeometryGenerator::Geometry& geo, const XMMATRIX& local_transform, const light::Material& material)
{
	UINT vtx_count = geo.vertices.size();

	std::vector<inputvertex::PosNormalTexTangent::Vertex> vertices(vtx_count);
	for (UINT i = 0; i < vtx_count; ++i)
	{
		vertices[i].pos = geo.vertices[i].pos;
		vertices[i].normal = geo.vertices[i].normal;
		vertices[i].uv = geo.vertices[i].uv;
		vertices[i].tangent = geo.vertices[i].tangent;
	}

	std::vector<UINT> indices;
	indices.insert(indices.begin(), geo.indices.begin(), geo.indices.end());

	if (!mesh.BuildVertexBuffer(device, vertices))
		return false;
	if (!mesh.BuildIndexBuffer(device, indices))
		return false;
	mesh.set_transform(local_transform);
	mesh.set_material(material);

	return true;
}


//
// ModelInstance class
//

ba::ModelInstance::ModelInstance() :
	model(nullptr),
	scale(XMMatrixIdentity()),
	rotation(XMMatrixIdentity()),
	translation(XMMatrixIdentity())
{
}
