#pragma once

/*
PCH: Yes
*/

namespace ba
{
	/*
	[pure virtual functions]
		void Render()
		void Update()
		void OnResize()

	[virtual functions]
		bool Init()		- call base function
		void Destroy()	- call base function(end)

		~Application()
		void UpdateOnKeyInput()
		void OnMouseMove()
		void OnMouseLBtnDown()
		void OnMouseMBtnDown()
		void OnMouseRBtnDown()
		void OnMouseLBtnUp()
		void OnMouseMBtnUp()
		void OnMouseRBtnUp()
	*/
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

		virtual bool Init(
			ID3D11Device* device, ID3D11DeviceContext* dc,
			Renderer* renderer, Timer* timer,
			int client_width, int client_height
		);
		virtual void Destroy();

		virtual void Render(IDXGISwapChain* swap_chain) = 0;
		virtual void Update() = 0;
		virtual bool OnResize(int client_width, int client_height) = 0;

		virtual void UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256]) {};

		virtual void OnMouseMove(WPARAM w_par, int x, int y) {};
		virtual void OnMouseLBtnDown(WPARAM w_par, int x, int y) {};
		virtual void OnMouseRBtnDown(WPARAM w_par, int x, int y) {};
		virtual void OnMouseLBtnUp(WPARAM w_par, int x, int y) {};
		virtual void OnMouseRBtnUp(WPARAM w_par, int x, int y) {};

	protected:
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;
		Renderer* renderer_;
		Timer* timer_;
		int client_width_;
		int client_height_;
	};
}
