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
		Model(ModelData* model_data);
		Model(ModelData* model_data, EModelType type);
		virtual ~Model();

		// If a collider for this model was created, this function would be called on collision.
		virtual void OnCollision(const collision::CollisionInfo& info);

		// Should update the world transform matrix manually after changing the scaling, rotation and translation of thins model.
		void RecalculateWorldTransform();

		virtual void Update(float delta_time);

		//
		// Mutators
		//

		void set_model_data(ModelData* model_data);
		void set_scale(const XMVECTOR& scale);
		void set_rotation(const XMVECTOR& rotation);
		void set_translation(const XMVECTOR& translation);

		//
		// Accessors
		//

		const ModelData* model_data() const;
		const XMVECTOR& scale() const;
		const XMVECTOR& rotation() const;
		const XMVECTOR& translation() const;
		const XMMATRIX& local_world() const;
		EModelType model_type() const;

	protected:
		ModelData*	model_data_;
		
		XMVECTOR	scale_;
		XMVECTOR	rotation_;
		XMVECTOR	translation_;
		XMMATRIX	local_world_;

	private:
		EModelType	model_type_;
	};
}
