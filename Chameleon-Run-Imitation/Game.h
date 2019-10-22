#pragma once

/*
PCH: Yes
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

	private:
		void UpdateOnKeyInput() override;

		void OnMouseMove(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnDown(WPARAM w_par, int x, int y) override;
		void OnMouseLBtnUp(WPARAM w_par, int x, int y) override;
		void OnMouseRBtnUp(WPARAM w_par, int x, int y) override;

	private:
		Renderer renderer_;

		Scene* current_scene_;

		POINT	last_mouse_pos_;
	};
}
