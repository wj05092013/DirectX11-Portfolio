#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class BasicEffect final : public EffectWrapper
	{
	public:
		enum TechType
		{
			kLight1 = 0,
			kLight2,
			kLight3,
			kLight0Tex,
			kLight1Tex,
			kLight2Tex,
			kLight3Tex,
			kLight0TexAlphaClip,
			kLight1TexAlphaClip,
			kLight2TexAlphaClip,
			kLight3TexAlphaClip,
			kLight1Fog,
			kLight2Fog,
			kLight3Fog,
			kLight0TexFog,
			kLight1TexFog,
			kLight2TexFog,
			kLight3TexFog,
			kLight0TexAlphaClipFog,
			kLight1TexAlphaClipFog,
			kLight2TexAlphaClipFog,
			kLight3TexAlphaClipFog,

			kLight1Reflect,
			kLight2Reflect,
			kLight3Reflect,
			kLight0TexReflect,
			kLight1TexReflect,
			kLight2TexReflect,
			kLight3TexReflect,
			kLight0TexAlphaClipReflect,
			kLight1TexAlphaClipReflect,
			kLight2TexAlphaClipReflect,
			kLight3TexAlphaClipReflect,
			kLight1FogReflect,
			kLight2FogReflect,
			kLight3FogReflect,
			kLight0TexFogReflect,
			kLight1TexFogReflect,
			kLight2TexFogReflect,
			kLight3TexFogReflect,
			kLight0TexAlphaClipFogReflect,
			kLight1TexAlphaClipFogReflect,
			kLight2TexAlphaClipFogReflect,
			kLight3TexAlphaClipFogReflect,

			kLight1Skinned,
			kLight2Skinned,
			kLight3Skinned,
			kLight0TexSkinned,
			kLight1TexSkinned,
			kLight2TexSkinned,
			kLight3TexSkinned,
			kLight0TexAlphaClipSkinned,
			kLight1TexAlphaClipSkinned,
			kLight2TexAlphaClipSkinned,
			kLight3TexAlphaClipSkinned,
			kLight1FogSkinned,
			kLight2FogSkinned,
			kLight3FogSkinned,
			kLight0TexFogSkinned,
			kLight1TexFogSkinned,
			kLight2TexFogSkinned,
			kLight3TexFogSkinned,
			kLight0TexAlphaClipFogSkinned,
			kLight1TexAlphaClipFogSkinned,
			kLight2TexAlphaClipFogSkinned,
			kLight3TexAlphaClipFogSkinned,

			kLight1ReflectSkinned,
			kLight2ReflectSkinned,
			kLight3ReflectSkinned,
			kLight0TexReflectSkinned,
			kLight1TexReflectSkinned,
			kLight2TexReflectSkinned,
			kLight3TexReflectSkinned,
			kLight0TexAlphaClipReflectSkinned,
			kLight1TexAlphaClipReflectSkinned,
			kLight2TexAlphaClipReflectSkinned,
			kLight3TexAlphaClipReflectSkinned,
			kLight1FogReflectSkinned,
			kLight2FogReflectSkinned,
			kLight3FogReflectSkinned,
			kLight0TexFogReflectSkinned,
			kLight1TexFogReflectSkinned,
			kLight2TexFogReflectSkinned,
			kLight3TexFogReflectSkinned,
			kLight0TexAlphaClipFogReflectSkinned,
			kLight1TexAlphaClipFogReflectSkinned,
			kLight2TexAlphaClipFogReflectSkinned,
			kLight3TexAlphaClipFogReflectSkinned,
			kMax
		};

	public:
		BasicEffect();
		~BasicEffect() override;

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
		void SetMaterial(const light::Material& material);
		void SetDiffuseMap(ID3D11ShaderResourceView* srv);

		//  For skinned object.
		//   Matrix array (max size: 96)
		void SetBoneTransforms(const XMFLOAT4X4* matrices, UINT count);
		//__

		// Per frame.
		//
		void SetEyePos(const XMFLOAT3& pos_w);
		void SetView(const XMMATRIX& matrix);
		void SetShadowTransform(const XMMATRIX& matrix);
		void SetShadowMap(ID3D11ShaderResourceView* srv);
		void SetSSAOMap(ID3D11ShaderResourceView* srv);
		//__

		// Per resize.
		//
		void SetProj(const XMMATRIX& matrix);
		//__

		// Change rarely.
		//
		//  'DirectionalLight' array (compulsary size: 3)
		void SetDirectionalLights(const light::DirectionalLight* lits);
		void SetFogStart(float distance);
		void SetFogRange(float range);
		void SetFogColor(const XMVECTOR& color);
		void SetShadowMapSize(float size);
		void SetToTex(const XMMATRIX& matrix);
		void SetCubeMap(ID3D11ShaderResourceView* srv);
		//__

	private:
		ID3DX11EffectTechnique** techs_;

		ID3DX11EffectMatrixVariable* world_;
		ID3DX11EffectMatrixVariable* world_inv_trans_;
		ID3DX11EffectMatrixVariable* tex_mapping_;
		ID3DX11EffectVariable* material_;
		ID3DX11EffectShaderResourceVariable* diffuse_map_;
		ID3DX11EffectMatrixVariable* bone_transforms_;
		
		ID3DX11EffectVectorVariable* eye_pos_;
		ID3DX11EffectMatrixVariable* view_;
		ID3DX11EffectMatrixVariable* shadow_transform_;
		ID3DX11EffectShaderResourceVariable* shadow_map_;
		ID3DX11EffectShaderResourceVariable* ssao_map_;
		
		ID3DX11EffectMatrixVariable* proj_;

		ID3DX11EffectVariable* directional_lights_;
		ID3DX11EffectScalarVariable* fog_start_;
		ID3DX11EffectScalarVariable* fog_range_;
		ID3DX11EffectVectorVariable* fog_color_;
		ID3DX11EffectScalarVariable* shadow_map_size_;
		ID3DX11EffectMatrixVariable* to_tex_;
		ID3DX11EffectShaderResourceVariable* cube_map_;
	};
}