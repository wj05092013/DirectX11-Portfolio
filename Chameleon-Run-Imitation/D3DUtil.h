#pragma once

/*
PCH: Yes
*/

namespace ba
{
	bool CreateTexRTVAndSRV(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format, ID3D11RenderTargetView** out_rtv, ID3D11ShaderResourceView** out_srv);
}