#include "stdafx.h"

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

	namespace game
	{
		//
		// Rendering Settings
		//

		const UINT kShadowMapSize = 2048U;

		const float kFogStart = 5.0f;
		const float kFogRange = 20.0f;
		const XMVECTOR kFogColor = color::kSilver;

		const XMMATRIX kToTex = XMMATRIX(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f
		);

		//
		// Models
		//

		// Model data of spheres.
		const float kSphereRadius = 0.5f;
		const int kSphereSubdivisionCount = 3;
		const XMMATRIX kSphereLocalTransform = XMMatrixTranslation(0.5f, 0.5f, 0.5f);

		// Model data of boxes.
		const XMFLOAT3 kBoxSize = XMFLOAT3(1.0f, 1.0f, 1.0f);
		const XMMATRIX kBoxLocalTransform = XMMatrixTranslation(0.5f, 0.5f, 0.5f);
		const float kBoxRestitutions[6] = { 0.8f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f };

		// Materials.
		const light::Material kDefaultMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),				// ambient
			XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f),				// diffuse
			XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f),				// specular
			XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)				// reflection
		};
		const light::Material kRedMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
			XMFLOAT4(1.0f, 0.0f, 0.501960f, 1.0f),
			XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f),
			XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)
		};
		const light::Material kYellowMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
			XMFLOAT4(1.0f, 0.925490f, 0.015686f, 1.0f),
			XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f),
			XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)
		};
		const light::Material kBlackMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
			XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
			XMFLOAT4(0.4f, 0.4f, 0.4f, 8.0f),
			XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)
		};
		const light::Material kMaterials[4] = {
			kDefaultMaterial, kRedMaterial, kYellowMaterial, kBlackMaterial
		};

		// Main character.
		//
		const XMFLOAT3 kCharacterInitScale(1.0f, 1.0f, 1.0f);
		const XMFLOAT4 kCharacterInitRotation(0.0f, 0.0f, 0.0f, 1.0f);
		const XMFLOAT3 kCharacterInitTranslation(0.0f, 5.0f, 0.0f);

		const float kCharacterInitMass = 1.0f;
		const bool kCharacterInitGravityEnable = true;
		const float kCharacterInitRestitution = 0.8f;

		const float kCharacterInitAccelerationZ = 3.0f;
		const float kCharacterInitMaxVelocityZ = 6.0f;

		const float kJumpEnableTime = 0.2f;
		const int kMaxJumpCount = 2;
		const XMFLOAT3 kCharacterJumpVelocity(0.0f, 5.0f, 0.0f);
		//__
	}

	namespace scene01
	{
		// Spheres.

		// Boxes.
		//
		const XMFLOAT3 kBox01InitScale(1.0f, 1.0f, 6.0f);
		const XMFLOAT4 kBox01InitRotation(0.0f, 0.0f, 0.0f, 1.0f);
		const XMFLOAT3 kBox01InitTranslation(0.0f, 0.0f, 0.0f);

		const XMFLOAT3 kBox02InitScale(1.0f, 1.0f, 3.0f);
		const XMFLOAT4 kBox02InitRotation(0.0f, 0.0f, 0.0f, 1.0f);
		const XMFLOAT3 kBox02InitTranslation(0.0f, 1.0f, 9.0f);

		const XMFLOAT3 kBox03InitScale(1.0f, 1.0f, 3.0f);
		const XMFLOAT4 kBox03InitRotation(0.0f, 0.0f, 0.0f, 1.0f);
		const XMFLOAT3 kBox03InitTranslation(0.0f, 1.0f, 15.0f);

		const XMFLOAT3 kBox04InitScale(1.0f, 1.0f, 2.0);
		const XMFLOAT4 kBox04InitRotation(0.0f, 0.0f, 0.0f, 1.0f);
		const XMFLOAT3 kBox04InitTranslation(0.0f, 0.0f, 20.0f);
		//__

		//
		// Cameras Settings
		//

		const float kCamFovY = 0.25f * XM_PI;
		const float kCamNearZ = 0.1f;
		const float kCamFarZ = 1000.0f;

		// Free-view camera.
		const XMFLOAT3 kFVCamInitPos(10.0f, 10.0f, -10.0f);
		const XMFLOAT3 kFVCamInitTarget(0.0f, 2.0f, 5.0f);
		const XMFLOAT3 kFVCamInitUp(0.0f, 1.0f, 0.0f);
		const FreeViewpointCamera::FreeViewpointCameraDesc kFVCamInitDesc = { 20.0f, 20.0f, 20.0f, 0.1f };

		// Rotational camera.
		const XMFLOAT3 kRTCamInitPos(10.0f, 10.0f, -20.0f);
		const XMFLOAT3 kRTCamInitTarget = game::kCharacterInitTranslation;
		const XMFLOAT3 kRTCamInitUp(0.0f, 1.0f, 0.0f);
		const RotationalCamera::RotationalCameraDesc kRTCamInitDesc = {
			0.1f, 10.0f, 40.0f, 20.0f
		};
	}
}