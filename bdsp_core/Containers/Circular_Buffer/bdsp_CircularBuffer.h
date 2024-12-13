#pragma once
namespace bdsp
{
	/**
	 * FIFO Circular Buffer
	 * @tparam StorageType The type to store in the buffer
	 * @tparam PtrType The type to use for indexing the buffer - defaults to unsigned int
	 */
	template<typename StorageType, typename PtrType = unsigned int>
	class CircularBuffer
	{
	public:
		/**
		 * Creates buffer with a set size and intial value for each element
		 * @param num Initial size of the buffer - defaults to 0
		 * @param initialValue Value to fill each element with - defaults to 0
		 */
		CircularBuffer(PtrType num = PtrType(0), StorageType initialValue = StorageType())
		{
			resize(num);
			internalArray.fill(initialValue);
		}
		~CircularBuffer() = default;

		void operator ++()
		{
			updatePTR(1);
		}
		void operator --()
		{
			updatePTR(-1);
		}

		/**
		 * Moves the pointer through the buffer a certain number of elements
		 * @param amt Number of elements to move the pointer - negative amounts will move the pointer backwards
		 * @return The new value of the pointer
		 */
		PtrType updatePTR(PtrType amt)
		{
			if (amt < 0)
			{
				amt = -((-amt) % getSize()); // limits neagtive amount to (-size,0] - ensures following modulo is not called with a negative dividend
			}

			ptr = (ptr + amt + getSize()) % getSize();

			return ptr;
		}

		/**
		 * Gets a value from the buffer a certain number of elements from the pointer
		 * @param offset Number of elements from the pointer to read the value -  defaults to 0 (i.e. reads the value of the element the pointer is currently pointing to)
		 */
		StorageType get(PtrType offset = PtrType(0))
		{
			if (offset != 0)
			{
				PtrType tmp = ptr; // create a temporary ptr to store the original value of the ptr
				PtrType index = updatePTR(offset);
				ptr = tmp; // restore ptr back to its origianl value
				return internalArray.getUnchecked(index);
			}
			else
			{
				return internalArray.getUnchecked(ptr);
			}
		}

		/**
		 * Inserts a new value into an element a certain number of elements from the pointer
		 * @param v The value to insert
		 * @param offset Number of elements from the pointer to set the value -  defaults to 0 (i.e. sets the value of the element the pointer is currently pointing to)
		 * @param incrementPtr Whether to increment the pointer after setting the value
		 */
		void set(StorageType v, PtrType offset = PtrType(0), bool incrementPtr = true)
		{
			if (offset != 0)
			{
				PtrType tmp = ptr; // create a temporary ptr to store the original value of the ptr
				PtrType index = updatePTR(offset);
				ptr = tmp; // restore ptr back to its origianl value
				internalArray.set(index, v);
			}
			else
			{
				internalArray.set(ptr, v);
			}

			if (incrementPtr)
			{
				operator++();
			}
		}

		/**
		 * Resizes the buffer
		 * @param newSize The new size of the buffer
		 */
		void resize(PtrType newSize)
		{
			internalArray.resize(newSize);
		}

		/**
		 * Returns the array this class wraps.
		 * Useful when you want to iterate through the buffer and don't care about the position of the pointer
		 */
		juce::Array<StorageType>& getInternalArray()
		{
			return internalArray;
		}

		int getSize()
		{
			return internalArray.size();
		}


		/**
		 * Converts the buffer to an array starting with the element pointed to by the pointer
		 */
		juce::Array<StorageType> flatten()
		{
			juce::Array<StorageType> out;
			out.insertArray(0, internalArray.getRawDataPointer() + ptr, getSize() - ptr);
			out.insertArray(-1, internalArray.getRawDataPointer(), ptr);

			return out;

		}

		/**
		 * Returns an array containing a certain number of element from the element being pointed to
		 * @param numElements Number of elements to include in the subBuffer
		 */
		juce::Array<StorageType> getSubBuffer(int numElements)
		{
			int first = juce::jmin(numElements, getSize() - static_cast<int>(ptr)); // length of the first sub-buffer add to the output (either numElements or num element until the end of the interal array)
			juce::Array<StorageType> out;
			out.insertArray(0, internalArray.getRawDataPointer() + static_cast<int>(ptr), first);

			if (first < numElements) // if we still need to add more elements
			{
				out.insertArray(-1, internalArray.getRawDataPointer(), numElements - getSize() + ptr); // add elements from the beginning
			}


			return out;

		}
	private:
		PtrType ptr = PtrType(0);
		juce::Array<StorageType> internalArray;

	};

} // namespace bdsp