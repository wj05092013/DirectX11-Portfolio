#include "stdafx.h"

ba::SceneManager::SceneManager() :
	device_(nullptr),
	dc_(nullptr),
	renderer_(nullptr),
	timer_(nullptr),
	add_scene_idx(0),
	next_scene_idx(0)
{
}

bool ba::SceneManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer)
{
	device_ = device;
	dc_ = dc;
	renderer_ = renderer;
	timer_ = timer;

	add_scene_idx = 0;
	next_scene_idx = 0;

	return true;
}

void ba::SceneManager::Destroy()
{
	for (auto iter = scenes_.begin(); iter != scenes_.end(); ++iter)
	{
		iter->second->Destroy();
		delete iter->second;
	}

	scenes_.clear();
}

bool ba::SceneManager::LoadNextScene(Scene** out_scene)
{
	auto iter = scenes_.find(next_scene_idx);

	// There's no more scene.
	if(iter == scenes_.end())
		return false;

	// Initialization failed.
	if (!iter->second->Init(device_, dc_, renderer_))
		return false;

	UnloadCurrentScene();
	++next_scene_idx;

	*out_scene = iter->second;
}

void ba::SceneManager::UnloadCurrentScene()
{
	auto iter = scenes_.find(next_scene_idx - 1);
	
	iter->second->Destroy();
}
