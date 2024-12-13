#pragma once
namespace bdsp
{

	/**
	 * Wraps a class and only contructs it if it gets called. Great for expensive objects that may not get used.
	 * @tparam T The class to wrap - this class must have a parameterless constructor
	 */
	template <class T>
	struct LazyLoad
	{
		/**
		 * Overload the -> operator to create a unique ptr if the object doesn't yet manage anything
		 */
		inline T* operator->() noexcept
		{
			if (!ptr.operator bool())
			{
				ptr = std::make_unique<T>();
			}

			return ptr.get();

		}

	private:
		std::unique_ptr<T> ptr;
	};
}