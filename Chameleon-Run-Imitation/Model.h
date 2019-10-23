#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class ModelData
	{
	public:
		ModelData();

		bool Init(ID3D11Device* device, const GeometryGenerator::Geometry& geo, const XMMATRIX& local_transform, const light::Material& material);

		Mesh mesh;
		ID3D11ShaderResourceView* diffuse_map;
	};

	class Model
	{
	public:
		Model();
		virtual ~Model();

		void CalcTransform();

		ModelData*	model_data;
		XMVECTOR	scale;
		XMVECTOR	rotation;
		XMVECTOR	translation;
		XMMATRIX	transform;
	};
}
