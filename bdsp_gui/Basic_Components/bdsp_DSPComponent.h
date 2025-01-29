#pragma once


namespace bdsp
{
	class Processor;
	/** component that does some sort of internal dsp processing for visualization(level meters, env followers, etc.)
	   Must call prepare in derived class for funky virtual class reasons*/
	class DSPComponent
	{
	public:
		DSPComponent(Processor* processor);
		virtual ~DSPComponent();
		virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0; // should flip prepared bool to true in function call


	protected:
		Processor* p;
		bool prepared = false;
	};

} // namespace bdsp
