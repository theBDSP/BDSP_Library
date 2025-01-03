#pragma once 
namespace bdsp
{
	/**
	 * A Wrapper for juce::HashMap that allows for multiple keys to point to the same value without inserting new value into map
	 * @see Juce::HashMap
	 */
	template<typename keyType, typename valueType>
	struct DoubleHashedMap
	{
	public:

		/**
		 * Creates an empy DoubleHashedMap
		 * @param conversionFunc Function that converts keys into intermediate values of the same type.
		 * This function is where you set how keys can overlap.
		 */
		DoubleHashedMap(std::function<keyType(keyType)>& conversionFunc)
		{
			firstHashFunction = conversionFunc;
		}

		valueType operator[](keyType key)
		{
			return map[getIntermediate(key)];
		}

		valueType& getReference(keyType key)
		{
			return map.getReference(getIntermediate(key));
		}

		void set(keyType key, valueType v)
		{
			map.set(getIntermediate(key), v);
		}


	private:

		/**
		 * Converts a key into the intermediate key used in the internal HashMap 
		 */
		int getIntermediate(keyType key)
		{
			keyType k = key;
			if (firstHashFunction.operator bool())
			{
				k = firstHashFunction(k);
			}

			return juce::DefaultHashFunctions::generateHash(k, map.getNumSlots());
		}
		juce::HashMap<int, valueType> map;
		std::function<keyType(keyType)> firstHashFunction;
	};

} // namespace bdsp