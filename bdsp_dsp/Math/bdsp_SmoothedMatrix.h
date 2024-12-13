#pragma once


namespace bdsp
{

	namespace dsp
	{
		// Extension juce SmoothedValue to allow basic arithmetic operations on the underlying value
		template <typename T>
		class OperableSmoothedValue : public juce::SmoothedValue<T>
		{
		public:
			
			template <class ... Types>
			OperableSmoothedValue(Types...args)
				:juce::SmoothedValue<T>(args...)
			{

			}

			~OperableSmoothedValue()
			{

			}

			inline T operator+ (OperableSmoothedValue<T>& other)
			{
				return juce::SmoothedValue<T>::getCurrentValue() + other.getCurrentValue();
			}

			inline T operator- (OperableSmoothedValue<T>& other)
			{
				return juce::SmoothedValue<T>::getCurrentValue() - other.getCurrentValue();
			}

			inline T operator* (OperableSmoothedValue<T>& other)
			{
				return juce::SmoothedValue<T>::getCurrentValue() * other.getCurrentValue();
			}

			inline T operator/ (OperableSmoothedValue<T>& other)
			{
				return juce::SmoothedValue<T>::getCurrentValue() / other.getCurrentValue();
			}

			inline bool operator== (OperableSmoothedValue<T>& other)
			{
				return juce::SmoothedValue<T>::getCurrentValue() == other.getCurrentValue();
			}

		};


		//extension of juce Matrix class to allow juce Smoothed Values as elements
		//row major ordered (row, col) |||| itereate getNumColumns() then getNumRows()
		template <typename T>
		class SmoothedMatrix : public Matrix<T>
		{
		public:

			template <class ... Types>
			SmoothedMatrix(Types...args)
				:Matrix<T>(args...)
			{
				for (int i = 0; i < juce::dsp::Matrix<T>::getNumColumns()*juce::dsp::Matrix<T>::getNumRows(); ++i)
				{
					smoothers.add(new juce::SmoothedValue<T>());
				}
			}

			~SmoothedMatrix()
			{

			}

			inline void updateSmoothedVariables()
			{
				for (size_t r = 0; r < juce::dsp::Matrix<T>::getNumRows(); ++r)
				{
					for (size_t c = 0; c < juce::dsp::Matrix<T>::getNumColumns(); ++c)
					{
						this->operator()(r, c) = smoothers[getArrayIndex(r, c)]->getNextValue();
					}
				}
			}

			void setSmoothingTime(T sampleRate, T timeInSeconds)
			{
				for (auto s : smoothers)
				{
					s->reset(sampleRate, timeInSeconds);
				}
			}

			void set(Matrix<T>& newValues)
			{
				jassert(newValues.getSize() == juce::dsp::Matrix<T>::getSize());
				for (size_t r = 0; r < juce::dsp::Matrix<T>::getNumRows(); ++r)
				{
					for (size_t c = 0; c < juce::dsp::Matrix<T>::getNumColumns(); ++c)
					{
						//static_cast<OperableSmoothedValue<T>>(operator()(r, c)).setTargetValue(newValues(r,c));
						set(r, c, newValues(r, c));
					}
				}
			}

			void set(int r, int c, T value)
			{
				smoothers[getArrayIndex(r, c)]->setTargetValue(value);
			}

			void init(int r, int c, T value)
			{
				smoothers[getArrayIndex(r, c)]->setCurrentAndTargetValue(value);
			}

		private:

			int getArrayIndex(int r, int c)
			{
				return r * juce::dsp::Matrix<T>::getNumColumns() + c;
			}

			juce::OwnedArray<juce::SmoothedValue<T>> smoothers;

			JUCE_LEAK_DETECTOR(SmoothedMatrix)
		};




	}// namespace dsp
}// namespace bdsp

