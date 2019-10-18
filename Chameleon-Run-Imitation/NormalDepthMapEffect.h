#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class NormalDepthMapEffect final : public EffectWrapper
	{
	public:
		enum TechType
		{
			kNormalDepthMap = 0,
			kNormalDepthMapAlphaClip,
			kMax
		};

	public:
		NormalDepthMapEffect();
		~NormalDepthMapEffect() override;

		bool Init(ID3D11Device* device, const std::wstring& file_name) override;
		void Release() override;

		ID3DX11EffectTechnique* tech(TechType type);


		//
		// Set Effect Variables
		//

		// Per objects.
		//
		void SetWorld(const XMMATRIX& matrix);
		void SetWorldInvTrans(const XMMATRIX& matrix);
		void SetTexMapping(const XMMATRIX& matrix);
		void SetDiffuseMap(ID3D11ShaderResourceView* srv);
		//__

		// Per frame.
		void SetView(const XMMATRIX& matrix);

		// Per resize.
		void SetProj(const XMMATRIX& matrix);

	private:
		ID3DX11EffectTechnique** techs_;

		ID3DX11EffectMatrixVariable* world_;
		ID3DX11EffectMatrixVariable* world_inv_trans_;
		ID3DX11EffectMatrixVariable* tex_mapping_;
		ID3DX11EffectShaderResourceVariable* diffuse_map_;

		ID3DX11EffectMatrixVariable* view_;

		ID3DX11EffectMatrixVariable* proj_;
	};
}