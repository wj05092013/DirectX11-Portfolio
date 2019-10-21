#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

		virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer);
		virtual void Release();

		virtual void Render(IDXGISwapChain* swap_chain) = 0;
		virtual void Update() = 0;
		virtual void OnResize(int width, int height, float near_z, float far_z, float fov_y) = 0;

	protected:
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;
		Renderer* renderer_;
	};
}