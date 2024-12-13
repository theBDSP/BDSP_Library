#pragma once


namespace bdsp
{

	namespace dsp
	{

		//extension of juce Matrix class
		//row major ordered (row, col) |||| itereate getNumColumns() then getNumRows()
		template <typename T>
		class Matrix : public juce::dsp::Matrix<T>
		{
		public:

			Matrix()
				:juce::dsp::Matrix<T>(1,1)
			{

			}
			Matrix(size_t numRows, size_t numColumns)
				:juce::dsp::Matrix<T>(juce::dsp::Matrix<T>(numRows, numColumns))
			{
			}

			Matrix(size_t numRows, size_t numColumns, const T* dataPointer)
				:juce::dsp::Matrix<T>(juce::dsp::Matrix<T>(numRows, numColumns, dataPointer))
			{
			}

			Matrix(juce::dsp::Matrix<T>& other)
				:juce::dsp::Matrix<T>(juce::dsp::Matrix<T>(other))
			{
			}


			~Matrix()
			{

			}

			Matrix<T> createHadamard(int stages, T scaling) // creates a 2^stages square matrix
			{
				Matrix<T> out(1, 1);
				out(0, 0) = scaling;

				for (int i = 0; i < stages; ++i)
				{
					out = createHadamardInternal(out);
				}

				
				return out;
			}



			// calculates the sum of every element in the matrix
			T sum()
			{
				T out = 0;
				for (size_t c = 0; c < juce::dsp::Matrix<T>::getNumColumns(); ++c)
				{
					for (size_t r = 0; r < juce::dsp::Matrix<T>::getNumRows(); ++r)
					{
						out += juce::dsp::Matrix<T>::operator()(r, c);
					}
				}
				return out;
			}

			// calculates the mean value of all the elements in the matrix
			T average()
			{
				jassert(!juce::dsp::Matrix<T>::isNullMatrix());
				return sum() / (juce::dsp::Matrix<T>::getNumRows() * juce::dsp::Matrix<T>::getNumColumns());
			}

			T RMS()
			{

				return sqrt(1.0 / (juce::dsp::Matrix<T>::getNumRows() * juce::dsp::Matrix<T>::getNumColumns()) * sumOfSquares());

			}

			T sumOfSquares()
			{
				T out = 0;
				for (size_t c = 0; c < juce::dsp::Matrix<T>::getNumColumns(); ++c)
				{
					for (size_t r = 0; r < juce::dsp::Matrix<T>::getNumRows(); ++r)
					{
						out += juce::dsp::Matrix<T>::operator()(r, c) * juce::dsp::Matrix<T>::operator()(r, c);
					}
				}

				return out;
			}

		private:


			Matrix<T> createHadamardInternal(Matrix<T> in)
			{
				jassert(in.isSquare());
				int n = in.getNumRows();
				Matrix<T> out(2 * n, 2 * n);
				for (int i = 0; i < n; ++i)
				{
					auto v = in(i, i);
					out(i, i) = v;
					out(n + i, i) = v;
					out(i, n + i) = v;
					out(n + i, n + i) = -v;
				}
				return out;
			}

			JUCE_LEAK_DETECTOR(Matrix);
		};
	}// namespace dsp
}// namespace bdsp

