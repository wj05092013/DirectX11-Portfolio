#include "stdafx.h"

namespace ba
{
	Particle::Particle() :
		device_(nullptr),
		effect_(nullptr),

		max_particles_(0),
		first_run_(true),
		game_time_(0.0f),
		delta_time_(0.0f),
		age_(0.0f),
		emit_pos_(0.0f, 0.0f, 0.0f),

		init_vb_(nullptr),
		input_vb_(nullptr),
		stream_out_vb_(nullptr),
		tex_arr_srv_(nullptr),
		rand_tex_srv_(nullptr),
		tex_arr_size_(0)
	{
	}

	void Particle::Update(float game_time, float delta_time)
	{
		game_time_ = game_time;
		delta_time_ = delta_time;

		age_ += delta_time_;
	}

	void Particle::Draw(ID3D11DeviceContext* dc, const Camera* camera)
	{
		// Set effect.
		effect_->SetEyePos(camera->position_w_xf());
		effect_->SetView(camera->view());
		effect_->SetProj(camera->proj());
		effect_->SetEmitPos(emit_pos_);
		effect_->SetGameTime(game_time_);
		effect_->SetDeltaTime(delta_time_);
		effect_->SetTexArrSize(tex_arr_size_);
		effect_->SetTexArr(tex_arr_srv_);
		effect_->SetRandTex(rand_tex_srv_);

		// Set input assembler state.
		dc->IASetInputLayout(inputvertex::Particle::kInputLayout);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		UINT strides = sizeof(inputvertex::Particle::Vertex);
		UINT offsets = 0;
		if (first_run_)
			dc->IASetVertexBuffers(0, 1, &init_vb_, &strides, &offsets);
		else
			dc->IASetVertexBuffers(0, 1, &input_vb_, &strides, &offsets);

		// Set stream-out buffer.
		dc->SOSetTargets(1, &stream_out_vb_, &offsets);

		// Perform stream-out technique.
		//
		ID3DX11EffectTechnique* tech = effect_->tech(ParticleEffect::kStreamOut);
		D3DX11_TECHNIQUE_DESC tech_desc;
		tech->GetDesc(&tech_desc);

		for (UINT p = 0; p < tech_desc.Passes; ++p)
		{
			tech->GetPassByIndex(p)->Apply(0, dc);

			if (first_run_)
			{
				dc->Draw(1, 0);
				first_run_ = false;
			}
			else
				dc->DrawAuto();

			dc->OMSetDepthStencilState(nullptr, 0);
		}
		//__

		// Unbind stream-out vertex buffer.
		ID3D11Buffer* empty_vb[1] = { nullptr };
		dc->SOSetTargets(1, empty_vb, &offsets);

		// Set stream-out vertex buffer to input assembler state.
		std::swap(input_vb_, stream_out_vb_);
		dc->IASetVertexBuffers(0, 1, &input_vb_, &strides, &offsets);

		// Perform drawing technique.
		tech = effect_->tech(ParticleEffect::kDrawing);
		tech->GetDesc(&tech_desc);

		float blend_factor[4]{};
		for (UINT p = 0; p < tech_desc.Passes; ++p)
		{
			tech->GetPassByIndex(p)->Apply(0, dc);

			dc->DrawAuto();
			dc->OMSetBlendState(nullptr, blend_factor, 0xffffffff);
			dc->OMSetDepthStencilState(nullptr, 0);
		}
		//__
	}

	Particle::~Particle()
	{
		Destroy();
	}

	bool Particle::Init(
		ID3D11Device* device, ParticleEffect* effect,
		UINT max_particles,
		ID3D11ShaderResourceView* tex_arr_srv, int tex_arr_size,
		ID3D11ShaderResourceView* rand_tex_srv
	)
	{
		Destroy();
		
		device_ = device;
		effect_ = effect;

		max_particles_ = max_particles;
		first_run_ = true;
		game_time_ = 0.0f;
		delta_time_ = 0.0f;
		age_ = 0.0f;
		emit_pos_ = XMFLOAT3(0.0f, 0.0f, 0.0f);

		tex_arr_srv_ = tex_arr_srv;
		rand_tex_srv_ = rand_tex_srv;
		tex_arr_size_ = tex_arr_size;

		if (!BuildVertexBuffers()) { Destroy(); return false; }
		return true;
	}

	void Particle::Destroy()
	{
		DestroyCOM(init_vb_);
		DestroyCOM(input_vb_);
		DestroyCOM(stream_out_vb_);
	}

	void Particle::Reset()
	{
		first_run_ = true;
		delta_time_ = 0.0f;
		age_ = 0.0f;

		Destroy();
		BuildVertexBuffers();
	}

	void Particle::set_emit_pos_xf(const XMFLOAT3& pos)
	{
		emit_pos_ = pos;
	}

	void Particle::set_emit_pos_xv(const XMVECTOR& pos)
	{
		XMStoreFloat3(&emit_pos_, pos);
	}

	bool Particle::BuildVertexBuffers()
	{
		inputvertex::Particle::Vertex emitter_particle;
		emitter_particle.age = 0.0f;
		emitter_particle.type = 0;

		std::vector<inputvertex::Particle::Vertex> vertices(0);
		vertices.push_back(emitter_particle);

		// Create a vertex buffer containing emitter particle only.
		//
		D3D11_BUFFER_DESC vb_desc{};
		vb_desc.ByteWidth = sizeof(inputvertex::Particle::Vertex) * 1;
		vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
		vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vb_init{};
		vb_init.pSysMem = &vertices[0];

		HRESULT res = device_->CreateBuffer(&vb_desc, &vb_init, &init_vb_);
		if (FAILED(res))
			return false;

		// Create vertex buffers for the IA state and the stream-out.
		//
		vb_desc.ByteWidth = sizeof(inputvertex::Particle::Vertex) * max_particles_;
		vb_desc.Usage = D3D11_USAGE_DEFAULT;
		vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

		res = device_->CreateBuffer(&vb_desc, nullptr, &input_vb_);
		if (FAILED(res))
			return false;

		res = device_->CreateBuffer(&vb_desc, nullptr, &stream_out_vb_);
		if (FAILED(res))
			return false;

		return true;
	}

}