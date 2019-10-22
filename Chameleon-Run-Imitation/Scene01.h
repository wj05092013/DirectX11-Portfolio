#pragma once

/*
PCH: No
*/

namespace ba
{
	class Scene01 : public Scene
	{
	public:
		Scene01();
		~Scene01() override;

		bool Init(
			ID3D11Device* device, ID3D11DeviceContext* dc,
			Renderer* renderer, Timer* timer,
			int client_width, int client_height
		) override;
		void Destroy() override;

		void Render(IDXGISwapChain* swap_chain) override;
		void Update() override;
		bool OnResize(int client_width, int client_height) override;

		void UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256]) override;
		void OnMouseMove(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnUp(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnUp(WPARAM w_par, int x, int y) override;

	private:
		// For memory deallocation on failing initialization of members.
		bool InitImpl();

	private:
		static const float kCamFovY, kCamNearZ, kCamFarZ;
		static const XMFLOAT3 kCamInitPos, kCamInitTarget, kCamInitUp;
		static const FreeViewpointCamera::FreeViewpointCameraDesc kCamInitDesc;

		static const UINT kShadowMapSize;

		static const float kFogStart, kFogRange;
		static const XMVECTOR kFogColor;
		
		static const XMMATRIX kToTex;

		// Graphic components.
		ShadowMap* shadow_map_;
		SSAOMap* ssao_map_;
		FreeViewpointCamera* camera_;

		int client_width_;
		int client_height_;
		float aspect_ratio_;

		Renderer::EffectVariableBundleChangeRarely		evb_change_rarely_;
		Renderer::EffectVariableBundleOnStartAndResize	evb_on_start_and_resize_;
		Renderer::EffectVariableBundlePerFrame			evb_per_frame_;

		light::DirectionalLight lights_[3];

		std::vector<ModelInstance> model_inst_;

		ShadowMap::BoundingSphere bounding_sphere_;

		POINT last_mouse_pos_;
	};
}
