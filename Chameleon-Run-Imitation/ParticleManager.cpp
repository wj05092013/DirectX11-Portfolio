#include "stdafx.h"

namespace ba
{
	ParticleManager::ParticleManager() :
		device_(nullptr),
		dc_(nullptr),
		timer_(nullptr)
	{
	}

	ParticleManager& ParticleManager::GetInstance()
	{
		static ParticleManager instance;
		return instance;
	}

	ParticleManager::~ParticleManager()
	{
		Destroy();
	}

	bool ParticleManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc, Timer* timer)
	{
		if (device_ || dc_)
			return false;

		device_ = device;
		dc_ = dc;
		timer_ = timer;

		return true;
	}

	void ParticleManager::Destroy()
	{
		device_ = nullptr;
		dc_ = nullptr;

		for (UINT i = 0; i < particles_.size(); ++i)
		{
			delete particles_[i];
		}
		particles_.clear();
	}

	bool ParticleManager::CreateParticle(ParticleEffect* effect, UINT max_particles, ID3D11ShaderResourceView* tex_arr_srv, int tex_arr_size, ID3D11ShaderResourceView* rand_tex_srv, Particle** out_particle)
	{
		if (!device_ || !dc_)
			return false;

		Particle* particle = new Particle();

		if (!particle->Init(device_, effect, max_particles, tex_arr_srv, tex_arr_size, rand_tex_srv))
		{
			delete particle;
			return false;
		}

		particles_.push_back(particle);
		*out_particle = particle;

		return true;
	}

	void ParticleManager::ResetParticles()
	{
		for (UINT i = 0; i < particles_.size(); ++i)
		{
			particles_[i]->Reset();
		}
	}

	void ParticleManager::UpdateParticles()
	{
		for (UINT i = 0; i < particles_.size(); ++i)
		{
			particles_[i]->Update(static_cast<float>(timer_->get_total_time()), static_cast<float>(timer_->get_delta_time()));
		}
	}

	void ParticleManager::DrawParticles(const Camera* camera)
	{
		for (UINT i = 0; i < particles_.size(); ++i)
		{
			particles_[i]->Draw(dc_, camera);
		}
	}
}