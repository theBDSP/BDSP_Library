#include "bdsp_PathButton.h"


namespace bdsp
{

	PathButton::PathButton(GUI_Universals* universalsToUse, bool toggle, bool on)
		:Button(universalsToUse)
	{
		setClickingTogglesState(toggle);
	}

	PathButton::~PathButton()
	{
	}

	PathButton& PathButton::operator=(const PathButton& oldButton)
	{
		upPath = oldButton.upPath;
		downPath = oldButton.downPath;

		upColor = oldButton.upColor;
		downColor = oldButton.downColor;

		return *this;
	}

	void PathButton::resized()
	{
		auto pathBounds = getLocalBounds().toFloat().reduced(pathBorder);
		scaleToFit(upPath, pathBounds);
		scaleToFit(downPath, pathBounds);
	}

	void PathButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		bool isDown = getToggleState() || shouldDrawButtonAsDown;
		g.setColour(getHoverColor(getColor(downColor), getColor(upColor), isDown, isHovering() || shouldDrawButtonAsHighlighted, universals->hoverMixAmt).withMultipliedAlpha(isEnabled() ? 1.0f : universals->disabledAlpha));

		g.fillPath(isDown ? downPath : upPath);

	}

	void PathButton::setPath(juce::Path down, juce::Path up)
	{
		downPath = down;

		upPath = up.isEmpty() ? down : up;

		resized();
	}

	void PathButton::setColor(const NamedColorsIdentifier& down, const NamedColorsIdentifier& up)
	{
		downColor = down;

		if (up.isEmpty())
		{
			upColor = downColor.withMultipliedAlpha(universals->disabledAlpha);
		}
		else
		{
			upColor = up;
		}

		repaint();
	}

	void PathButton::setPathBorder(float newBorder)
	{
		pathBorder = newBorder;
		resized();
		//repaint();
	}

}// namnepace bdsp
