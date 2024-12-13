#pragma once

namespace bdsp
{

	class MacroComponentLNF : public LookAndFeel
	{
	public:
		MacroComponentLNF(GUI_Universals* universalsToUse)
			:LookAndFeel(universalsToUse)
		{
		}

		~MacroComponentLNF()
		{

		}

		void drawLabel(juce::Graphics& g, juce::Label& label) override
		{

			//auto alpha = label.isEnabled() ? 1.0f : universals->disabledAlpha;

			//auto bdspLabel = dynamic_cast<Label*>(&label);
			//jassert(bdspLabel != nullptr);

			//auto maxText = bdspLabel->getMaxText();
			//maxText = maxText.isEmpty() ? label.getText() : maxText;

			//const auto& textArea = label.getLocalBounds().withSizeKeepingCentre(label.getWidth() * 0.9, label.getHeight() * 0.95);

			////g.setFont(resizeFontToFit(getFont(bdspLabel->getFontIndex()), textArea.getWidth(), textArea.getHeight()/2, universals->Fonts[bdspLabel->getFontIndex()].getWidestString(BDSP_MAX_MACRO_NAME_LENGTH/2)));


			//g.setFont(getFont(bdspLabel->getFontIndex()).withHeight(textArea.getHeight() * bdspLabel->getHeightRatio() * BDSP_FONT_HEIGHT_SCALING));




			//g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::backgroundColourId), bdspLabel->isHovering()).withMultipliedAlpha(alpha));
			//g.fillRoundedRectangle(label.getLocalBounds().toFloat(), universals->roundedRectangleCurve);

			//if (!label.isBeingEdited())
			//{
			//	g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::textColourId), bdspLabel->isHovering()).withMultipliedAlpha(alpha));
			//	//bdsp::drawText(g, g.getCurrentFont(), label.getText(), textArea,false, label.getJustificationType());
			//	//				//g.drawFittedText(label.getText(), textArea, label.getJustificationType(), 2);
			//	g.drawText(label.getText(), textArea, juce::Justification::centred, true);
			//}
			//else
			//{


			//	//label.getCurrentTextEditor()->applyFontToAllText(resizeFontToFit(getFont(bdspLabel->getFontIndex()), textArea.getWidth(), textArea.getHeight(), universals->Fonts[bdspLabel->getFontIndex()].getWidestString(BDSP_MAX_MACRO_NAME_LENGTH)));
			//	label.getCurrentTextEditor()->applyFontToAllText(g.getCurrentFont());


			//	label.getCurrentTextEditor()->setJustification(label.getJustificationType());
			//	label.getCurrentTextEditor()->setIndents(0, 0);
			//	label.getCurrentTextEditor()->setBorder(juce::BorderSize<int>());
			//	label.getCurrentTextEditor()->setPopupMenuEnabled(false);
			//	label.getCurrentTextEditor()->setMultiLine(false);

			//}


		}


		void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& te) override
		{
			auto alpha = te.isEnabled() ? 1.0f : universals->disabledAlpha;

			g.setColour(universals->hoverAdjustmentFunc(te.findColour(juce::TextEditor::backgroundColourId), te.isMouseOver()).withMultipliedAlpha(alpha));
			g.fillAll();
		}

	};

} // namespace bdsp
