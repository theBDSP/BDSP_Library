#pragma once 


namespace bdsp
{

	class Label : public juce::Label, public ComponentCore
	{
	public:
		template <class ... Types>
		Label(GUI_Universals* universalsToUse, Types...args)
			:juce::Label(args...),
			ComponentCore(this, universalsToUse)
		{
			setJustificationType(juce::Justification::centred);
		}

		//Label()
		//	:Label(nullptr)
		//{

		//}
		~Label() = default;


		void paint(juce::Graphics& g) override
		{
			if (universals != nullptr)
			{

				auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;


				auto max = maxText.isEmpty() ? getText() : maxText;


				g.setColour(findColour(juce::Label::backgroundColourId).withMultipliedAlpha(alpha));
				g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);


				//g.setFont(bdsp::resizeFontToFit(getFont(bdspLabel->getFontIndex()), textArea.getWidth()*0.9, textArea.getHeight()*0.9, maxText));
				g.setFont(universals->Fonts[getFontIndex()].getFont().withHeight(getHeight() * heightRatio * BDSP_FONT_HEIGHT_SCALING));


				if (!isBeingEdited())
				{
					g.setColour(universals->hoverAdjustmentFunc(findColour(juce::Label::textColourId), isHovering()).withMultipliedAlpha(alpha));

					drawText(g, g.getCurrentFont(), getText(), getLocalBounds(), false, getJustificationType());


				}

				if (getCurrentTextEditor() != nullptr)
				{
					getCurrentTextEditor()->applyFontToAllText(g.getCurrentFont());

					getCurrentTextEditor()->setJustification(getJustificationType());
					getCurrentTextEditor()->setIndents(0, 0);
					getCurrentTextEditor()->setBorder(juce::BorderSize<int>(0));
					getCurrentTextEditor()->setPopupMenuEnabled(false);

				}
			}
		}


		void setMaxText(const juce::String& newMax)
		{
			maxText = newMax;
		}



		juce::String getMaxText()
		{
			return maxText;
		}

		void linkToComponent(ComponentCore* compToLinkTo)
		{
			addHoverPartner(compToLinkTo);
			addMouseListener(compToLinkTo->getComponent(), false);
			linked = true;
		}

		float getHeightRatio()
		{
			return heightRatio;
		}

		void setHeightRatio(float newRatio)
		{
			jassert(newRatio <= 1.0f && newRatio > 0);
			heightRatio = newRatio;
		}

	private:
		juce::String maxText;
		bool linked = false;

		float heightRatio = 0.75;


	};


} // namespace bdsp