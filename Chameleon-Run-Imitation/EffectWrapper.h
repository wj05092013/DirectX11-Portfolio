#pragma once

/*
PCH: Yes
*/

namespace ba
{
	/*
	[pure virtual functions]
		virtual ~Application()

	[virtual functions]
		bool Init()		- call base function
		void Release()	- call base function(end)
	*/
	class EffectWrapper : private Uncopiable
	{
	public:
		EffectWrapper();
		virtual ~EffectWrapper() = 0;

		virtual bool Init(ID3D11Device* device, const std::wstring& file_name);
		virtual void Release();

	protected:
		ID3DX11Effect* effect();

	private:
		bool CreateEffectFromFX(ID3D11Device* device);
		bool CreateEffectFromFXO(ID3D11Device* device);

	private:
		std::wstring file_name_;
		ID3DX11Effect* effect_;
	};
}
