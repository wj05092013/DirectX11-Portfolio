#pragma once

/*
PCH: No
*/

namespace ba
{
	class Scene01 : public Scene
	{
	public:
		enum State
		{
			kPause = 0,
			kRun,
			kFinish
		};

	public:
		Scene01();
		~Scene01() override;

		bool Init(
			ID3D11Device* device, ID3D11DeviceContext* dc,
			Renderer* renderer, Timer* timer,
			int client_width, int client_height
		) override;
		void Destroy() override;

		void Render() override;
		void Update() override;
		bool OnResize(int client_width, int client_height) override;

		void UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256]) override;
		void OnMouseMove(HWND wnd, WPARAM w_par, int x, int y) override;
		void OnMouseLBtnDown(HWND wnd, WPARAM w_par, int x, int y) override;
		void OnMouseRBtnDown(HWND wnd, WPARAM w_par, int x, int y) override;
		void OnMouseLBtnUp(HWND wnd, WPARAM w_par, int x, int y) override;
		void OnMouseRBtnUp(HWND wnd, WPARAM w_par, int x, int y) override;

	private:
		// For memory deallocation on failing initialization of members.
		bool InitImpl();

		bool LoadCharacter();
		bool LoadModels();
		void DestroyModels();

	private:
		static const float kCamFovY, kCamNearZ, kCamFarZ;
		static const XMFLOAT3 kCamInitPos, kCamInitTarget, kCamInitUp;
		static const FreeViewpointCamera::FreeViewpointCameraDesc kCamInitDesc;

		static const UINT kShadowMapSize;

		static const float kFogStart, kFogRange;
		static const XMVECTOR kFogColor;
		
		static const XMMATRIX kToTex;

		State scene_state_;

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

		ShadowMap::BoundingSphere bounding_sphere_;

		POINT last_mouse_pos_;

		Character* character_;
		ModelData* sphere_;
		ModelData* red_box_;
		ModelData* ylw_box_;
		std::vector<Model> model_inst_;
	};
}
