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

		// Debug screen.
		DebugScreen debug_screen_;

		POINT	last_mouse_pos_;
	};
}
