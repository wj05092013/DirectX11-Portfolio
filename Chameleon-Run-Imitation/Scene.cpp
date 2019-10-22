#include "stdafx.h"

ba::Scene::Scene() :
	device_(nullptr),
	dc_(nullptr),
	renderer_(nullptr),
	timer_(nullptr),
	client_width_(0),
	client_height_(0)
{
}

ba::Scene::~Scene()
{
}

bool ba::Scene::Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer, int client_width, int client_height)
{
	device_ = device;
	dc_ = dc;
	renderer_ = renderer;
	timer_ = timer;
	client_width_ = client_width;
	client_height_ = client_height;

	return true;
}

void ba::Scene::Destroy()
{
	// Do nothing.
}
