#pragma once

/*
PCH: Yes
*/

namespace ba
{
	//
	// All 'EffectWrapper' instances are managed in here.
	//

	namespace effects
	{
		bool InitAll(ID3D11Device* device);
		void ReleaseAll();

		// .fxo file names to create effect interfaces with.
		//
		const std::wstring kBasicEffectFileName = L"FX/Basic.fxo";
		const std::wstring kNormalMappingEffectFileName = L"FX/NormalMapping.fxo";
		const std::wstring kShadowMapEffectFileName = L"FX/ShadowMap.fxo";
		const std::wstring kNormalDepthMapEffectFileName = L"FX/NormalDepthMap.fxo";
		const std::wstring kSSAOMapEffectFileName = L"FX/SSAOMap.fxo";
		const std::wstring kBlurSSAOMapEffectFileName = L"FX/BlurSSAOMap.fxo";
		const std::wstring kDebugScreenEffectFileName = L"FX/DebugScreen.fxo";
		const std::wstring kRenderTextureEffectFileName = L"FX/RenderTexture.fxo";
		//__

		// Instances.
		//
		extern BasicEffect kBasicEffect;
		extern NormalMappingEffect kNormalMappingEffect;
		extern ShadowMapEffect kShadowMapEffect;
		extern NormalDepthMapEffect kNormalDepthMapEffect;
		extern SSAOMapEffect kSSAOMapEffect;
		extern BlurSSAOEffect kBlurSSAOEffect;
		extern DebugScreenEffect kDebugScreenEffect;
		extern RenderTextureEffect kRenderTextureEffect;
		//__
	}
}