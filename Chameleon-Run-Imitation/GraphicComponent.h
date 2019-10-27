#pragma once

/*
PCH: Yes
*/

#include "GraphicComponentManager.h"

namespace ba
{
	class GraphicComponent : private Uncopiable
	{
	protected:
		// Only the class 'GraphicComponentManager' can call this constructor.
		GraphicComponent();
		friend class GraphicComponentManager;

	public:
		virtual ~GraphicComponent() = 0;

		virtual void Destroy() = 0;
	};
}
