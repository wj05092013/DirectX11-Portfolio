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

		for (auto iter = particles_.begin(); iter != particles_.end(); ++iter)
		{
			delete (*iter);
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

		particles_.insert(particle);
		*out_particle = particle;

		return true;
	}

	void ParticleManager::DestroyParticle(Particle* target)
	{
		particles_.erase(target);

		auto iter = particles_.find(target);
		if (iter != particles_.end())
		{
			delete(*iter);
			particles_.erase(iter);
		}
	}

	void ParticleManager::ResetParticles()
	{
		for (auto iter = particles_.begin(); iter != particles_.end(); ++iter)
		{
			(*iter)->Reset();
		}
	}

	void ParticleManager::UpdateParticles()
	{
		for (auto iter = particles_.begin(); iter != particles_.end(); ++iter)
		{
			(*iter)->Update(static_cast<float>(timer_->get_total_time()), static_cast<float>(timer_->get_delta_time()));
		}
	}

	void ParticleManager::DrawParticles(const Camera* camera)
	{
		for (auto iter = particles_.begin(); iter != particles_.end(); ++iter)
		{
			(*iter)->Draw(dc_, camera);
		}
	}
}