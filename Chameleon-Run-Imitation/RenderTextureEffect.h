#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class RenderTextureEffect final : public EffectWrapper
	{
	public:
		RenderTextureEffect();
		~RenderTextureEffect() override;

		bool Init(ID3D11Device* device, const std::wstring& file_name) override;

		ID3DX11EffectTechnique* tech();

		// Set Effect Variables
		void SetTexMapping(const XMMATRIX& matrix);
		void SetInputImage(ID3D11ShaderResourceView* srv);

	private:
		ID3DX11EffectTechnique* tech_;

		ID3DX11EffectMatrixVariable* tex_mapping_;
		ID3DX11EffectShaderResourceVariable* input_image_;
	};
}
