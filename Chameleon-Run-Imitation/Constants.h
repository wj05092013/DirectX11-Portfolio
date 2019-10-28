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
		extern const XMVECTOR kWhite;
		extern const XMVECTOR kBlack;
		extern const XMVECTOR kRed;
		extern const XMVECTOR kGreen;
		extern const XMVECTOR kBlue;
		extern const XMVECTOR kYellow;
		extern const XMVECTOR kCyan;
		extern const XMVECTOR kMagenta;

		extern const XMVECTOR kSilver;
		extern const XMVECTOR kLightSteelBlue;
	}

	namespace collision
	{
		extern const float kDomainStartZ;
		extern const float kDomainSizeZ;
	}

	namespace scene01
	{
		//
		// Models
		//

		// Model data of spheres.
		extern const float kSphereRadius;
		extern const int kSphereSubdivisionCount;
		extern const XMMATRIX kSphereLocalTransform;

		// Model data of boxes.
		extern const XMFLOAT3 kBoxSize;
		extern const XMMATRIX kBoxLocalTransform;
		extern const float kBoxRestitutions[6];

		// Materials.
		extern const light::Material kRedMaterial;
		extern const light::Material kYellowMaterial;
		extern const light::Material kBlackMaterial;

		// Main character.
		//
		extern const XMFLOAT3 kCharacterInitScale;
		extern const XMFLOAT4 kCharacterInitRotation;
		extern const XMFLOAT3 kCharacterInitTranslation;

		extern const float kCharacterInitMass;
		extern const bool kCharacterInitGravityEnable;
		extern const float kCharacterInitRestitution;

		extern const float kCharacterInitAccelerationZ;
		extern const float kCharacterInitMaxVelocityZ;

		extern const XMFLOAT3 kCharacterJumpVelocity;
		//__

		// Spheres.

		// Boxes.
		//
		extern const XMFLOAT3 kBox01InitScale;
		extern const XMFLOAT4 kBox01InitRotation;
		extern const XMFLOAT3 kBox01InitTranslation;
		
		extern const XMFLOAT3 kBox02InitScale;
		extern const XMFLOAT4 kBox02InitRotation;
		extern const XMFLOAT3 kBox02InitTranslation;

		extern const XMFLOAT3 kBox03InitScale;
		extern const XMFLOAT4 kBox03InitRotation;
		extern const XMFLOAT3 kBox03InitTranslation;

		extern const XMFLOAT3 kBox04InitScale;
		extern const XMFLOAT4 kBox04InitRotation;
		extern const XMFLOAT3 kBox04InitTranslation;
		//__

		//
		// Scene Settings for Rendering
		//

		extern const UINT kShadowMapSize;

		extern const float kFogStart;
		extern const float kFogRange;
		extern const XMVECTOR kFogColor;

		extern const XMMATRIX kToTex;

		//
		// Cameras Settings
		//

		extern const float kCamFovY;
		extern const float kCamNearZ;
		extern const float kCamFarZ;

		// Free-view camera.
		extern const XMFLOAT3 kFVCamInitPos;
		extern const XMFLOAT3 kFVCamInitTarget;
		extern const XMFLOAT3 kFVCamInitUp;
		extern const FreeViewpointCamera::FreeViewpointCameraDesc kFVCamInitDesc;

		// Rotational camera.
		extern const XMFLOAT3 kRTCamInitPos;
		extern const XMFLOAT3 kRTCamInitTarget;
		extern const XMFLOAT3 kRTCamInitUp;
		extern const RotationalCamera::RotationalCameraDesc kRTCamInitDesc;
	}
}