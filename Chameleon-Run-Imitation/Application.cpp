#include "stdafx.h"

using namespace ba;

Application* Application::kApp = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w_par, LPARAM l_par)
{
	return Application::kApp->WndProc(hwnd, msg, w_par, l_par);
}


const std::wstring Application::kClassName = L"D3D11";
const std::wstring Application::kWndName = L"D3D11";

const int Application::kInitClientWidth = 1080;
const int Application::kInitClientHeight = 720;


ba::Application::Application() :
	wnd_inst_(nullptr), main_wnd_(nullptr),
	client_width_(kInitClientWidth), client_height_(kInitClientHeight),
	client_minimized_(false), client_maximized_(false), client_resizing_(false),
	timer_(), paused_(false), prev_time_(0.0f), frame_count_(0),

	device_(nullptr), dc_(nullptr),
	swap_chain_(nullptr), rtv_(nullptr), dsv_(nullptr),
	depth_stencil_buffer_(nullptr), viewport_{},
	b_4x_msaa_(true), msaa_quality_level_(0), sample_desc_{},
	key_pressed_{}, key_down_{}, key_up_{}, prev_key_down_(0), prev_key_up_(0)
{
	kApp = this;
}

ba::Application::~Application()
{
}

LRESULT ba::Application::WndProc(HWND hwnd, UINT msg, WPARAM w_par, LPARAM l_par)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		OnMouseMove(w_par, GET_X_LPARAM(l_par), GET_Y_LPARAM(l_par));
		break;

	case WM_LBUTTONDOWN:
		OnMouseLBtnDown(w_par, GET_X_LPARAM(l_par), GET_Y_LPARAM(l_par));
		break;
	case WM_MBUTTONDOWN:
		OnMouseMBtnDown(w_par, GET_X_LPARAM(l_par), GET_Y_LPARAM(l_par));
		break;
	case WM_RBUTTONDOWN:
		OnMouseRBtnDown(w_par, GET_X_LPARAM(l_par), GET_Y_LPARAM(l_par));
		break;

	case WM_LBUTTONUP:
		OnMouseLBtnUp(w_par, GET_X_LPARAM(l_par), GET_Y_LPARAM(l_par));
		break;
	case WM_MBUTTONUP:
		OnMouseMBtnUp(w_par, GET_X_LPARAM(l_par), GET_Y_LPARAM(l_par));
		break;
	case WM_RBUTTONUP:
		OnMouseRBtnUp(w_par, GET_X_LPARAM(l_par), GET_Y_LPARAM(l_par));
		break;

	case WM_KEYDOWN:
		key_pressed_[w_par] = true;
		key_down_[w_par] = true;
		prev_key_down_ = w_par;
		break;
	case WM_KEYUP:
		key_pressed_[w_par] = false;
		key_up_[w_par] = true;
		prev_key_up_ = w_par;
		break;

	case WM_ACTIVATE:
		if (LOWORD(w_par) == WA_INACTIVE)
		{
			paused_ = true;
			timer_.Pause();
		}
		else
		{
			paused_ = false;
			timer_.Unpause();
		}
		break;

	case WM_ENTERSIZEMOVE:
		client_resizing_ = true;
		paused_ = true;
		timer_.Pause();
		break;

	case WM_EXITSIZEMOVE:
		client_resizing_ = false;
		paused_ = false;
		timer_.Unpause();
		break;

	case WM_SIZE:
		client_width_ = LOWORD(l_par);
		client_height_ = HIWORD(l_par);

		if (device_)
		{
			if (w_par == SIZE_MAXIMIZED)
			{
				client_maximized_ = true;
				client_minimized_ = false;
				paused_ = false;
				timer_.Unpause();
				if (!OnResize()) { Destroy(); DestroyWindow(main_wnd_); }
			}
			else if (w_par == SIZE_MINIMIZED)
			{
				client_maximized_ = false;
				client_minimized_ = true;
				paused_ = true;
				timer_.Unpause();
			}
			else if (w_par == SIZE_RESTORED)
			{
				if (client_maximized_)
				{
					client_maximized_ = false;
					if (!OnResize()) { Destroy(); DestroyWindow(main_wnd_); }
				}
				else if (client_minimized_)
				{
					client_minimized_ = false;
					paused_ = false;
					timer_.Unpause();
					if (!OnResize()) { Destroy(); DestroyWindow(main_wnd_); }
				}
				else if (client_resizing_)
				{

				}
				else
				{
					// API call such as SetWindowPos or SetFullscreenState.
					if (!OnResize()) { Destroy(); DestroyWindow(main_wnd_); }
				}
			}
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, w_par, l_par);
	}
	return 0;
}

void ba::Application::Update()
{
	ShowFrameState();
	UpdateOnKeyInput();
	UpdateDirectX();

	// Reinitialize the key down and key up states.
	key_down_[prev_key_down_] = false;
	key_up_[prev_key_up_] = false;
	prev_key_down_ = 0;
	prev_key_up_ = 0;
}

void ba::Application::ShowFrameState()
{
	float curr_time = (float)timer_.get_total_time();

	++frame_count_;

	if (curr_time - prev_time_ >= 1.0f)
	{
		int fps = frame_count_;
		float mspf = 1000.0f / fps;

		std::wostringstream out_str;
		out_str.precision(6);
		out_str << kWndName << " - [ "
			<< L"FPS: " << fps << L", "
			<< L"Frame Time: " << mspf << L" (ms) ]";

		SetWindowText(main_wnd_, out_str.str().c_str());

		frame_count_ = 0;
		prev_time_ = curr_time;
	}
}

bool ba::Application::Init(HINSTANCE wnd_inst)
{
	if (!InitMainWindow(wnd_inst))
		return false;
	if (!InitDirectX())
	{
		Destroy();
		return false;
	}
	return true;
}

void ba::Application::Run()
{
	timer_.Reset();

	MSG msg{};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer_.Tick();

			if (paused_)
			{
				Sleep(100);
			}
			else
			{
				Update();
				Render();
			}
		}
	}
}

void ba::Application::Destroy()
{
	DestroyDirectX();
}

bool ba::Application::InitMainWindow(HINSTANCE wnd_inst)
{
	wnd_inst_ = wnd_inst;

	WNDCLASSEXW wnd_class;

	wnd_class.cbSize = sizeof(WNDCLASSEX);
	wnd_class.style = CS_HREDRAW | CS_VREDRAW;
	wnd_class.lpfnWndProc = ::WndProc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = wnd_inst_;
	wnd_class.hIcon = LoadIcon(wnd_inst_, MAKEINTRESOURCE(IDI_CHAMELEONIMITATION));
	wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wnd_class.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wnd_class.lpszMenuName = nullptr;
	wnd_class.lpszClassName = kClassName.c_str();
	wnd_class.hIconSm = LoadIcon(wnd_inst_, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wnd_class);

	RECT rect = { 0, 0, client_width_, client_height_ };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	main_wnd_ = CreateWindowW(
		kClassName.c_str(), kWndName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height,
		nullptr, nullptr, wnd_inst_, nullptr
	);
	if (!main_wnd_)
	{
		MessageBox(nullptr, L"fail: CreateWindowW", nullptr, 0);
		return false;
	}

	ShowWindow(main_wnd_, SW_SHOWDEFAULT);
	UpdateWindow(main_wnd_);

	return true;
}

bool ba::Application::InitDirectX()
{
	if (!CreateDeviceAndImmediateContext()) return false;
	if (!CreateSwapChain()) return false;
	if (!CreateRenderTargetView()) return false;
	if (!CreateDepthStencilView()) return false;

	InitViewport();

	return true;
}

void ba::Application::DestroyDirectX()
{
	DestroyCOM(device_);
	DestroyCOM(dc_);
	DestroyCOM(swap_chain_);
	DestroyCOM(rtv_);
	DestroyCOM(dsv_);
	DestroyCOM(depth_stencil_buffer_);
}

bool ba::Application::OnResize()
{
	if (device_ == nullptr || dc_ == nullptr || swap_chain_ == nullptr)
		return false;

	DestroyCOM(rtv_);
	DestroyCOM(dsv_);
	DestroyCOM(depth_stencil_buffer_);


	HRESULT res = swap_chain_->ResizeBuffers(1, client_width_, client_height_, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: ResizeBuffers", nullptr, 0);
		return false;
	}

	ID3D11Texture2D* back_buffer = nullptr;
	res = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& back_buffer);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: GetBuffer", nullptr, 0);
		return false;
	}

	res = device_->CreateRenderTargetView(back_buffer, nullptr, &rtv_);
	DestroyCOM(back_buffer);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: CreateRenderTargetView", nullptr, 0);
		return false;
	}

	if (!CreateDepthStencilView())
	{
		MessageBox(nullptr, L"fail: CreateDepthStencilView", nullptr, 0);
		return false;
	}

	InitViewport();

	return true;
}

bool ba::Application::CreateDeviceAndImmediateContext()
{
	UINT create_flag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	create_flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_level;

	HRESULT res = D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		create_flag, nullptr, 0, D3D11_SDK_VERSION,
		&device_, &feature_level, &dc_
	);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: D3D11CreateDevice", nullptr, 0);
		return false;
	}
	if (feature_level != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(nullptr, L"fail: feature_level != D3D_FEATURE_LEVEL_11_0", nullptr, 0);
		return false;
	}

	res = device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaa_quality_level_);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: CheckMultisampleQualityLevels", nullptr, 0);
		return false;
	}
	if (msaa_quality_level_ <= 0)
	{
		MessageBox(nullptr, L"fail: msaa_quality_level_ <= 0", nullptr, 0);
		return false;
	}

	if (b_4x_msaa_)
	{
		sample_desc_.Count = 4;
		sample_desc_.Quality = msaa_quality_level_ - 1;
	}
	else
	{
		sample_desc_.Count = 1;
		sample_desc_.Quality = 0;
	}

	return true;
}

bool ba::Application::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;

	swap_chain_desc.BufferDesc.Width = client_width_;
	swap_chain_desc.BufferDesc.Height = client_height_;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60U;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1U;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SampleDesc = sample_desc_;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = main_wnd_;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;


	IDXGIDevice* dxgi_device = nullptr;
	IDXGIAdapter* dxgi_adapter = nullptr;
	IDXGIFactory* dxgi_factory = nullptr;

	HRESULT res = device_->QueryInterface(__uuidof(IDXGIDevice), (void**)& dxgi_device);

	if (SUCCEEDED(res))
	{
		res = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)& dxgi_adapter);

		if (SUCCEEDED(res))
		{
			res = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgi_factory);

			if (SUCCEEDED(res))
				res = dxgi_factory->CreateSwapChain(device_, &swap_chain_desc, &swap_chain_);
		}
	}
	DestroyCOM(dxgi_factory);
	DestroyCOM(dxgi_adapter);
	DestroyCOM(dxgi_device);

	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: CreateSwapChain", nullptr, 0);
		return false;
	}

	return true;
}

bool ba::Application::CreateRenderTargetView()
{
	ID3D11Texture2D* back_buffer;

	HRESULT res = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& back_buffer);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: GetBuffer", nullptr, 0);
		return false;
	}

	res = device_->CreateRenderTargetView(back_buffer, nullptr, &rtv_);
	DestroyCOM(back_buffer);

	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: CreateRenderTargetView", nullptr, 0);
		return false;
	}

	return true;
}

bool ba::Application::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc;
	depth_stencil_buffer_desc.Width = client_width_;
	depth_stencil_buffer_desc.Height = client_height_;
	depth_stencil_buffer_desc.MipLevels = 1;
	depth_stencil_buffer_desc.ArraySize = 1;
	depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_buffer_desc.SampleDesc = sample_desc_;
	depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_buffer_desc.CPUAccessFlags = 0;
	depth_stencil_buffer_desc.MiscFlags = 0;

	HRESULT res = device_->CreateTexture2D(&depth_stencil_buffer_desc, nullptr, &depth_stencil_buffer_);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: CreateTexture2D", nullptr, 0);
		return false;
	}

	res = device_->CreateDepthStencilView(depth_stencil_buffer_, nullptr, &dsv_);
	if (FAILED(res))
	{
		MessageBox(nullptr, L"fail: CreateDepthStencilView", nullptr, 0);
		return false;
	}

	return true;
}

void ba::Application::InitViewport()
{
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.Width = static_cast<float>(client_width_);
	viewport_.Height = static_cast<float>(client_height_);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

float ba::Application::aspect_ratio() const
{
	return float(client_width_) / float(client_height_);
}

DXGI_SAMPLE_DESC ba::Application::GetSampleDesc() const
{
	return sample_desc_;
}
