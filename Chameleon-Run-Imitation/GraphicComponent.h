#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class GraphicComponent : private Uncopiable
	{
	public:
		GraphicComponent();
		virtual ~GraphicComponent() = 0;

		virtual bool Init() = 0;
		virtual void Release() = 0;
	};
}
