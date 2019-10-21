#include "stdafx.h"

ba::GraphicComponentManager& ba::GraphicComponentManager::GetInstance()
{
	static GraphicComponentManager instance;
	return instance;
}

bool ba::GraphicComponentManager::Init()
{
	// Do nothing.
	return true;
}

void ba::GraphicComponentManager::Release()
{
	for (auto iter = components_.begin(); iter != components_.end(); ++iter)
	{
		iter->second->Release();
		delete iter->second;
	}

	components_.clear();
}

void ba::GraphicComponentManager::ReleaseComponent(const std::string& key_str)
{
	auto iter = components_.find(key_str);

	if (iter == components_.end())
		return;

	iter->second->Release();
	delete iter->second;

	components_.erase(key_str);
}
