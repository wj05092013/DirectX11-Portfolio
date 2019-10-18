#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class BlurSSAOEffect final : public EffectWrapper
	{
	public:
		enum TechType
		{
			kHorizontalBlur = 0,
			kVerticalBlur,
			kMax
		};

	public:
		BlurSSAOEffect();
		~BlurSSAOEffect() override;

		bool Init(ID3D11Device* device, const std::wstring& file_name) override;
		void Release() override;

		ID3DX11EffectTechnique* tech(TechType type);


		//
		// Set Effect Variables
		//

		// Per resize.
		void SetTexelWidth(float width);
		void SetTexelHeight(float height);

		// Change rarely.
		void SetNormalDepthMap(ID3D11ShaderResourceView* srv);
		void SetInputImage(ID3D11ShaderResourceView* srv);

	private:
		ID3DX11EffectTechnique** techs_;

		ID3DX11EffectScalarVariable* texel_width_;
		ID3DX11EffectScalarVariable* texel_height_;

		ID3DX11EffectShaderResourceVariable* normal_depth_map_;
		ID3DX11EffectShaderResourceVariable* input_image_;
	};
}