#pragma once
namespace bdsp
{
	/**
	 * Shifts the colors of a gradient, wrapping around the endpoints
	 * Negative amounts will shift in the opposite direction to positive amounts
	 *
	 * @param cg The gradient to modify
	 * @param amount Amount to shift the gradient by - must be in range (-1,1)
	 */
	inline void shiftGradient(juce::ColourGradient& cg, double amount)
	{
		jassert(abs(amount) < 1);

		auto newStart = cg.getColourAtPosition(1 - amount); // color at the new start point
		auto newEnd = cg.getColourAtPosition(amount); // color at the new end point

		juce::ColourGradient newCG(newStart, cg.point1, newEnd, cg.point2, cg.isRadial); // fill a new gradient with the new endpoint colors

		for (int i = 0; i < cg.getNumColours(); ++i) // loop through each specified color in the original gradient
		{
			newCG.addColour(fmod(cg.getColourPosition(i) + amount, 1.0), cg.getColour(i)); // add the color from the original gradient after shifting its position
		}

		cg = newCG;
	}

	/**
	 * Reverses a gradient's direction by swapping its endpoints
	 * @param cg The gradient to modify
	 */
	inline void flipGradient(juce::ColourGradient& cg)
	{
		std::swap(cg.point1, cg.point2);
	}

	/**
	 * Calculates a color for UI elements to indicate different states when clicked down and being hovered over
	 * @param downColor Color when fully down
	 * @param upColor Color when fully up
	 * @param isDown
	 * @param isHover
	 * @param amt How much of the other color is added when hovering
	 * @return The resulting color
	 */
	inline juce::Colour getHoverColor(const juce::Colour& downColor, const juce::Colour& upColor, bool isDown, bool isHover, float amt)
	{
		juce::Colour main, alt, out;

		if (isDown)
		{
			main = downColor;
			alt = upColor;


		}
		else
		{
			main = upColor;
			alt = downColor;
		}

		if (isHover)
		{
			out = main.interpolatedWith(alt, amt);
		}
		else
		{
			out = main;
		}
		return out;
	}

	enum colorChannel { Hue, Saturation, Brightness, Red, Green, Blue };

	/**
	 * Structure to pass into sorting function for juce Arrays, to sort an array of colors by a certain color channel
	 */
	struct ColorComparator
	{
		ColorComparator(colorChannel type)
			:c(type)
		{

		}
		int compareElements(juce::Colour first, juce::Colour second)
		{
			switch (c)
			{
			case colorChannel::Hue:
				return 255 * (first.getHue() - second.getHue());
				break;
			case colorChannel::Saturation:
				return 255 * (first.getSaturation() - second.getSaturation());
				break;
			case colorChannel::Brightness:
				return 255 * (first.getBrightness() - second.getBrightness());
				break;
			case colorChannel::Red:
				return first.getRed() - second.getRed();
				break;
			case colorChannel::Green:
				return first.getGreen() - second.getGreen();
				break;
			case colorChannel::Blue:
				return first.getBlue() - second.getBlue();
				break;
			default:
				return 0;
				break;

			}
		}

	private:
		colorChannel c;
	};

} // namespace bdsp