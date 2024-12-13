#pragma once
namespace bdsp
{
	namespace dsp
	{
		/**
		 * Processor that processes audio through a set of processors in series. Processors can be added, removed, and reordered dynamically.
		 */
		template <typename SampleType>
		class ProcessorChain : public BaseProcessingUnit<SampleType>
		{
		public:
			ProcessorChain();
			virtual ~ProcessorChain() = default;

			void prepare(const juce::dsp::ProcessSpec& spec) override;

			void reset() override;




			/**
			 * Process a block of audio with each processor in callingOrder
			 */
			void processInternal(bool isBypassed) noexcept override
			{
				juce::dsp::ProcessContextReplacing<SampleType> context(BaseProcessingUnit<SampleType>::internalWetBlock); // create a processing context that uses the internal wet block
				for (int i = 0; i < processors.size(); ++i)
				{
					processors.getUnchecked(callingOrder.getUnchecked(i))->process(context); // process through each processor in callingOrder
				}
				outGain.process(context);

                BaseProcessingUnit<SampleType>::applyDryWetMix();
			}


			/**
			 * Process a single sample of audio with each processor in callingOrder
			 */
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override;


			/**
			 * Process a single stereo sample of audio with each processor in callingOrder
			 */
			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override;




			void updateSmoothedVariables() override;

			void setSmoothingTime(SampleType timeInSeconds) override;


			/**
			 * Adds a new processor to the end of this chain. This object does not own the processor added. 
			 * @param proc The processor to add
			 * @return A pointer to this object so you can chain adds into a single call
			 */
			ProcessorChain* addProcessor(BaseProcessingUnit<SampleType>* proc);


			/**
			 * Adds a set of new processors to the end of this chain. This object does not own the processors added.
			 * @param proc The processors to add
			 * @return A pointer to this object so you can chain adds into a single call
			 */
			ProcessorChain* addProcessors(juce::Array<BaseProcessingUnit<SampleType>*>& procs);


			/**
			 * Removes a processor from this chain 
			 * @param proc The processor to remove
			 */
			virtual void removeProcessor(BaseProcessingUnit<SampleType>* proc);


			/**
			 * Changes the callingOrder in which the processors are called when processing audio
			 * @param newOrder The new calling order
			 */
			void reorderProcessors(const juce::Array<int>& newOrder);

			/**
			 * Moves a single processor in the calling callingOrder
			 * @param proccesorToMove The processor to move
			 * @param newIdx The new index in the calling callingOrder
			 */
			void moveProcessor(BaseProcessingUnit<SampleType>* proccesorToMove, int newIdx);

			/**
			 * Removes all processors 
			 */
			virtual void clear();


			int getNumProcessors();


			/**
			 * Replaces a processor with a different one. This does not change the calling callingOrder, the new processor will take the old one's place in the calling callingOrder.
			 * This does nothing the processor to replace is not currently in this chain.
			 */
			void replaceProcessor(BaseProcessingUnit<SampleType>* processorToReplace, BaseProcessingUnit<SampleType>* processorToInsert);



			/**
			 * Replaces a processor with a different one. This does not change the calling callingOrder, the new processor will take the old one's place in the calling callingOrder.
			 * @param processorToReplace The index in the calling callingOrder of the processor to replace
			 */
			void replaceProcessor(int processorToReplace, BaseProcessingUnit<SampleType>* processorToInsert);

			/**
			 * Returns a pointer to the processor at a certain index in the calling callingOrder. 
			 * @param index The index in the calling callingOrder to get the processor from
			 */
			BaseProcessingUnit<SampleType>* getProcessor(int index);


			/**
			 * Sets a target value for the output gain to be smoothed to 
			 */
			void setOutGain(SampleType newGain);

			/**
			 * Sets a value for the output gain to be set to (no smoothing)
			 */
			void initOutGain(SampleType newGain);

		protected:
			juce::Array<BaseProcessingUnit<SampleType>*> processors; // the processors in the order they were added to the chain
			juce::Array<int> callingOrder; // the order in which the processors get called when processing audio (each element represents an index of the processors array)
			Gain<SampleType> outGain;

		};
	} // namespace dsp
} // namespace bdsp
