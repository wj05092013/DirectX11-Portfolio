#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class ParticleEffect final : public EffectWrapper
	{
	public:
		enum ETechType
		{
			kStreamOut = 0,
			kDrawing,
			kMax
		};

	public:
		ParticleEffect();
		~ParticleEffect() override;

		bool Init(ID3D11Device* device, const std::wstring& file_name) override;
		void Destroy() override;

		ID3DX11EffectTechnique* tech(ETechType type);

		//
		// Set Effect Variables
		//

		// Per frame.
		void SetEyePos(const XMFLOAT3& pos);
		void SetView(const XMMATRIX& matrix);
		void SetEmitPos(const XMFLOAT3& pos);
		void SetGameTime(float time);
		void SetDeltaTime(float time);

		// Per resize.
		void SetProj(const XMMATRIX& matrix);

		// Change rarely.
		void SetTexArrSize(int size);
		void SetTexArr(ID3D11ShaderResourceView* srv);
		void SetRandTex(ID3D11ShaderResourceView* srv);

	private:
		ID3DX11EffectTechnique** techs_;

		ID3DX11EffectVectorVariable* eye_pos_;
		ID3DX11EffectMatrixVariable* view_;
		ID3DX11EffectVectorVariable* emit_pos_;
		ID3DX11EffectScalarVariable* game_time_;
		ID3DX11EffectScalarVariable* delta_time_;

		ID3DX11EffectMatrixVariable* proj_;

		ID3DX11EffectScalarVariable* tex_arr_size_;
		ID3DX11EffectShaderResourceVariable* tex_arr_;
		ID3DX11EffectShaderResourceVariable* rand_tex_;
	};
}
