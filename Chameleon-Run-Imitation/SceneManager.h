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
		bool Init(ID3D11Device* device, ID3D11DeviceContext* dc);
		void Destroy();
		
		template<class Type>
		void CreateScene(const int key);

		void DestroyScene(const int key);

		bool LoadScene(const int key, Renderer* renderer, Scene** out_scene);

		void UnloadScene(const int key);

	private:
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;

		std::map<int, Scene*> scenes_;
	};

	template<class Type>
	inline void SceneManager::CreateScene(const int key)
	{
		auto iter = scenes_.find(key);
		if (iter == scenes_.end())
		{
			Scene* scene = new Type;
			scenes_.insert({ key, scene });
		}
	}
}
