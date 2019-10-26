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
		enum EModelType
		{
			kStatic,
			kPhysics
		};

	public:
		Model();
		Model(EModelType type);
		virtual ~Model();

		// If a collider for this model was created, this function would be called on collision.
		virtual void OnCollision(const collision::CollisionInfo& info);

		// Update the world transform matrix using the scale, rotation, translation vectors.
		void CalcWorldTransform();

		EModelType model_type() const;

	public:
		ModelData*	model_data;
		
		XMVECTOR	scale;
		XMVECTOR	rotation;
		XMVECTOR	translation;
		XMMATRIX	world_transform;

	private:
		EModelType	model_type_;
	};
}
