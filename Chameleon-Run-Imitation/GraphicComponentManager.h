#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class GraphicComponentManager : private Uncopiable
	{
	private:
		GraphicComponentManager();

	public:
		static GraphicComponentManager& GetInstance();

		bool Init();
		void Release();

		template<class Type>
		Type* CreateComponent(const std::string& key_str);

		void ReleaseComponent(const std::string& key_str);

	private:
		std::map<std::string, GraphicComponent*> components_;
	};

	template<class Type>
	inline Type* GraphicComponentManager::CreateComponent(const std::string& key_str)
	{
		auto iter = components_.find(key_str);

		GraphicComponent* component = nullptr;

		if (iter == components_.end())
		{
			// Create new component for the key.
			component = new Type();
			components_.insert({ key_str, component });
		}
		else
		{
			// There is already a value for the key.
			component = iter->second;
		}
		
		return component;
	}
}
