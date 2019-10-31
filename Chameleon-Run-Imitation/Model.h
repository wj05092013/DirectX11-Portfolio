#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Scene;

	class ModelData
	{
	public:
		ModelData();

		bool Init(ID3D11Device* device, const GeometryGenerator::Geometry& geo, const XMMATRIX& local_transform);

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

		enum EColorType
		{
			kDefault,
			kRed,
			kYellow,
			kBlack
		};

	public:
		Model(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer);
		Model(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer, EModelType type);
		virtual ~Model();

		// If a collider for this model was created, this function would be called on collision.
		virtual void OnCollision(const physics::CollisionInfo& info);

		// Should update the world transform matrix manually after changing the scaling, rotation and translation of thins model.
		void RecalculateWorldTransform();

		virtual void Update();

		//
		// Mutators
		//

		// Change color type and the model's material automatically.
		void set_color_type(EColorType type);

		// Calling this function is not necessary if 'set_color_type' has been called.
		void set_material(const light::Material& material);

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

		const std::string& name() const;
		EColorType color_type() const;
		const light::Material& material() const;
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
		std::string name_;
		EColorType color_type_;
		light::Material material_;

		ModelData*	model_data_;
		
		XMFLOAT3	scale_;
		XMFLOAT4	rotation_;
		XMFLOAT3	translation_;
		XMMATRIX	local_world_;

		Scene* scene_;
		Timer* timer_;

	private:
		EModelType	model_type_;
	};
}
