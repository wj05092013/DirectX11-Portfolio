#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class TextureManager : public Uncopiable
	{
	private:
		TextureManager();

	public:
		static TextureManager& GetInstance();

		void Init(ID3D11Device* device);
		void Release();

		bool CreateSRV(const std::wstring& file_name, ID3D11ShaderResourceView** out_srv);

	private:
		ID3D11Device* device_;
		std::map<std::wstring, ID3D11ShaderResourceView*> srvs_;
	};
}