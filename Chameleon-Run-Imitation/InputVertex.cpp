#include "stdafx.h"

//
// PosNormalTex
//

const D3D11_INPUT_ELEMENT_DESC ba::inputvertex::PosNormalTex::kInputElemDesc[3] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
ID3D11InputLayout* ba::inputvertex::PosNormalTex::kInputLayout = nullptr;

bool ba::inputvertex::PosNormalTex::Init(ID3D11Device* device)
{
	if (kInputLayout)
		return false;

	ID3DX11EffectTechnique* tech = effects::kBasicEffect.tech(BasicEffect::ETechType::kLight1);
	D3DX11_PASS_DESC pass_desc;
	tech->GetPassByIndex(0)->GetDesc(&pass_desc);

	HRESULT res = device->CreateInputLayout(
		kInputElemDesc, 3,
		pass_desc.pIAInputSignature, pass_desc.IAInputSignatureSize, &kInputLayout
	);
	if (FAILED(res))
		return false;
	return true;
}

void ba::inputvertex::PosNormalTex::Destroy()
{
	DestroyCOM(kInputLayout);
}

//
// PosNormalTexTangent
//

const D3D11_INPUT_ELEMENT_DESC ba::inputvertex::PosNormalTexTangent::kInputElemDesc[4] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
ID3D11InputLayout* ba::inputvertex::PosNormalTexTangent::kInputLayout = nullptr;

bool ba::inputvertex::PosNormalTexTangent::Init(ID3D11Device* device)
{
	if (kInputLayout)
		return false;

	ID3DX11EffectTechnique* tech = effects::kNormalMappingEffect.tech(NormalMappingEffect::kLight1);
	D3DX11_PASS_DESC pass_desc;
	tech->GetPassByIndex(0)->GetDesc(&pass_desc);

	HRESULT res = device->CreateInputLayout(
		kInputElemDesc, 4,
		pass_desc.pIAInputSignature, pass_desc.IAInputSignatureSize, &kInputLayout
	);
	if (FAILED(res))
		return false;
	return true;
}

void ba::inputvertex::PosNormalTexTangent::Destroy()
{
	DestroyCOM(kInputLayout);
}

//
// PosNormalTexTanSkinned
//

const D3D11_INPUT_ELEMENT_DESC ba::inputvertex::PosNormalTexTanSkinned::kInputElemDesc[6] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
ID3D11InputLayout* ba::inputvertex::PosNormalTexTanSkinned::kInputLayout = nullptr;

bool ba::inputvertex::PosNormalTexTanSkinned::Init(ID3D11Device* device)
{
	if (kInputLayout)
		return false;

	ID3DX11EffectTechnique* tech = effects::kBasicEffect.tech(BasicEffect::ETechType::kLight1Skinned);
	D3DX11_PASS_DESC pass_desc;
	tech->GetPassByIndex(0)->GetDesc(&pass_desc);

	HRESULT res = device->CreateInputLayout(
		kInputElemDesc, 6,
		pass_desc.pIAInputSignature, pass_desc.IAInputSignatureSize, &kInputLayout
	);
	if (FAILED(res))
		return false;
	return true;
}

void ba::inputvertex::PosNormalTexTanSkinned::Destroy()
{
	DestroyCOM(kInputLayout);
}

//
// Particle
//

const D3D11_INPUT_ELEMENT_DESC ba::inputvertex::Particle::kInputElemDesc[5] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
ID3D11InputLayout* ba::inputvertex::Particle::kInputLayout = nullptr;

bool ba::inputvertex::Particle::Init(ID3D11Device* device)
{
	if (kInputLayout)
		return false;

	ID3DX11EffectTechnique* tech = effects::kSmokeParticleEffect.tech(ParticleEffect::kStreamOut);
	D3DX11_PASS_DESC pass_desc;
	tech->GetPassByIndex(0)->GetDesc(&pass_desc);

	HRESULT res = device->CreateInputLayout(
		kInputElemDesc, 5,
		pass_desc.pIAInputSignature, pass_desc.IAInputSignatureSize, &kInputLayout
	);
	if (FAILED(res))
		return false;
	return true;
}

void ba::inputvertex::Particle::Destroy()
{
	DestroyCOM(kInputLayout);
}

//
// Integrated management of all input layouts.
//

ba::inputvertex::PosNormalTex ba::inputvertex::kPosNormalTex;
ba::inputvertex::PosNormalTexTangent ba::inputvertex::kPosNormalTexTangent;
ba::inputvertex::PosNormalTexTanSkinned ba::inputvertex::kPosNormalTexTanSkinned;
ba::inputvertex::Particle ba::inputvertex::kParticle;

bool ba::inputvertex::InitAll(ID3D11Device* device)
{
	if (!kPosNormalTex.Init(device)) return false;
	if (!kPosNormalTexTangent.Init(device)) { DestroyAll(); return false; }
	if (!kPosNormalTexTanSkinned.Init(device)) { DestroyAll(); return false; }
	if (!kParticle.Init(device)) { DestroyAll(); return false; }

	return true;
}

void ba::inputvertex::DestroyAll()
{
	kPosNormalTex.Destroy();
	kPosNormalTexTangent.Destroy();
	kPosNormalTexTanSkinned.Destroy();
	kParticle.Destroy();
}
