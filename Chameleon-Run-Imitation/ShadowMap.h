#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class ShadowMap
	{
	public:
		struct BoundingSphere
		{
			XMFLOAT3 center;
			float radius;
		};

	public:
		ShadowMap();
		~ShadowMap();

		bool Init(ID3D11Device* device, UINT width, UINT height);
		void Release();

		void BuildShadowTransform(const ba::light::DirectionalLight& lit, const BoundingSphere& scene_bounds);

		XMMATRIX view() const;
		XMMATRIX proj() const;
		XMMATRIX world_to_tex() const;

		const D3D11_VIEWPORT& viewport();
		ID3D11ShaderResourceView* srv();
		ID3D11DepthStencilView* dsv();

	private:
		bool BuildShadowMapViews();
		void BuildShadowMapViewport();

	private:
		ID3D11Device* device_;

		UINT width_;
		UINT height_;

		XMFLOAT4X4 view_;
		XMFLOAT4X4 proj_;
		XMFLOAT4X4 world_to_tex_;

		D3D11_VIEWPORT viewport_;
		ID3D11ShaderResourceView* srv_;
		ID3D11DepthStencilView* dsv_;
	};

}
