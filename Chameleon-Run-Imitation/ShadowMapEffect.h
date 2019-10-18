#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class ShadowMapEffect final : public EffectWrapper
	{
	public:
		enum TechType
		{
			kBuildShadowMap = 0,
			kBuildShadowMapAlphaClip,
			kBuildShadowMapSkinned,
			kBuildShadowMapAlphaClipSkinned,
			kMax
		};

	public:
		ShadowMapEffect();
		~ShadowMapEffect() override;

		bool Init(ID3D11Device* device, const std::wstring& file_name) override;
		void Release() override;

		ID3DX11EffectTechnique* tech(TechType type);


		//
		// Set Effect Variables
		//

		// Per object.
		//
		void SetWorld(const XMMATRIX& matrix);
		void SetWorldInvTrans(const XMMATRIX& matrix);
		void SetTexMapping(const XMMATRIX& matrix);
		void SetDiffuseMap(ID3D11ShaderResourceView* srv);

		//  For skinned object.
		//   Matrix array (max size: 96)
		void SetBoneTransforms(const XMFLOAT4X4* matrices, UINT count);
		//__

		// Per frame.
		//
		void SetLightView(const XMMATRIX& matrix);
		void SetLightProj(const XMMATRIX& matrix);
		//__

	private:
		ID3DX11EffectTechnique** techs_;

		ID3DX11EffectMatrixVariable* world_;
		ID3DX11EffectMatrixVariable* world_inv_trans_;
		ID3DX11EffectMatrixVariable* tex_mapping_;
		ID3DX11EffectShaderResourceVariable* diffuse_map_;
		ID3DX11EffectMatrixVariable* bone_transforms_;
		
		ID3DX11EffectMatrixVariable* light_view_;
		ID3DX11EffectMatrixVariable* light_proj_;
	};
}