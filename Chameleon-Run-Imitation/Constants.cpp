#include "stdafx.h"
#include "Constants.h"

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

		const UINT kShadowMapSize = 4096U;
		const float kSceneBoundsRadius = 300.0f;

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
		const float kSphereRestitution = 0.5f;

		// Model data of boxes.
		const XMFLOAT3 kBoxSize = XMFLOAT3(1.0f, 1.0f, 1.0f);
		const XMMATRIX kBoxLocalTransform = XMMatrixTranslation(0.5f, 0.5f, 0.5f);
		const float kBoxRestitutions[6] = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };

		// Model data of grounds.
		extern const float kGroundSizeX = 1.0f;
		extern const float kGroundSizeZ = 1.0f;
		extern const UINT kGroundSliceCountX = 10;
		extern const UINT kGroundSliceCountZ = 10;
		extern const XMMATRIX kGroundLocalTransform = XMMatrixIdentity();

		// Materials.
		const light::Material kDefaultMaterial = {
			XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),				// ambient
			XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),				// diffuse
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
	}

	namespace scene01
	{
		// Models

		// Main character.
		//
		const std::string kCharacterName = "character";
		const XMFLOAT3 kCharacterInitScale(1.0f, 1.0f, 1.0f);
		const XMFLOAT4 kCharacterInitRotation(0.0f, 0.0f, 0.0f, 1.0f);
		const XMFLOAT3 kCharacterInitTranslation(0.0f, 2.0f, 0.0f);

		const float kCharacterInitMass = 1.0f;
		const bool kCharacterInitGravityEnable = true;
		const float kCharacterInitRestitution = 0.8f;

		const float kCharacterInitAccelerationZ = 5.0f;
		const float kCharacterInitMaxVelocityZ = 6.0f;

		const float kJumpEnableTime = 0.2f;
		const int kMaxJumpCount = 2;
		const XMFLOAT3 kCharacterJumpVelocity(0.0f, 7.0f, 0.0f);
		//__

		// Spheres.
		const int kSphereCount = 2;
		const std::string kSphereNames[2] = {
			"sphere00", "sphere01"
		};
		const XMFLOAT3 kSphereInitScales[2] = {
			{0.8f, 0.8f, 0.8f},
			{1.4f, 1.4f, 1.4f}
		};
		const XMFLOAT4 kSphereInitRotations[2] = {
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
		const XMFLOAT3 kSphereInitTranslations[2] = {
			{0.0f, 4.0f, 11.0f},
			{0.0f, 6.0f, 64.0f}
		};
		const Model::EColorType kSphereInitColorType[2] = {
			Model::kYellow,
			Model::kRed
		};

		// Boxes.
		//
		const int kBoxCount = 10;
		const std::string kBoxNames[10] = {
			"box00", "box01", "box02", "box03", "box04", "box05", "box06", "box07", "box08", "box09"
		};
		const XMFLOAT3 kBoxInitScales[10] = {
			{1.0f, 1.0f, 9.0f},			// 0
			{1.0f, 1.0f, 3.0f},
			{1.0f, 1.0f, 3.0f},
			{1.0f, 1.0f, 2.0},
			{1.0f, 1.0f, 8.0},
			{1.0f, 1.0f, 1.0},			// 5
			{1.0f, 1.0f, 1.0},
			{1.0f, 1.0f, 9.0},
			{1.0f, 1.0f, 5.0},
			{1.0f, 1.0f, 9.0f}
		};
		const XMFLOAT4 kBoxInitRotations[10] = {
			{0.0f, 0.0f, 0.0f, 1.0f},	// 0
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f},	// 5
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
		const XMFLOAT3 kBoxInitTranslations[10] = {
			{0.0f, 0.0f, 0.0f},			// 0
			{0.0f, 2.0f, 10.0f},
			{0.0f, 2.0f, 17.0f},
			{0.0f, 0.0f, 23.0f},
			{0.0f, 1.0f, 30.0f},
			{0.0f, 4.0f, 45.0f},		// 5
			{0.0f, 4.0f, 50.0f},
			{0.0f, 4.0f, 58.0f},
			{0.0f, 1.0f, 71.0f},
			{0.0f, 0.0f, 78.0f}
		};
		const Model::EColorType kBoxInitColorType[10] = {
			Model::kRed,				// 0
			Model::kYellow,
			Model::kBlack,
			Model::kRed,
			Model::kYellow,
			Model::kRed,				// 5
			Model::kYellow,
			Model::kRed,
			Model::kRed,
			Model::kYellow
		};
		//__

		// Ground.
		const std::string kGroundName = "ground";
		const XMFLOAT3 kGroundInitScale = XMFLOAT3(1000.0f, 1.0f, 1000.0f);
		const XMFLOAT4 kGroundInitRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		const XMFLOAT3 kGroundInitTranslation = XMFLOAT3(0.0f, -5.0f, 450.0f);
		const Model::EColorType kGroundInitColorType = Model::kDefault;

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
		const float kRTCamInitTargetDiffZ = 3.0f;
		const XMFLOAT3 kRTCamInitPos(10.0f, 10.0f, -20.0f);
		const XMFLOAT3 kRTCamInitTarget{3.0f, 3.0f, kCharacterInitTranslation.z + kRTCamInitTargetDiffZ };
		const XMFLOAT3 kRTCamInitUp(0.0f, 1.0f, 0.0f);
		const RotationalCamera::RotationalCameraDesc kRTCamInitDesc = {
			0.1f, 10.0f, 40.0f, 20.0f
		};
	}
}