#include "bdsp_DSPComponent.h"
namespace bdsp
{

	DSPComponent::DSPComponent(Processor* processor)
	{
		p = processor;
		p->dspComps.add(this);

	}
	DSPComponent::~DSPComponent()
	{
		p->dspComps.removeAllInstancesOf(this);
	}

} // namespace bdsp
