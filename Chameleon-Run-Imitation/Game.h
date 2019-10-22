#pragma once

/*
PCH: No
*/

namespace ba
{
	class Game final : public Application
	{
	public:
		Game();
		~Game() override;

	private:
		void Render() override;
		void UpdateDirectX() override;

		bool OnResize() override;

		bool InitDirectX() override;
		void DestroyDirectX() override;

		void InitSceneBounds();
		void InitLights();

		bool InitModels();
		void ReleaseModels();

	private:
		void UpdateOnKeyInput() override;

		void OnMouseMove(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnUp(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnUp(WPARAM w_par, int x, int y) override;

	private:
		// Renderer.
		Renderer renderer_;
		Renderer::EffectVariableBundleChangeRarely		evb_change_rarely_;
		Renderer::EffectVariableBundleOnStartAndResize	evb_on_start_and_resize_;
		Renderer::EffectVariableBundlePerFrame			evb_per_frame_;

		// Shadow map.
		static const float kShadowMapSize;
		ShadowMap::BoundingSphere	scene_bounds_;
		ShadowMap					shadow_map_;

		// SSAO map.
		SSAOMap ssao_map_;

		// Debug screen.
		DebugScreen debug_screen_;

		// Scene objects.
		//
		Model*						wolf_model_;
		Model*						floor_model_;
		std::vector<ModelInstance>	model_instances_;

		light::DirectionalLight		lights_[3];
		//__

		// Properties related to camera contorl.
		//
		static const XMVECTOR kInitCamPos;
		static const XMVECTOR kInitCamTarget;
		static const XMVECTOR kInitCamUp;

		static const float kFovY;
		static const float kNearZ;
		static const float kFarZ;

		static const float kForwardMovingRate;
		static const float kRightMovingRate;
		static const float kUpperMovingRate;
		static const float kRotationRate;

		Camera	cam_;

		POINT	last_mouse_pos_;
	};
}
