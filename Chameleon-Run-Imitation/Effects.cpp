#include "stdafx.h"

ba::BasicEffect ba::effects::kBasicEffect;
ba::NormalMappingEffect ba::effects::kNormalMappingEffect;
ba::ShadowMapEffect ba::effects::kShadowMapEffect;
ba::NormalDepthMapEffect ba::effects::kNormalDepthMapEffect;
ba::SSAOMapEffect ba::effects::kSSAOMapEffect;
ba::BlurSSAOEffect ba::effects::kBlurSSAOEffect;
ba::DebugScreenEffect ba::effects::kDebugScreenEffect;
ba::RenderTextureEffect ba::effects::kRenderTextureEffect;

bool ba::effects::InitAll(ID3D11Device* device)
{
	if (!kBasicEffect.Init(device, kBasicEffectFileName)) return false;
	if (!kNormalMappingEffect.Init(device, kNormalMappingEffectFileName)) { ReleaseAll(); return false; }
	if (!kShadowMapEffect.Init(device, kShadowMapEffectFileName)) { ReleaseAll(); return false; }
	if (!kNormalDepthMapEffect.Init(device, kNormalDepthMapEffectFileName)) { ReleaseAll(); return false; }
	if (!kSSAOMapEffect.Init(device, kSSAOMapEffectFileName)) { ReleaseAll(); return false; }
	if (!kBlurSSAOEffect.Init(device, kBlurSSAOMapEffectFileName)) { ReleaseAll(); return false; }
	if (!kDebugScreenEffect.Init(device, kDebugScreenEffectFileName)) { ReleaseAll(); return false; }
	if (!kRenderTextureEffect.Init(device, kRenderTextureEffectFileName)) { ReleaseAll(); return false; }

	return true;
}

void ba::effects::ReleaseAll()
{
	kBasicEffect.Release();
	kNormalMappingEffect.Release();
	kShadowMapEffect.Release();
	kNormalDepthMapEffect.Release();
	kSSAOMapEffect.Release();
	kBlurSSAOEffect.Release();
	kDebugScreenEffect.Release();
	kRenderTextureEffect.Release();
}
