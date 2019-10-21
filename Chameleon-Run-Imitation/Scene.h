#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		bool Init();

		void Render();
		void Update();
		void OnResize();

		void set_camera(Camera* camera, const XMVECTOR& init_pos, const XMVECTOR& init_target, const XMVECTOR& init_up);
		
	private:
		Camera* camera_;
		XMVECTOR init_cam_pos_;
		XMVECTOR init_cam_target_;
		XMVECTOR init_cam_up_;

		
	};
}