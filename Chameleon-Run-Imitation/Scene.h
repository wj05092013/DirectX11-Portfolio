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
	protected:
		// Only the class 'SceneManager' can call this constructor.
		Scene();
		virtual bool Init(
			ID3D11Device* device, ID3D11DeviceContext* dc,
			Renderer* renderer, Timer* timer,
			int client_width, int client_height
		);
		virtual void Destroy();
		friend class ba::SceneManager;

	public:
		virtual ~Scene();

		virtual void Render() = 0;
		virtual void Update() = 0;
		virtual bool OnResize(int client_width, int client_height) = 0;

		virtual void UpdateOnKeyInput(bool key_pressed[256], bool key_down[256], bool key_up[256]) {};

		virtual void OnMouseMove(HWND wnd, WPARAM w_par, int x, int y) {};
		virtual void OnMouseLBtnDown(HWND wnd, WPARAM w_par, int x, int y) {};
		virtual void OnMouseRBtnDown(HWND wnd, WPARAM w_par, int x, int y) {};
		virtual void OnMouseLBtnUp(HWND wnd, WPARAM w_par, int x, int y) {};
		virtual void OnMouseRBtnUp(HWND wnd, WPARAM w_par, int x, int y) {};

	protected:
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;
		Renderer* renderer_;
		Timer* timer_;
		int client_width_;
		int client_height_;
	};
}
