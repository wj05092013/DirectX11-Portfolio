#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Renderer : public Uncopiable
	{
		//
		// Initial Setting
		//

	public:
		Renderer();
		~Renderer();

		bool Init(ID3D11Device* device, ID3D11DeviceContext* dc);
		void Destroy();
		
	private:
		ID3D11DeviceContext* dc_;


		//
		// Related to Scene Rendering
		//

	public:
		struct ScreenDesc
		{
			ID3D11RenderTargetView* rtv;
			ID3D11DepthStencilView* dsv;
			D3D11_VIEWPORT* viewport;
		};

		struct EffectVariableBundlePerFrame
		{
			// Empty.
		};

		struct EffectVariableBundleOnStartAndResize
		{
			// Empty.
		};

		struct EffectVariableBundleChangeRarely
		{
			light::DirectionalLight*	directional_lights;
			float						fog_start;
			float						fog_range;
			XMVECTOR					fog_color;
			float						shadow_map_size;
			XMMATRIX					to_tex;
		};

		void RenderScene(const std::vector<Model*>& model_instances, const EffectVariableBundlePerFrame& bundle);
		void RenderShadowMap(const std::vector<Model*>& model_instances, const EffectVariableBundlePerFrame& bundle);
		void RenderNormalDepthMap(const std::vector<Model*>& model_instances, const EffectVariableBundlePerFrame& bundle);

		void SetEffectVariablesOnStartAndResize(const EffectVariableBundleOnStartAndResize& bundle);

		void SetEffectVariablesChangeRarely(const EffectVariableBundleChangeRarely& bundle);

		void set_screen_desc(const ScreenDesc& screen_desc);
		void set_camera(Camera* camera);
		void set_shadow_map(ShadowMap* shadow_map);
		void set_ssao_map(SSAOMap* ssao_map);
		
	private:
		ScreenDesc screen_desc_;

		Camera* camera_;
		ShadowMap* shadow_map_;
		SSAOMap* ssao_map_;
	};
}
