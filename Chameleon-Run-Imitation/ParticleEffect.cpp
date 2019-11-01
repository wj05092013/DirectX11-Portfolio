#include "stdafx.h"

namespace ba
{
	ParticleEffect::ParticleEffect() :
		techs_(nullptr),
		
		eye_pos_(nullptr),
		view_(nullptr),
		emit_pos_(nullptr),
		game_time_(nullptr),
		delta_time_(nullptr),

		proj_(nullptr),

		tex_arr_size_(nullptr),
		tex_arr_(nullptr),
		rand_tex_(nullptr)
	{
	}

	ParticleEffect::~ParticleEffect()
	{
		ParticleEffect::Destroy();
	}

	bool ParticleEffect::Init(ID3D11Device* device, const std::wstring& file_name)
	{
		if (techs_)
			return false;

		if (!EffectWrapper::Init(device, file_name))
			return false;

		ID3DX11Effect* e = effect();

		techs_ = new ID3DX11EffectTechnique * [ETechType::kMax];

		techs_[kStreamOut] = e->GetTechniqueByName("StreamOutTech");
		techs_[kDrawing] = e->GetTechniqueByName("DrawingTech");
		
		eye_pos_ = e->GetVariableByName("eye_pos")->AsVector();
		view_ = e->GetVariableByName("view")->AsMatrix();
		emit_pos_ = e->GetVariableByName("emit_pos")->AsVector();
		game_time_ = e->GetVariableByName("game_time")->AsScalar();
		delta_time_ = e->GetVariableByName("delta_time")->AsScalar();

		proj_ = e->GetVariableByName("proj")->AsMatrix();

		tex_arr_size_ = e->GetVariableByName("tex_arr_size")->AsScalar();
		tex_arr_ = e->GetVariableByName("tex_arr")->AsShaderResource();
		rand_tex_ = e->GetVariableByName("rand_tex")->AsShaderResource();

		return true;
	}

	void ParticleEffect::Destroy()
	{
		if (techs_)
		{
			delete[]techs_;
			techs_ = nullptr;
		}

		EffectWrapper::Destroy();
	}

	ID3DX11EffectTechnique* ParticleEffect::tech(ETechType type)
	{
		return techs_[type];
	}

	void ParticleEffect::SetEyePos(const XMFLOAT3& pos)
	{
		eye_pos_->SetRawValue(&pos, 0, sizeof(XMFLOAT3));
	}

	void ParticleEffect::SetView(const XMMATRIX& matrix)
	{
		view_->SetMatrix(reinterpret_cast<const float*>(&matrix));
	}

	void ParticleEffect::SetEmitPos(const XMFLOAT3& pos)
	{
		emit_pos_->SetRawValue(&pos, 0, sizeof(XMFLOAT3));
	}

	void ParticleEffect::SetGameTime(float time)
	{
		game_time_->SetFloat(time);
	}

	void ParticleEffect::SetDeltaTime(float time)
	{
		delta_time_->SetFloat(time);
	}

	void ParticleEffect::SetProj(const XMMATRIX& matrix)
	{
		proj_->SetMatrix(reinterpret_cast<const float*>(&matrix));
	}

	void ParticleEffect::SetTexArrSize(int size)
	{
		tex_arr_size_->SetInt(size);
	}

	void ParticleEffect::SetTexArr(ID3D11ShaderResourceView* srv)
	{
		tex_arr_->SetResource(srv);
	}

	void ParticleEffect::SetRandTex(ID3D11ShaderResourceView* srv)
	{
		rand_tex_->SetResource(srv);
	}
}
