#include "stdafx.h"

ba::Scene::Scene() :
	device_(nullptr),
	dc_(nullptr),
	renderer_(nullptr),
	timer_(nullptr)
{
}

ba::Scene::~Scene()
{
}

bool ba::Scene::Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer)
{
	device_ = device;
	dc_ = dc;
	renderer_ = renderer;
	timer_ = timer;

	return true;
}

void ba::Scene::Destroy()
{
	// Do nothing.
}
