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

		bool Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer) override;
		void Destroy() override;

		void Render(IDXGISwapChain* swap_chain) override;
		void Update() override;
		void OnResize(int width, int height, float near_z, float far_z, float fov_y) override;

		void UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256]) override;
		void OnMouseMove(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnUp(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnUp(WPARAM w_par, int x, int y) override;

	private:
		static const UINT kShadowMapSize;
		static const Camera::CameraDesc kCamBaseInitDesc;
		static const FreeViewpointCamera::FreeViewpointCameraDesc kCamInitDesc;
		static const light::DirectionalLight kLightInit;

		// Graphic components.
		ShadowMap* shadow_map_;
		SSAOMap* ssao_map_;
		FreeViewpointCamera* camera_;

		Renderer::EffectVariableBundleChangeRarely		evb_change_rarely_;
		Renderer::EffectVariableBundleOnStartAndResize	evb_on_start_and_resize_;
		Renderer::EffectVariableBundlePerFrame			evb_per_frame_;

		light::DirectionalLight lights_[3];

		std::vector<ModelInstance> model_inst_;

		ShadowMap::BoundingSphere bounding_sphere_;

		POINT last_mouse_pos_;
	};
}
