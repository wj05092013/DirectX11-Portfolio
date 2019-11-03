#include "stdafx.h"
#include "Constants.h"

namespace ba
{
	//
	// ModelData class
	//

	ModelData::ModelData() :
		diffuse_map(nullptr)
	{
	}

	bool ModelData::Init(ID3D11Device* device, const geometrygenerator::Geometry& geo, const XMMATRIX& local_transform)
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

		return true;
	}

	//
	// Model class
	//

	Model::Model(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer) :
		name_(name),
		color_type_(kDefault),
		material_(game::kMaterials[color_type_]),

		model_data_(model_data),
		scale_(0.0f, 0.0f, 0.0f),
		rotation_(0.0f, 0.0f, 0.0f, 1.0f),
		translation_(0.0f, 0.0f, 0.0f),
		local_world_(XMMatrixIdentity()),
		scene_(scene),
		timer_(timer),
		model_type_(kStatic)
	{
	}

	Model::Model(const std::string& name, ModelData* model_data, Scene* scene, Timer* timer, EModelType type) :
		name_(name),
		color_type_(kDefault),
		material_(game::kMaterials[color_type_]),

		model_data_(model_data),
		scale_(0.0f, 0.0f, 0.0f),
		rotation_(0.0f, 0.0f, 0.0f, 1.0f),
		translation_(0.0f, 0.0f, 0.0f),
		local_world_(XMMatrixIdentity()),
		scene_(scene),
		timer_(timer),
		model_type_(type)
	{
	}

	Model::~Model()
	{
	}

	void Model::OnCollision(const physics::CollisionInfo& info)
	{
		// Do nothing.
	}

	void Model::RecalculateWorldTransform()
	{
		local_world_ = model_data_->mesh.local_transform()
			* XMMatrixScalingFromVector(XMLoadFloat3(&scale_))
			* XMMatrixRotationQuaternion(XMLoadFloat4(&rotation_))
			* XMMatrixTranslationFromVector(XMLoadFloat3(&translation_));
	}

	void Model::Update()
	{
		// Do nothing.
	}

	void Model::set_color_type(EColorType type)
	{
		color_type_ = type;
		material_ = game::kMaterials[color_type_];
	}

	void Model::set_material(const light::Material& material)
	{
		material_ = material;
	}

	void Model::set_model_data(ModelData* model_data)
	{
		model_data_ = model_data;
	}

	void Model::set_scale(const XMFLOAT3& scale)
	{
		scale_ = scale;
	}

	void Model::set_scale(const XMVECTOR& scale)
	{
		XMStoreFloat3(&scale_, scale);
	}

	void Model::set_rotation(const XMFLOAT4& rotation)
	{
		rotation_ = rotation;
	}

	void Model::set_rotation(const XMVECTOR& rotation)
	{
		XMStoreFloat4(&rotation_, rotation);
	}

	void Model::set_translation(const XMFLOAT3& translation)
	{
		translation_ = translation;
	}

	void Model::set_translation(const XMVECTOR& translation)
	{
		XMStoreFloat3(&translation_, translation);
	}

	const std::string& Model::name() const
	{
		return name_;
	}

	Model::EColorType Model::color_type() const
	{
		return color_type_;
	}

	const light::Material& Model::material() const
	{
		return material_;
	}

	const ModelData* Model::model_data() const
	{
		return model_data_;
	}

	const XMFLOAT3& Model::scale_xf() const
	{
		return scale_;
	}

	const XMVECTOR Model::scale_xv() const
	{
		return XMLoadFloat3(&scale_);
	}

	const XMFLOAT4& Model::rotation_xf() const
	{
		return rotation_;
	}

	const XMVECTOR Model::rotation_xv() const
	{
		return XMLoadFloat4(&rotation_);
	}

	const XMFLOAT3& Model::translation_xf() const
	{
		return translation_;
	}

	const XMVECTOR Model::translation_xv() const
	{
		return XMLoadFloat3(&translation_);
	}

	const XMMATRIX& Model::local_world() const
	{
		return local_world_;
	}

	Model::EModelType Model::model_type() const
	{
		return model_type_;
	}
}
