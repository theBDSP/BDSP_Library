#pragma once


namespace bdsp
{

	namespace dsp
	{
		/**
		 * Extends the juce SmoothedValue class to include basic arithmetic operations on the current value.
		 */
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


		/**
		 * A specialized matrix where each element is a smoothed value.
		 * Row-major ordered (row,col)
		 */
		template <typename T>
		class SmoothedMatrix : public Matrix<T>
		{
		public:

			/**
			 * Creates an empty matrix.
			 * If a constructor that has arguments for dimensions or the copy constructor is called, this will also create the proper number of smoother objects.
			 * If not, you will need to call setSize when you know the dimensions of your matrix
			 */
			template <class ... Types>
			SmoothedMatrix(Types...args)
				:Matrix<T>(args...)
			{
				setSize(juce::dsp::Matrix<T>::getNumColumns(), juce::dsp::Matrix<T>::getNumRows());
			}

			~SmoothedMatrix()
			{

			}

			/* Updates each element of the matrix to its next smoothed value */
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

			/* Sets the target value for each element of the matrix */
			void set(Matrix<T>& newValues)
			{
				if (newValues.getSize() != juce::dsp::Matrix<T>::getSize()) // if the given matrix has different dimensions than this
				{
					setSize(newValues.getNumRows(), newValues.getNumColumns());
				}

				for (size_t r = 0; r < juce::dsp::Matrix<T>::getNumRows(); ++r)
				{
					for (size_t c = 0; c < juce::dsp::Matrix<T>::getNumColumns(); ++c)
					{
						set(r, c, newValues(r, c));
					}
				}
			}

			/* Sets the target value of a single element */
			void set(int r, int c, T value)
			{
				smoothers[getArrayIndex(r, c)]->setTargetValue(value);
			}

			/* Sets the value of a single element without smoothing */
			void init(int r, int c, T value)
			{
				smoothers[getArrayIndex(r, c)]->setCurrentAndTargetValue(value);
			}

			void setSize(int r, int c)
			{
				smoothers.clear();
				for (int i = 0; i < r * c; ++i)
				{
					smoothers.add(new OperableSmoothedValue<T>());
				}
			}

		private:

			int getArrayIndex(int r, int c)
			{
				return r * juce::dsp::Matrix<T>::getNumColumns() + c;
			}

			juce::OwnedArray<OperableSmoothedValue<T>> smoothers;

			JUCE_LEAK_DETECTOR(SmoothedMatrix)
		};




	}// namespace dsp
}// namespace bdsp

