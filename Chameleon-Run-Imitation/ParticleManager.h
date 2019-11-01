#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class ParticleManager
	{
	private:
		ParticleManager();

	public:
		static ParticleManager& GetInstance();

		~ParticleManager();

		bool Init(ID3D11Device* device, ID3D11DeviceContext* dc, Timer* timer);
		void Destroy();

		bool CreateParticle(
			ParticleEffect* effect, UINT max_particles,
			ID3D11ShaderResourceView* tex_arr_srv, int tex_arr_size,
			ID3D11ShaderResourceView* rand_tex_srv,
			Particle** out_particle
		);

		void ResetParticles();
		void UpdateParticles();
		void DrawParticles(const Camera* camera);

	private:
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;
		Timer* timer_;

		std::vector<Particle*> particles_;
	};
}
