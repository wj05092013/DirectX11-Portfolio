#include "stdafx.h"

ba::SceneManager::SceneManager() :
	device_(nullptr),
	dc_(nullptr)
{
}

bool ba::SceneManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	// Do nothing.
	return false;
}

void ba::SceneManager::Destroy()
{
	for (auto iter = scenes_.begin(); iter != scenes_.end(); ++iter)
	{
		iter->second->Destroy();
	}
}

void ba::SceneManager::DestroyScene(const std::string& key_str)
{
	auto iter = scenes_.find(key_str);

	if (iter == scenes_.end())
		return;

	iter->second->Destroy();
	delete iter->second;

	scenes_.erase(iter);
}

bool ba::SceneManager::LoadScene(const std::string& key_str, Renderer* renderer, Scene** out_scene)
{
	auto iter = scenes_.find(key_str);

	if(iter == scenes_.end())
		return false;

	if (!iter->second->Init(device_, dc_, renderer))
		return false;

	*out_scene = iter->second;
}

void ba::SceneManager::UnloadScene(const std::string& key_str)
{
	auto iter = scenes_.find(key_str);

	if (iter == scenes_.end())
		return;

	iter->second->Destroy();
}
