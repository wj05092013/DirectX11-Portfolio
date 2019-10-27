#pragma once

/*
PCH: Yes
*/

#include "FreeViewpointCamera.h"
#include "RotationalCamera.h"

namespace ba
{
	namespace color
	{
		const XMVECTOR kWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
		const XMVECTOR kBlack = { 0.0f, 0.0f, 0.0f, 1.0f };
		const XMVECTOR kRed = { 1.0f, 0.0f, 0.0f, 1.0f };
		const XMVECTOR kGreen = { 0.0f, 1.0f, 0.0f, 1.0f };
		const XMVECTOR kBlue = { 0.0f, 0.0f, 1.0f, 1.0f };
		const XMVECTOR kYellow = { 1.0f, 1.0f, 0.0f, 1.0f };
		const XMVECTOR kCyan = { 0.0f, 1.0f, 1.0f, 1.0f };
		const XMVECTOR kMagenta = { 1.0f, 0.0f, 1.0f, 1.0f };

		const XMVECTOR kSilver = { 0.75f, 0.75f, 0.75f, 1.0f };
		const XMVECTOR kLightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
	}

	namespace collision
	{
		const float kDomainStartZ = 0.0f;
		const float kDomainSizeZ = 10.0f;
	}

	namespace scene01
	{
		//
		// Models
		//

		// Model data of spheres.
		static const float kSphereRadius = 0.5f;
		static const int kSphereSubdivisionCount = 3;
		static const XMMATRIX kSphereLocalTransform = XMMatrixTranslation(0.5f, 0.5f, 0.5f);

		// Model data of boxes.
		static const XMFLOAT3 kBoxSize = XMFLOAT3(1.0f, 1.0f, 1.0f);
		static const XMMATRIX kBoxLocalTransform = XMMatrixTranslation(0.5f, 0.5f, 0.5f);
		static const float kBoxRestitutions[6] = { 0.8f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f };

		// Materials.
		static const light::Material kRedMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),				// ambient
			XMFLOAT4(1.0f, 0.0f, 0.501960f, 1.0f),			// diffuse
			XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f),				// specular
			XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)				// reflection
		};
		static const light::Material kYellowMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
			XMFLOAT4(1.0f, 0.925490f, 0.015686f, 1.0f),
			XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f),
			XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)
		};
		static const light::Material kBlackMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
			XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
			XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f),
			XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)
		};

		// Main character.
		static const XMVECTOR kCharacterInitScale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
		static const XMVECTOR kCharacterInitRotation = XMQuaternionIdentity();
		static const XMVECTOR kCharacterInitTranslation = XMVectorSet(0.0f, 5.0f, 0.0f, 0.0f);
		static const float kCharacterInitMass = 1.0f;
		static const bool kCharacterInitGravityEnable = true;
		static const float kCharacterInitRestitution = 0.8f;

		// Spheres.

		// Boxes.
		//
		static const XMVECTOR kBox01InitScale = XMVectorSet(1.0f, 1.0f, 6.0f, 0.0f);
		static const XMVECTOR kBox01InitRotation = XMQuaternionIdentity();
		static const XMVECTOR kBox01InitTranslation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		
		static const XMVECTOR kBox02InitScale = XMVectorSet(1.0f, 1.0f, 3.0f, 0.0f);
		static const XMVECTOR kBox02InitRotation = XMQuaternionIdentity();
		static const XMVECTOR kBox02InitTranslation = XMVectorSet(0.0f, 1.0f, 9.0f, 0.0f);

		static const XMVECTOR kBox03InitScale = XMVectorSet(1.0f, 1.0f, 3.0f, 0.0f);
		static const XMVECTOR kBox03InitRotation = XMQuaternionIdentity();
		static const XMVECTOR kBox03InitTranslation = XMVectorSet(0.0f, 1.0f, 15.0f, 0.0f);

		static const XMVECTOR kBox04InitScale = XMVectorSet(1.0f, 1.0f, 2.0f, 0.0f);
		static const XMVECTOR kBox04InitRotation = XMQuaternionIdentity();
		static const XMVECTOR kBox04InitTranslation = XMVectorSet(0.0f, 0.0f, 20.0f, 0.0f);
		//__

		//
		// Scene Settings for Rendering
		//

		static const UINT kShadowMapSize = 2048U;

		static const float kFogStart = 5.0f;
		static const float kFogRange = 20.0f;
		static const XMVECTOR kFogColor = color::kSilver;

		static const XMMATRIX kToTex = XMMATRIX(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f
		);

		//
		// Cameras Settings
		//

		static const float kCamFovY = 0.25f * XM_PI;
		static const float kCamNearZ = 0.1f;
		static const float kCamFarZ = 1000.0f;

		// Free-view camera.
		static const XMVECTOR kFVCamInitPos = XMVectorSet(10.0f, 10.0f, -10.0f, 1.0f);
		static const XMVECTOR kFVCamInitTarget = XMVectorSet(0.0f, 2.0f, 5.0f, 1.0f);
		static const XMVECTOR kFVCamInitUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		static const FreeViewpointCamera::FreeViewpointCameraDesc kFVCamInitDesc = { 20.0f, 20.0f, 20.0f, 0.1f };

		// Rotational camera.
		static const XMVECTOR kRTCamInitPos = XMVectorSet(10.0f, 10.0f, -10.0f, 1.0f);
		static const XMVECTOR kRTCamInitTarget = kCharacterInitTranslation;
		static const XMVECTOR kRTCamInitUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		static const RotationalCamera::RotationalCameraDesc kRTCamInitDesc = {
			kCharacterInitTranslation, 0.1f, 5.0f, 20.0f, 20.0f
		};
	}
}