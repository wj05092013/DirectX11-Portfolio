#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class ShadowMap : public GraphicComponent
	{
	public:
		struct BoundingSphere
		{
			XMFLOAT3 center;
			float radius;
		};

	public:
		ShadowMap();
		~ShadowMap() override;

		bool Init(ID3D11Device* device, UINT width, UINT height);

		// Calling this function for managing resources is unnecessary, if the instance was created by 'GraphicComponentManager'.
		void Destroy() override;

		void BuildShadowTransform();


		//
		// Mutators
		//

		void set_directional_light(const light::DirectionalLight* light);
		void set_bounding_sphere(const BoundingSphere* bounds);


		//
		// Accessors.
		//

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

		const light::DirectionalLight* directional_light_;
		const BoundingSphere* bounding_sphere_;

		XMFLOAT4X4 view_;
		XMFLOAT4X4 proj_;
		XMFLOAT4X4 world_to_tex_;

		D3D11_VIEWPORT viewport_;
		ID3D11ShaderResourceView* srv_;
		ID3D11DepthStencilView* dsv_;
	};

}
