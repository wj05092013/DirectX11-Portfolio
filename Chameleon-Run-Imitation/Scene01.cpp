#include "stdafx.h"
#include "Scene01.h"

ba::Scene01::Scene01()
{
}

ba::Scene01::~Scene01()
{
}

bool ba::Scene01::Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer)
{


	return true;
}

void ba::Scene01::Destroy()
{
}

void ba::Scene01::Render(IDXGISwapChain* swap_chain)
{
}

void ba::Scene01::Update()
{
}

void ba::Scene01::OnResize(int width, int height, float near_z, float far_z, float fov_y)
{
}

void ba::Scene01::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
{
}

void ba::Scene01::OnMouseMove(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseLBtnDown(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseRBtnDown(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseLBtnUp(WPARAM w_par, int x, int y)
{
}

void ba::Scene01::OnMouseRBtnUp(WPARAM w_par, int x, int y)
{
}
