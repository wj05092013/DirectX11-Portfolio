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

ba::SceneManager& ba::SceneManager::GetInstance()
{
	static SceneManager instance;
	return instance;
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

bool ba::SceneManager::LoadNextScene(Scene** out_scene, int client_width, int client_height)
{
	auto iter = scenes_.find(next_scene_idx);

	// There's no more scene.
	if (iter == scenes_.end())
		return false;

	// Initialization failed.
	if (!(iter->second->Init(device_, dc_, renderer_, timer_, client_width, client_height)))
		return false;

	UnloadCurrentScene();
	++next_scene_idx;

	*out_scene = iter->second;

	return true;
}

void ba::SceneManager::UnloadCurrentScene()
{
	auto iter = scenes_.find(next_scene_idx - 1);
	if (iter != scenes_.end())
	{
		iter->second->Destroy();
	}
}
