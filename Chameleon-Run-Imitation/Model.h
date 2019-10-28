#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class ModelData
	{
	public:
		enum EColorType
		{
			kDefault,
			kRed,
			kYellow,
			kBlack
		};

	public:
		ModelData();

		bool Init(ID3D11Device* device, const GeometryGenerator::Geometry& geo, const XMMATRIX& local_transform, EColorType type);

		// Change color type and the model's material automatically.
		void ChangeColor(EColorType type);

		Mesh mesh;
		ID3D11ShaderResourceView* diffuse_map;
		EColorType color_type;
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
		Model(ModelData* model_data, Timer* timer);
		Model(ModelData* model_data, Timer* timer, EModelType type);
		virtual ~Model();

		// If a collider for this model was created, this function would be called on collision.
		virtual void OnCollision(const collision::CollisionInfo& info);

		// Should update the world transform matrix manually after changing the scaling, rotation and translation of thins model.
		void RecalculateWorldTransform();

		virtual void Update();

		//
		// Mutators
		//

		void set_model_data(ModelData* model_data);
		void set_scale(const XMFLOAT3& scale);
		void set_scale(const XMVECTOR& scale);
		void set_rotation(const XMFLOAT4& rotation);
		void set_rotation(const XMVECTOR& rotation);
		void set_translation(const XMFLOAT3& translation);
		void set_translation(const XMVECTOR& translation);

		//
		// Accessors
		//

		const ModelData* model_data() const;
		const XMFLOAT3& scale_xf() const;
		const XMVECTOR scale_xv() const;
		const XMFLOAT4& rotation_xf() const;
		const XMVECTOR rotation_xv() const;
		const XMFLOAT3& translation_xf() const;
		const XMVECTOR translation_xv() const;
		const XMMATRIX& local_world() const;
		EModelType model_type() const;

	protected:
		ModelData*	model_data_;
		
		XMFLOAT3	scale_;
		XMFLOAT4	rotation_;
		XMFLOAT3	translation_;
		XMMATRIX	local_world_;

		Timer* timer_;

	private:
		EModelType	model_type_;
	};
}
