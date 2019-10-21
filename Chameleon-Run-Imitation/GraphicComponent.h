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
		virtual ~GraphicComponent();

		virtual void Release() = 0;
	};
}
