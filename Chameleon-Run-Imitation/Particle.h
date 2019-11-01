#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Particle
	{
	private:
		// Only the class 'ParticleManager' can call this constructor.
		Particle();
		
		// Only the class 'ParticleManager' can update and draw the particles.
		
		void Update(float game_time, float delta_time);
		void Draw(ID3D11DeviceContext* dc, const Camera* camera);

		friend class ParticleManager;

	public:
		~Particle();

		bool Init(
			ID3D11Device* device, ParticleEffect* effect,
			UINT max_particles,
			ID3D11ShaderResourceView* tex_arr_srv, int tex_arr_size,
			ID3D11ShaderResourceView* rand_tex_srv
		);
		void Destroy();

		void Reset();

		void set_emit_pos_xf(const XMFLOAT3& pos);
		void set_emit_pos_xv(const XMVECTOR& pos);

	private:
		bool BuildVertexBuffers();

	private:
		ID3D11Device* device_;
		ParticleEffect* effect_;

		UINT max_particles_;
		bool first_run_;

		float game_time_;
		float delta_time_;
		float age_;

		XMFLOAT3 emit_pos_;

		ID3D11Buffer* init_vb_;
		ID3D11Buffer* input_vb_;
		ID3D11Buffer* stream_out_vb_;

		ID3D11ShaderResourceView* tex_arr_srv_;
		ID3D11ShaderResourceView* rand_tex_srv_;
		int tex_arr_size_;
	};
}
