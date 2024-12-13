#pragma once
#include "bdsp_ProcessorChain.h"

namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType>
		ProcessorChain<SampleType>::ProcessorChain()
		{
			outGain.initGain(1);
		}
		template<typename SampleType>
		void ProcessorChain<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
		{
			for (auto p : processors)
			{
				p->prepare(spec);
			}
			outGain.prepare(spec);
		}
		template<typename SampleType>
		void ProcessorChain<SampleType>::reset()
		{
			for (auto p : processors)
			{
				p->reset();
			}
			outGain.reset();
		}


		template<typename SampleType>
		void ProcessorChain<SampleType>::updateSmoothedVariables()
		{
			for (auto p : processors)
			{
				p->updateSmoothedVariables();
			}
			outGain.updateSmoothedVariables();
		}


		template<typename SampleType>
		SampleType ProcessorChain<SampleType>::processSample(int channel, const SampleType& inputSample) noexcept
		{
			SampleType out = inputSample;
			for (int i = 0; i < processors.size(); ++i)
			{
				out = processors.getUnchecked(callingOrder.getUnchecked(i))->processSample(channel, out); // the current processor processes the output of the last processor
			}
			out = outGain.processSample(channel, inputSample); // apply output gain
			return out;
		}

		template<typename SampleType>
		StereoSample<SampleType> ProcessorChain<SampleType>::processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
		{
			StereoSample<SampleType> out = inputSample;
			for (int i = 0; i < processors.size(); ++i)
			{
				out = processors.getUnchecked(callingOrder.getUnchecked(i))->processSampleStereo(out); // the current processor processes the output of the last processor
			}
			out = outGain.processSampleStereo(out); // apply output gain
			return out;
		}
		template<typename SampleType>
		void ProcessorChain<SampleType>::setSmoothingTime(SampleType timeInSeconds)
		{
			for (auto p : processors)
			{
				p->setSmoothingTime(timeInSeconds);
			}
		}

		template<typename SampleType>
		ProcessorChain<SampleType>* ProcessorChain<SampleType>::addProcessor(BaseProcessingUnit<SampleType>* proc)
		{
			processors.add(proc);
			callingOrder.add(callingOrder.size());
			return this;
		}

		template<typename SampleType>
		ProcessorChain<SampleType>* ProcessorChain<SampleType>::addProcessors(juce::Array<BaseProcessingUnit<SampleType>*>& procs)
		{
			for (int i = 0; i < procs.size(); ++i)
			{
				addProcessor(procs[i]);
			}
			return this;
		}


		template<typename SampleType>
		void ProcessorChain<SampleType>::removeProcessor(BaseProcessingUnit<SampleType>* proc)
		{
			int idx = processors.indexOf(proc); 
			processors.remove(idx);

			for (int i = 0; i < callingOrder.size(); ++i)
			{
				if (callingOrder[i] > idx) // this proc now has a new index because a proc before it was removed
				{
					callingOrder.set(i, callingOrder[i] - 1);
				}
			}
		}
		template<typename SampleType>
		void ProcessorChain<SampleType>::reorderProcessors(const juce::Array<int>& newOrder)
		{
			callingOrder = newOrder;
		}
		template<typename SampleType>
		void ProcessorChain<SampleType>::moveProcessor(BaseProcessingUnit<SampleType>* proccesorToMove, int newIdx)
		{
			callingOrder.move(processors.indexOf(proccesorToMove), newIdx);
		}
		template<typename SampleType>
		void ProcessorChain<SampleType>::clear()
		{
			processors.clear();
			callingOrder.clear();
		}

		template<typename SampleType>
		int ProcessorChain<SampleType>::getNumProcessors()
		{
			return processors.size();
		}

		template<typename SampleType>
		void ProcessorChain<SampleType>::replaceProcessor(BaseProcessingUnit<SampleType>* processorToReplace, BaseProcessingUnit<SampleType>* processorToInsert)
		{
			int idx = processors.indexOf(processorToReplace);
			if (idx > -1) // only replace a processer that is in this chain
			{
				processors.set(idx, processorToInsert);
			}
		}


		template<typename SampleType>
		void ProcessorChain<SampleType>::replaceProcessor(int processorToReplace, BaseProcessingUnit<SampleType>* processorToInsert)
		{
			processors.set(callingOrder[processorToReplace], processorToInsert);
		}

		template<typename SampleType>
		BaseProcessingUnit<SampleType>* ProcessorChain<SampleType>::getProcessor(int index)
		{
			return processors.getUnchecked(callingOrder.getUnchecked(index));
		}

		template<typename SampleType>
		void ProcessorChain<SampleType>::setOutGain(SampleType newGain)
		{
			outGain.setGain(newGain);
		}

		template<typename SampleType>
		void ProcessorChain<SampleType>::initOutGain(SampleType newGain)
		{
			outGain.initGain(newGain);
		}



		template class ProcessorChain<float>;
		template class ProcessorChain<double>;

	} // namespace dsp
} // namespace bdsp