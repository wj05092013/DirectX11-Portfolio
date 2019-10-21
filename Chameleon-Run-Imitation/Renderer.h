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
		struct SceneRenderingComponents
		{
			ID3D11RenderTargetView* rtv;
			ID3D11DepthStencilView* dsv;
			D3D11_VIEWPORT* viewport;
			Camera* cam;
			ShadowMap* shadow_map;
			SSAOMap* ssao_map;
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
			light::DirectionalLight* directional_lights;
			float						fog_start;
			float						fog_range;
			XMVECTOR					fog_color;
			float						shadow_map_size;
			XMMATRIX					to_tex;
		};

		void RenderScene(const std::vector<ModelInstance>& model_instances, const EffectVariableBundlePerFrame& bundle);
		void RenderShadowMap(const std::vector<ModelInstance>& model_instances, const EffectVariableBundlePerFrame& bundle);
		void RenderNormalDepthMap(const std::vector<ModelInstance>& model_instances, const EffectVariableBundlePerFrame& bundle);

		void SetEffectVariablesOnStartAndResize(const EffectVariableBundleOnStartAndResize& bundle);

		void SetEffectVariablesChangeRarely(const EffectVariableBundleChangeRarely& bundle);

		void set_rendering_components(const SceneRenderingComponents& rendering_component);
		
	private:
		SceneRenderingComponents rendering_components_;
	};
}
