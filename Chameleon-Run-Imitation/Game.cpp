#include "stdafx.h"
#include "Scene01.h"

ba::Game::Game() :
	current_scene_(nullptr),
	last_mouse_pos_{}
{
}

ba::Game::~Game()
{
}

void ba::Game::Render()
{
	current_scene_->Render();

	swap_chain_->Present(0U, 0U);
}

void ba::Game::UpdateDirectX()
{
	current_scene_->Update();
}

bool ba::Game::OnResize()
{
	if (!Application::OnResize())
		return false;

	if (!current_scene_->OnResize(client_width_, client_height_))
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
	if (!SceneManager::GetInstance().Init(device_, dc_, &renderer_, &timer_))
		return false;
	TextureManager::GetInstance().Init(device_);

	// Set screen descriptions participating on all kinds of rendering.
	Renderer::ScreenDesc screen_desc;
	screen_desc.rtv = rtv_;
	screen_desc.dsv = dsv_;
	screen_desc.viewport = &viewport_;
	renderer_.set_screen_desc(screen_desc);
	
	SceneManager::GetInstance().AddScene<Scene01>();

	if (!SceneManager::GetInstance().LoadNextScene(&current_scene_, client_width_, client_height_))
		return false;

	return true;
}

void ba::Game::DestroyDirectX()
{
	TextureManager::GetInstance().Destroy();
	SceneManager::GetInstance().Destroy();
	inputvertex::DestroyAll();
	renderer_.Destroy();
	GraphicComponentManager::GetInstance().Destroy();

	Application::DestroyDirectX();
}

void ba::Game::UpdateOnKeyInput()
{
	current_scene_->UpdateOnKeyInput(key_pressed_, key_switch_);
}

void ba::Game::OnMouseMove(WPARAM w_par, int x, int y)
{
	current_scene_->OnMouseMove(main_wnd_, w_par, x, y);
}

void ba::Game::OnMouseLBtnDown(WPARAM w_par, int x, int y)
{
	current_scene_->OnMouseLBtnDown(main_wnd_, w_par, x, y);
}

void ba::Game::OnMouseRBtnDown(WPARAM w_par, int x, int y)
{
	current_scene_->OnMouseRBtnDown(main_wnd_, w_par, x, y);
}

void ba::Game::OnMouseLBtnUp(WPARAM w_par, int x, int y)
{
	current_scene_->OnMouseLBtnUp(main_wnd_, w_par, x, y);
}

void ba::Game::OnMouseRBtnUp(WPARAM w_par, int x, int y)
{
	current_scene_->OnMouseRBtnUp(main_wnd_, w_par, x, y);
}
