// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#include "resource.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// C system header files.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ standard library header files.
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

// Windows header files.
#include <Windows.h>
#include <windowsx.h>

// DirectX11 header files.
#include <D3D11.h>
#include <D3DX11.h>
#include <DirectXMath.h>

// Other library header files.

// Current project's header files.
#include "UsingNamespace.h"
#include "Uncopiable.h"
#include "Unconstructable.h"
#include "Constants.h"
#include "Macros.h"
#include "MathHelper.h"
#include "Timer.h"
#include "Light.h"

#include "d3dx11effect.h"
#include "EffectWrapper.h"
#include "BasicEffect.h"
#include "NormalMappingEffect.h"
#include "ShadowMapEffect.h"
#include "NormalDepthMapEffect.h"
#include "SSAOMapEffect.h"
#include "BlurSSAOEffect.h"
#include "DebugScreenEffect.h"
#include "RenderTextureEffect.h"
#include "Effects.h"

#include "GraphicComponent.h"
#include "GraphicComponentManager.h"
#include "InputVertex.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Model.h"
#include "DebugScreen.h"
#include "TextureManager.h"
#include "Camera.h"
#include "FreeViewpointCamera.h"
#include "RotationalCamera.h"
#include "RenderStates.h"
#include "ShadowMap.h"
#include "SSAOMap.h"
#include "Renderer.h"

#include "SceneComponent.h"
#include "SceneComponentManager.h"
#include "Scene.h"
#include "SceneManager.h"

#include "Application.h"