#pragma once
namespace bdsp
{



	/** Returns a rectangle with the same size, but a new x-center.
	*/
	template<typename T>
	juce::Rectangle<T> rectWithCenterX(const juce::Rectangle<T>& rect, T newCenterX) noexcept
	{
        return rect.withCentre({ newCenterX,rect.getCentreY() });
	}


	/** Returns a rectangle with the same size, but a new y-center.
	*/
	template<typename T>
	juce::Rectangle<T> rectWithCenterY(const juce::Rectangle<T>& rect, T newCenterY) noexcept
	{
        return rect.withCentre({ rect.getCentreX(),newCenterY });
	}

	/**
	 * Shrinks a rectangle to have a certain aspect ratio
	 * @param rect The rectangle to confine
	 * @param newAspectRatio The desired aspect ratio
	 * @param placement The placement of the confined rectangle within the original rectangle - defaults to centred
	 * @return The confined rectangle
	 */
	template<typename T>
	inline juce::Rectangle<T> confineToAspectRatio(const juce::Rectangle<T>& rect, const double& newAspectRatio, juce::RectanglePlacement placement = juce::RectanglePlacement::centred)
	{

		double w = rect.getWidth();
		double h = rect.getHeight();
		auto currentAspectRatio = w / h;

		//================================================================================================================================================================================================
		// create a new rectangle with the desired aspect ratio or return the provided on if it already has the desired aspect ratio
		juce::Rectangle<double> newRect;
		if ((currentAspectRatio - newAspectRatio) > (1.0 / h))// too wide
		{
			newRect = rect.toDouble().withWidth(h * newAspectRatio);
		}
		else if ((1 / currentAspectRatio - 1 / newAspectRatio) > (1.0 / w))// too tall
		{
			newRect = rect.toDouble().withHeight(w / newAspectRatio);
		}
		else if (newAspectRatio == 1) // edge case with certain display scalings may shift edges by a single pixel
		{
			auto min = juce::jmin(rect.getWidth(), rect.getHeight());
			return rect.withHeight(min).withWidth(min);
		}
		else
		{
			return rect;
		}
		//================================================================================================================================================================================================
		// place the new rectangle within the original rectangle
		auto x = rect.getX();
		auto y = rect.getY();

		if (placement.testFlags(juce::RectanglePlacement::xMid))
		{
			if (newRect.getWidth() / 2 != newRect.getWidth() / 2.0) // int rect and odd width
			{
				newRect = newRect.withWidth(newRect.getWidth() - 1);
			}

			x = rect.getCentreX() - newRect.getWidth() / 2;
		}
		if (placement.testFlags(juce::RectanglePlacement::yMid))
		{
			if (newRect.getHeight() / 2 != newRect.getHeight() / 2.0) // int rect and odd Height
			{
				newRect = newRect.withHeight(newRect.getHeight() - 1);
			}
			y = rect.getCentreY() - newRect.getHeight() / 2;
		}
		if (placement.testFlags(juce::RectanglePlacement::xRight))
		{
			x = rect.getRight() - newRect.getWidth();
		}
		if (placement.testFlags(juce::RectanglePlacement::yBottom))
		{
			y = rect.getBottom() - newRect.getHeight();
		}
		//================================================================================================================================================================================================

		return newRect.withPosition(x, y).template toType<T>();
	}


	/**
	 * Rounds a rectangles edges away from its center
	 * @param rect The rectangle to expand
	 * @param forceEvenWidth If true, forces the width of the resulting rectangle to be even - defaults to false
	 * @param forceEvenHeight If true, forces the height of the resulting rectangle to be even - defaults to false
	 * @return The expanded rectangle
	 */
	template<typename T>
	inline juce::Rectangle<int> expandRectangleToInt(const juce::Rectangle<T>& rect, bool forceEvenWidth = false, bool forceEvenHeight = false)
	{
		auto out = juce::Rectangle<int>::leftTopRightBottom(floor(rect.getX()), floor(rect.getY()), ceil(rect.getRight()), ceil(rect.getBottom()));
		if (forceEvenWidth && out.getWidth() % 2 == 1)
		{
			out.setWidth(out.getWidth() + 1);
		}
		if (forceEvenHeight && out.getHeight() % 2 == 1)
		{
			out.setHeight(out.getHeight() + 1);
		}
		return out;
	}

	/**
	 * Rounds a rectangles edges towards its center
	 * @param rect The rectangle to expand
	 * @param forceEvenWidth If true, forces the width of the resulting rectangle to be even - defaults to false
	 * @param forceEvenHeight If true, forces the height of the resulting rectangle to be even - defaults to false
	 * @return The shrunk rectangle
	 */
	template<typename T>
	inline juce::Rectangle<int> shrinkRectangleToInt(const juce::Rectangle<T>& rect, bool forceEvenWidth = false, bool forceEvenHeight = false)
	{
		auto out = juce::Rectangle<int>::leftTopRightBottom(ceil(rect.getX()), ceil(rect.getY()), floor(rect.getRight()), floor(rect.getBottom()));

		if (forceEvenWidth && out.getWidth() % 2 == 1)
		{
			out.setWidth(out.getWidth() - 1);
		}
		if (forceEvenHeight && out.getHeight() % 2 == 1)
		{
			out.setHeight(out.getHeight() - 1);
		}
		return out;
	}


} // namespace bdsp
