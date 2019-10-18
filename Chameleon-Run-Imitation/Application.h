#pragma once

/*
PCH: Yes
*/

namespace ba
{
	/*
	[pure virtual functions]
		void Render()
		void UpdateDirectX()

	[virtual functions]
		virtual ~Application()
		void UpdateOnKeyInput()
		bool OnResize()			- call base function
		bool InitDirectX()		- call base function
		void ReleaseDirectX()	- call base function(end)
		void OnMouseMove(WPARAM w_par, int x, int y)
		void OnMouseLBtnDown(WPARAM w_par, int x, int y)
		void OnMouseMBtnDown(WPARAM w_par, int x, int y)
		void OnMouseRBtnDown(WPARAM w_par, int x, int y)
		void OnMouseLBtnUp(WPARAM w_par, int x, int y)
		void OnMouseMBtnUp(WPARAM w_par, int x, int y)
		void OnMouseRBtnUp(WPARAM w_par, int x, int y)
	*/
	class Application : private Uncopiable
	{
	public:
		Application();
		virtual ~Application();

		LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w_par, LPARAM l_par);

		static Application* kApp;

	private:
		// Render scene.
		virtual void Render() = 0;

		// Update functions.
		//
		void Update();

		void ShowFrameState();
		virtual void UpdateOnKeyInput() {};

		virtual void UpdateDirectX() = 0;
		//__

	public:
		// Run main loop.
		void Run();

		// Integrated initialization and releasement of application.
		//
		bool Init(HINSTANCE wnd_inst);
		void Release();
		//__

	private:
		// Initialize the window.
		bool InitMainWindow(HINSTANCE wnd_inst);

	protected:
		// Initialize DirectX properties.
		virtual bool InitDirectX();

		// Release DirectX properties.
		virtual void ReleaseDirectX();

		// Called when the window is resized.
		virtual bool OnResize();

	private:
		// Called by InitDirectX().
		//
		bool CreateDeviceAndImmediateContext();
		bool CreateSwapChain();
		bool CreateRenderTargetView();
		bool CreateDepthStencilView();
		void InitViewport();
		//__

	protected:
		// Compute aspect ratio and return it.
		float aspect_ratio() const;
		DXGI_SAMPLE_DESC GetSampleDesc() const;

		// Window properties.
		//
		static const std::wstring kClassName;
		static const std::wstring kWndName;

		HINSTANCE wnd_inst_;
		HWND main_wnd_;

		static const int kInitClientWidth;
		static const int kInitClientHeight;

		int client_width_;
		int client_height_;

		bool client_minimized_;
		bool client_maximized_;
		bool client_resizing_;
		//__

		// Properties related to the game timer.
		//
		Timer timer_;
		bool paused_;
		float prev_time_;
		int frame_count_;
		//__

		// DirectX properties.
		//
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;

		IDXGISwapChain* swap_chain_;
		ID3D11RenderTargetView* rtv_;
		ID3D11DepthStencilView* dsv_;
		ID3D11Texture2D* depth_stencil_buffer_;
		D3D11_VIEWPORT viewport_;

		bool b_4x_msaa_;
		UINT msaa_quality_level_;
		DXGI_SAMPLE_DESC sample_desc_;
		//__

	private:
		// Called on mouse input.
		//
		virtual void OnMouseMove(WPARAM w_par, int x, int y) {};
		virtual void OnMouseLBtnDown(WPARAM w_par, int x, int y) {};
		virtual void OnMouseMBtnDown(WPARAM w_par, int x, int y) {};
		virtual void OnMouseRBtnDown(WPARAM w_par, int x, int y) {};
		virtual void OnMouseLBtnUp(WPARAM w_par, int x, int y) {};
		virtual void OnMouseMBtnUp(WPARAM w_par, int x, int y) {};
		virtual void OnMouseRBtnUp(WPARAM w_par, int x, int y) {};
		//__

	protected:
		// Cache keyboard inputs.
		//
		bool key_pressed_[256];
		bool key_switch_[256];
		//__
	};
}
