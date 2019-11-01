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

		void Init(ID3D11Device* device, ID3D11DeviceContext* dc_);
		void Destroy();

		bool CreateTex2DSRV(
			const std::wstring& file_name, ID3D11ShaderResourceView** out_srv
			);
		bool CreateTex2DArrSRV(
			const std::vector<std::wstring>& file_names, ID3D11ShaderResourceView** out_srv,
			DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
			UINT filter = D3DX11_FILTER_NONE, UINT mip_filter = D3DX11_FILTER_LINEAR
			);
		bool CreateRandomTex1DSRV(UINT size, ID3D11ShaderResourceView** srv);

	private:
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;
		std::map<std::wstring, ID3D11ShaderResourceView*> srvs_;
	};
}