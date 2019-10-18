#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Model
	{
	public:
		Model();

		bool Init(ID3D11Device* device, const GeometryGenerator::Geometry& geo, const XMMATRIX& local_transform, const light::Material& material);

		Mesh mesh;
		ID3D11ShaderResourceView* diffuse_map;
	};

	class ModelInstance
	{
	public:
		ModelInstance();

		Model*		model;
		XMMATRIX	scale;
		XMMATRIX	rotation;
		XMMATRIX	translation;
	};
}
