#pragma once
namespace bdsp
{


	/**
	 * Wraps a juce::OwnedArray. Access elements with a key instead of an index.
	 * @see juce::OwnedArray
	 */
	template <typename keyType, typename valType, class HashFunctionType = std::hash<keyType>>
	class OwnedMap
	{

	public:

		/**
		 * Adds a new element to the map
		 * @param k The key to associate with the new element
		 * @param newObject The new object you want to add to the map
		 */
		void add(keyType k, std::unique_ptr<valType> newObject)
		{
			map[k] = newObject.release();
		}

		/**
		 * Adds a new element to the map
		 * @param k The key to associate with the new element
		 * @param newObject The new object you want to add to the map
		 */
		void add(keyType k, valType* newObject)
		{
			map[k] = nullptr;
			map[k].reset(newObject);
		}

		/**
		 * Gets a pointer to an object stored in the map
		 */
		valType* get(keyType k)
		{
			return map[k].get();
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
			return map.find(k) != map.end();
		}

	private:
		std::unordered_map<keyType, std::unique_ptr<valType>, HashFunctionType> map;
	};
} // namespace bdsp