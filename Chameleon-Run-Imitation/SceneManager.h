#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class SceneManager
	{
	private:
		SceneManager();
		
	public:
		bool Init(ID3D11Device* device, ID3D11DeviceContext* dc, Renderer* renderer, Timer* timer);
		void Destroy();
		
		// The template parameter 'Type' must be an instance of 'Scene' class.
		template<class Type>
		void AddScene();

		bool LoadNextScene(Scene** out_scene);

	private:
		void UnloadCurrentScene();

		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;
		Renderer* renderer_;
		Timer* timer_;

		std::map<int, Scene*> scenes_;

		int add_scene_idx;
		int next_scene_idx;
	};

	template<class Type>
	inline void SceneManager::AddScene()
	{
		Scene* scene = new Type;
		scenes_.insert({ add_scene_idx, scene });
	}
}
