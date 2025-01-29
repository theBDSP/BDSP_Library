#pragma once


namespace bdsp
{

	namespace dsp
	{
		/**
		 * Extension of juce Matrix class.
		 * Row major ordered (row, col).
		 */
		template <typename T>
		class Matrix : public juce::dsp::Matrix<T>
		{
		public:

			Matrix()
				:juce::dsp::Matrix<T>(1, 1)
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

			/*creates a 2^stages square Hadamard matrix*/
			Matrix<T> createHadamardMatrix(int stages, T scaling)
			{
				Matrix<T> out(1, 1);
				out(0, 0) = scaling;

				for (int i = 0; i < stages; ++i)
				{
					out = createHadamardMatrixInternal(out);
				}


				return out;
			}



			/* Calculates the sum of every element in the matrix */ 
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

			/* Calculates the mean value of all the elements in the matrix	*/
			T average()
			{
				jassert(!juce::dsp::Matrix<T>::isNullMatrix());
				return sum() / (juce::dsp::Matrix<T>::getNumRows() * juce::dsp::Matrix<T>::getNumColumns());
			}

			/* Calulates the RMS (root-mean-squared) value of the enitre matrix */
			T RMS()
			{
				return sqrt(1.0 / (juce::dsp::Matrix<T>::getNumRows() * juce::dsp::Matrix<T>::getNumColumns()) * sumOfSquares());
			}

			/* Calculates the sum of the square of each element */
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

			/**
			 * Creates the next iteration of a Hadamard matrix using Sylvester's construction.
			 * Each iteration is created by copying the matrix H into a new matrix in the following way:
			 * | H  H |
			 * | H -H |
			 */
			Matrix<T> createHadamardMatrixInternal(Matrix<T> in)
			{
				jassert(in.isSquare());
				int n = in.getNumRows();
				Matrix<T> out(2 * n, 2 * n);

				// iterate each element in the starting matrix
				for (int r = 0; r < n; ++r)
				{
					for (int c = 0; c < n; ++c)
					{
						// set the values in the new matrix according to Sylverster's construction
						auto v = in(r, c);
						out(r, c) = v;
						out(n + r, c) = v;
						out(r, n + c) = v;
						out(n + r, n + c) = -v;
					}
				}
				return out;
			}

			JUCE_LEAK_DETECTOR(Matrix)
		};
	}// namespace dsp
}// namespace bdsp

