#include "stdafx.h"


//
// ModelData class
//

ba::ModelData::ModelData() :
	diffuse_map(nullptr)
{
}

bool ba::ModelData::Init(ID3D11Device* device, const GeometryGenerator::Geometry& geo, const XMMATRIX& local_transform, const light::Material& material)
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
	mesh.set_local_transform(local_transform);
	mesh.set_material(material);

	return true;
}


//
// Model class
//

ba::Model::Model() :
	model_data(nullptr),
	scale(XMVectorZero()),
	rotation(XMVectorZero()),
	translation(XMVectorZero()),
	world_transform(XMMatrixIdentity())
{
}

ba::Model::~Model()
{
}

void ba::Model::CalcWorldTransform()
{
	world_transform = XMMatrixScalingFromVector(scale)
		* XMMatrixRotationQuaternion(rotation)
		* XMMatrixTranslationFromVector(translation);
}
