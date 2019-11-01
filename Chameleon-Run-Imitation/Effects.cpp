#include "stdafx.h"

ba::BasicEffect ba::effects::kBasicEffect;
ba::NormalMappingEffect ba::effects::kNormalMappingEffect;
ba::ShadowMapEffect ba::effects::kShadowMapEffect;
ba::NormalDepthMapEffect ba::effects::kNormalDepthMapEffect;
ba::SSAOMapEffect ba::effects::kSSAOMapEffect;
ba::BlurSSAOEffect ba::effects::kBlurSSAOEffect;
ba::DebugScreenEffect ba::effects::kDebugScreenEffect;
ba::RenderTextureEffect ba::effects::kRenderTextureEffect;
ba::ParticleEffect ba::effects::kSmokeParticleEffect;

bool ba::effects::InitAll(ID3D11Device* device)
{
	if (!kBasicEffect.Init(device, kBasicEffectFileName)) return false;
	if (!kNormalMappingEffect.Init(device, kNormalMappingEffectFileName)) { DestroyAll(); return false; }
	if (!kShadowMapEffect.Init(device, kShadowMapEffectFileName)) { DestroyAll(); return false; }
	if (!kNormalDepthMapEffect.Init(device, kNormalDepthMapEffectFileName)) { DestroyAll(); return false; }
	if (!kSSAOMapEffect.Init(device, kSSAOMapEffectFileName)) { DestroyAll(); return false; }
	if (!kBlurSSAOEffect.Init(device, kBlurSSAOMapEffectFileName)) { DestroyAll(); return false; }
	if (!kDebugScreenEffect.Init(device, kDebugScreenEffectFileName)) { DestroyAll(); return false; }
	if (!kRenderTextureEffect.Init(device, kRenderTextureEffectFileName)) { DestroyAll(); return false; }
	if (!kSmokeParticleEffect.Init(device, kSmokeParticleEffectFileName)) { DestroyAll(); return false; }

	return true;
}

void ba::effects::DestroyAll()
{
	kBasicEffect.Destroy();
	kNormalMappingEffect.Destroy();
	kShadowMapEffect.Destroy();
	kNormalDepthMapEffect.Destroy();
	kSSAOMapEffect.Destroy();
	kBlurSSAOEffect.Destroy();
	kDebugScreenEffect.Destroy();
	kRenderTextureEffect.Destroy();
	kSmokeParticleEffect.Destroy();
}
