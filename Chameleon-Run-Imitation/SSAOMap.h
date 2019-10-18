#pragma once

namespace ba
{
	class SSAOMap : private Uncopiable
	{
	public:
		SSAOMap();
		~SSAOMap();

		bool Init(
			ID3D11Device* device, ID3D11DeviceContext* dc,
			UINT width, UINT height, float fov_y, float far_z
		);
		void Release();

		bool OnResize(UINT width, UINT height, float fov_y, float far_z);

		void BuildSSAOMap(const Camera& cam);
		void BlurSSAOMap(UINT blur_count);

		ID3D11ShaderResourceView* normal_depth_map_srv();
		ID3D11RenderTargetView* normal_depth_map_rtv();
		ID3D11DepthStencilView* dsv();
		ID3D11ShaderResourceView* ssao_map_srv();

	private:
		void BlurSSAOMap(
			ID3D11ShaderResourceView* input_image_srv,
			ID3D11RenderTargetView* output_image_rtv,
			BlurSSAOEffect::TechType tech_type
		);

		bool BuildFullScreenQuad();
		bool BuildNormalDepthMapViews();
		bool BuildDepthStencilView();
		bool BuildSSAOMapViews();
		bool BuildRandomVectorMapView();
		void BuildFarPlaneCornersVectors();
		void BuildOffsetVectors();
		void BuildSSAOViewport();

		void ReleaseFullScreenQuad();
		void ReleaseNormalDepthMapViews();
		void ReleaseDepthStencilView();
		void ReleaseSSAOMapViews();
		void ReleaseRandomVectorMapView();

	private:
		ID3D11Device* device_;
		ID3D11DeviceContext* dc_;

		ID3D11ShaderResourceView* normal_depth_map_srv_;
		ID3D11RenderTargetView* normal_depth_map_rtv_;
		ID3D11DepthStencilView* dsv_;

		ID3D11ShaderResourceView* ssao_map_0_srv_;
		ID3D11ShaderResourceView* ssao_map_1_srv_;
		ID3D11RenderTargetView* ssao_map_0_rtv_;
		ID3D11RenderTargetView* ssao_map_1_rtv_;

		static const UINT kRandomVectorMapSize;
		ID3D11ShaderResourceView* random_vector_map_srv_;

		ID3D11Buffer* quad_vb_;
		ID3D11Buffer* quad_ib_;

		UINT width_;
		UINT height_;
		float fov_y_;
		float far_z_;

		D3D11_VIEWPORT ssao_viewport_;

		XMFLOAT4 far_plane_corners_[4];
		XMFLOAT4 offset_vectors_[14];
	};
}
