#pragma once
namespace bdsp
{

	/**
	 * Wraps a juce::OwnedArray. Access elements with a key instead of an index.
	 * @see juce::OwnedArray
	 */
	template <typename keyType, typename valType>
	class OwnedMap
	{

	public:

		/**
		 * Adds a new element to the map
		 * @param k The key to associate with the new element
		 * @param args Arguments to pass to valType constructor when creating the new element
		 */
		template <class ... Types>
		void add(keyType k, Types...args)
		{
			vals.add(std::make_unique <valType>(args...));
			idxMap.insert(std::pair<keyType, int>(k, vals.size() - 1));
		}

		/**
		 * Gets a pointer to an object stored in the map
		 */
		valType* get(keyType k)
		{
			int idx = idxMap[k];
			return vals[idx];
		}

		/**
		 * Gets a pointer to an object stored in the map
		 */
		valType* operator[](keyType k)
		{
			return get(k);
		}

		/**
		 * Checks if the map has an object with an associated key 
		 * @param k The key to check
		 * @return True if the map has an object associated with the specified key
		 */
		bool contains(keyType k)
		{
			return idxMap.find(k) != idxMap.end();
		}

	private:
		std::unordered_map<keyType, int> idxMap;
		juce::OwnedArray<valType> vals;
	};
} // namespace bdsp