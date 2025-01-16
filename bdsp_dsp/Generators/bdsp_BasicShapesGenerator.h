#pragma once 

namespace bdsp
{
	namespace dsp
	{
		/**
		 * Generates Sin, Triangle, or Square waves, with optional skewing.
		 * Can also mix between adjacent shapes (sin-triangle, triangle-square)
		 */
		template <typename SampleType>
		class BasicShapesGenerator : public BasicWaveGenerator<SampleType>
		{
		public:

			BasicShapesGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:BasicWaveGenerator<SampleType>(lookupsToUse)
			{
			}

			/**
			 * 0 = sin
			 * 0.5 = tri
			 * 1 = square
			 */
			void setShape(SampleType newShape)
			{
				shape = newShape;
			}


			void setSkew(SampleType newSkew)
			{
				skew = newSkew;
			}

			SampleType getSample(int channel) override
			{
				return BasicWaveGenerator<SampleType>::lookups->waveLookups->getLFOValue(shape, skew, BasicWaveGenerator<SampleType>::phase);
			}

		private:
			SampleType skew;

			SampleType shape = 0;
		};

		//================================================================================================================================================================================================

		/**
		 * Simple generator that only creates un-skewed sin waves
		 */
		template <typename SampleType>
		class SinGenerator : public BasicWaveGenerator <SampleType>
		{
		public:


			SinGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:BasicWaveGenerator<SampleType>(lookupsToUse)
			{

			}

			SampleType getSample(int channel) override
			{
				return BasicWaveGenerator<SampleType>::lookups->waveLookups->fastLookupSin(0.5, BasicWaveGenerator<SampleType>::phase);
			}
		};

		//================================================================================================================================================================================================
		/**
		 * Simple generator that only creates un-skewed triangle waves
		 */
		template <typename SampleType>
		class TriGenerator : public BasicWaveGenerator <SampleType>
		{
		public:


			TriGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:BasicWaveGenerator<SampleType>(lookupsToUse)
			{

			}

			SampleType getSample(int channel) override
			{
				return BasicWaveGenerator<SampleType>::lookups->waveLookups->fastLookupTri(0.5, BasicWaveGenerator<SampleType>::phase);
			}
		};

		//================================================================================================================================================================================================
		/**
		 * Simple generator that only creates un-skewed square waves
		 */
		template <typename SampleType>
		class SquareGenerator : public BasicWaveGenerator <SampleType>
		{
		public:


			SquareGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:BasicWaveGenerator<SampleType>(lookupsToUse)
			{

			}

			SampleType getSample(int channel) override
			{
				return BasicWaveGenerator<SampleType>::lookups->waveLookups->fastLookupSqr(0.5, BasicWaveGenerator<SampleType>::phase);
			}
		};

		//================================================================================================================================================================================================
		/**
		 * Simple generator that only creates saw waves
		 */
		template <typename SampleType>
		class SawGenerator : public BasicWaveGenerator <SampleType>
		{
		public:


			SawGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:BasicWaveGenerator<SampleType>(lookupsToUse)
			{

			}

			SampleType getSample(int channel) override
			{
				return BasicWaveGenerator<SampleType>::lookups->waveLookups->fastLookupTri(0, BasicWaveGenerator<SampleType>::phase);
			}
		};

	} // namespace dsp
} // namespace bdsp
