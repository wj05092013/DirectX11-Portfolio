#include "stdafx.h"
#include "Game.h"

ba::Game::Game() :
	scene_bounds_{},

	wolf_model_(nullptr),
	
	last_mouse_pos_{}
{
}

ba::Game::~Game()
{
}

void ba::Game::Render()
{
	debug_screen_.set_srv(ssao_map_.normal_depth_map_srv());
	debug_screen_.Render(dc_);

	swap_chain_->Present(0U, 0U);
}

void ba::Game::UpdateDirectX()
{
}

bool ba::Game::OnResize()
{
	if (!Application::OnResize())
		return false;

	return true;
}

bool ba::Game::InitDirectX()
{
	if (!Application::InitDirectX())
		return false;

	if (!GraphicComponentManager::GetInstance().Init())
		return false;
	if (!renderer_.Init(device_, dc_))
		return false;
	if (!inputvertex::InitAll(device_))
		return false;
	
	if (!debug_screen_.Init(device_))
		return false;
	debug_screen_.set_ndc_position_size(0.4f, -0.4f, 0.6f, 0.6f);

	TextureManager::GetInstance().Init(device_);

	if (!InitModels())
		return false;

	InitSceneBounds();
	InitLights();

	// Set screen descriptions participating on all kinds of rendering.
	Renderer::ScreenDesc screen_desc;
	screen_desc.rtv = rtv_;
	screen_desc.dsv = dsv_;
	screen_desc.viewport = &viewport_;
	renderer_.set_screen_desc(screen_desc);
	
	return true;
}

void ba::Game::DestroyDirectX()
{
	renderer_.Destroy();
	ReleaseModels();

	TextureManager::GetInstance().Destroy();

	inputvertex::DestroyAll();

	GraphicComponentManager::GetInstance().Destroy();

	Application::DestroyDirectX();
}

void ba::Game::UpdateOnKeyInput()
{
	// Control the camera.
	//
	if (key_pressed_['W'])
	if (key_pressed_['S'])
	if (key_pressed_['D'])
	if (key_pressed_['A'])
	if (key_pressed_['E'])
	if (key_pressed_['Q'])
	//__
}

void ba::Game::OnMouseMove(WPARAM w_par, int x, int y)
{
	if ((w_par & MK_LBUTTON) != 0)
	{
		
	}

	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::Game::OnMouseLBtnDown(WPARAM w_par, int x, int y)
{
	SetCapture(main_wnd_);
	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::Game::OnMouseRBtnDown(WPARAM w_par, int x, int y)
{
	SetCapture(main_wnd_);
	last_mouse_pos_.x = x;
	last_mouse_pos_.y = y;
}

void ba::Game::OnMouseLBtnUp(WPARAM w_par, int x, int y)
{
	ReleaseCapture();
}

void ba::Game::OnMouseRBtnUp(WPARAM w_par, int x, int y)
{
	ReleaseCapture();
}
