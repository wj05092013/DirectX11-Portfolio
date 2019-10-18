#pragma once

/*
PCH: Yes
*/

namespace ba
{
	namespace renderstates
	{
		namespace rasterizer
		{
			extern ID3D11RasterizerState* kExample;

			bool Init(ID3D11Device* device);
			void Release();
		}
		
		namespace depthstencil
		{
			bool Init(ID3D11Device* device);
			void Release();
		}

		namespace blend
		{
			enum BlendMode
			{
				kOver,			// default
				kTranslucent,
				kAdditive,
				kModulate,
				kModulate2,
			};

			bool Init(ID3D11Device* device);
			void Release();

			extern ID3D11BlendState* kBSOver;
			extern ID3D11BlendState* kBSTranslucent;
			extern ID3D11BlendState* kBSAdditive;
			extern ID3D11BlendState* kBSModulate;
			extern ID3D11BlendState* kBSModulate2;
		}

		bool InitAll(ID3D11Device* device);
		void ReleaseAll();
	}
}