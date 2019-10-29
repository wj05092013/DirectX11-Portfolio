#pragma once

/*
PCH: No
*/

#include "Model.h"
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

	namespace game
	{
		//
		// Rendering Settings
		//

		extern const UINT kShadowMapSize;
		extern const float kSceneBoundsRadius;

		extern const float kFogStart;
		extern const float kFogRange;
		extern const XMVECTOR kFogColor;

		extern const XMMATRIX kToTex;

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

		// Model data of grounds.
		extern const float kGroundSizeX;
		extern const float kGroundSizeZ;
		extern const UINT kGroundSliceCountX;
		extern const UINT kGroundSliceCountZ;
		extern const XMMATRIX kGroundLocalTransform;

		// Materials.
		//
		extern const light::Material kDefaultMaterial;
		extern const light::Material kRedMaterial;
		extern const light::Material kYellowMaterial;
		extern const light::Material kBlackMaterial;

		extern const light::Material kMaterials[4];
		//__
	}

	namespace scene01
	{
		//
		// Models
		//

		// Main character.
		//
		extern const std::string kCharacterName;
		extern const XMFLOAT3 kCharacterInitScale;
		extern const XMFLOAT4 kCharacterInitRotation;
		extern const XMFLOAT3 kCharacterInitTranslation;

		extern const float kCharacterInitMass;
		extern const bool kCharacterInitGravityEnable;
		extern const float kCharacterInitRestitution;

		extern const float kCharacterInitAccelerationZ;
		extern const float kCharacterInitMaxVelocityZ;

		extern const float kJumpEnableTime;
		extern const int kMaxJumpCount;
		extern const XMFLOAT3 kCharacterJumpVelocity;
		//__

		// Spheres.

		// Boxes.
		//
		extern const int kBoxCount;
		extern const std::string kBoxNames[10];
		extern const XMFLOAT3 kBoxInitScales[10];
		extern const XMFLOAT4 kBoxInitRotations[10];
		extern const XMFLOAT3 kBoxInitTranslations[10];
		extern const Model::EColorType kBoxInitColorType[10];
		//__

		// Ground.
		extern const std::string kGroundName;
		extern const XMFLOAT3 kGroundInitScale;
		extern const XMFLOAT4 kGroundInitRotation;
		extern const XMFLOAT3 kGroundInitTranslation;
		extern const Model::EColorType kGroundInitColorType;


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
		extern const float kRTCamInitTargetDiffZ;
		extern const XMFLOAT3 kRTCamInitPos;
		extern const XMFLOAT3 kRTCamInitTarget;
		extern const XMFLOAT3 kRTCamInitUp;
		extern const RotationalCamera::RotationalCameraDesc kRTCamInitDesc;
	}
}