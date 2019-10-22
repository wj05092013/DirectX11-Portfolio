#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class GraphicComponent : private Uncopiable
	{
	protected:
		// Only the class 'GraphicComponentManager' can call this constructor.
		GraphicComponent();
		friend class ba::GraphicComponentManager;

	public:
		virtual ~GraphicComponent() = 0;

		virtual void Destroy() = 0;
	};
}
