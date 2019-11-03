#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class DebugScreen
	{
	public:
		DebugScreen();
		~DebugScreen();

		bool Init(ID3D11Device* device);
		void Destroy();

		void Render(ID3D11DeviceContext* dc);

		void set_ndc_position_size(float left_top_x, float left_top_y, float width, float height);
		void set_srv(ID3D11ShaderResourceView* srv);
		void set_tech_type(DebugScreenEffect::ETechType tech_type);

	private:
		bool BuildGeometryBuffers(ID3D11Device* device);

		ID3D11Buffer* vb_;
		ID3D11Buffer* ib_;

		ID3D11ShaderResourceView* srv_;

		XMFLOAT4X4 proj_;

		DebugScreenEffect::ETechType tech_type_;
	};
}
