#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class DebugScreenEffect final : public EffectWrapper
	{
	public:
		enum ETechType
		{
			kViewRGBA = 0,
			kViewRed,
			kViewGreen,
			kViewBlue,
			kViewAlpha,
			kMax
		};

	public:
		DebugScreenEffect();
		~DebugScreenEffect() override;

		bool Init(ID3D11Device* device, const std::wstring& file_name) override;
		void Destroy() override;

		ID3DX11EffectTechnique* tech(ETechType type);


		//
		// Set Effect Variables
		//
		void SetProj(const XMMATRIX& matrix);
		void SetDebugTex(ID3D11ShaderResourceView* srv);

	private:
		ID3DX11EffectTechnique** techs_;

		ID3DX11EffectMatrixVariable* proj_;
		ID3DX11EffectShaderResourceVariable* debug_tex_;
	};
}