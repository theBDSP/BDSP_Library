#pragma once

namespace bdsp
{
	struct PresetManagerColors
	{

		PresetManagerColors(GUI_Universals* universalsToUse)
		{
			universals = universalsToUse;

			browserBKGD = BDSP_COLOR_DARK;
			itemHighlight = BDSP_COLOR_BLACK;


			presetText = BDSP_COLOR_WHITE;

			sortedColumnText = BDSP_COLOR_COLOR;

			menuBKGD = BDSP_COLOR_MID;
			menuPopupWidthRatio = 1.0 / 2.5;
			menuItemFontIndex = 2;

			tagBKGD = BDSP_COLOR_WHITE;
			tagBKGDSelected = BDSP_COLOR_COLOR;

			tagText = BDSP_COLOR_BLACK;
			tagTextSelected = BDSP_COLOR_BLACK;

			tagMenuBKGD = BDSP_COLOR_DARK;

			saveMenuTagBKGD = tagBKGD;
			saveMenuTagBKGDSelected = tagBKGDSelected;

			saveMenuTagText = tagText;
			saveMenuTagTextSelected = tagTextSelected;

			saveMenuBKGD = BDSP_COLOR_MID;
			saveMenuBKGDTop = BDSP_COLOR_DARK;
			saveMenuBKGDBot = BDSP_COLOR_DARK;
			saveMenuNameBarFontIndex = 1;



			scrollBar = BDSP_COLOR_LIGHT;
			scrollBarDown = BDSP_COLOR_WHITE;

			outline = BDSP_COLOR_LIGHT;
			emptyFav = BDSP_COLOR_WHITE;


			filledFav = BDSP_COLOR_COLOR;

			searchBarBKGD = outline;

			searchBarEmptyText = searchBarBKGD.mixedWith(presetText, 0.5);


			searchBarButtonBKGD = NamedColorsIdentifier(BDSP_COLOR_DARK).mixedWith(BDSP_COLOR_BLACK, 0.5);


			searchBarFontIndex = 1;

			saveMenuTitle = BDSP_COLOR_WHITE;
			saveMenuLabel = BDSP_COLOR_WHITE;
		}


		NamedColorsIdentifier browserBKGD;
		NamedColorsIdentifier itemHighlight;


		NamedColorsIdentifier presetText;
		NamedColorsIdentifier sortedColumnText;




		NamedColorsIdentifier tagBKGD;
		NamedColorsIdentifier tagBKGDSelected;

		NamedColorsIdentifier tagText;
		NamedColorsIdentifier tagTextSelected;

		NamedColorsIdentifier menuBKGD;
		NamedColorsIdentifier tagMenuBKGD;

		NamedColorsIdentifier saveMenuTagBKGD;
		NamedColorsIdentifier saveMenuTagBKGDSelected;

		NamedColorsIdentifier saveMenuTagText;
		NamedColorsIdentifier saveMenuTagTextSelected;

		NamedColorsIdentifier saveMenuBKGD;
		NamedColorsIdentifier saveMenuBKGDBot;
		NamedColorsIdentifier saveMenuBKGDTop;
		int saveMenuNameBarFontIndex;



		NamedColorsIdentifier scrollBar;
		NamedColorsIdentifier scrollBarDown;

		NamedColorsIdentifier outline;
		NamedColorsIdentifier emptyFav;

		NamedColorsIdentifier filledFav;

		NamedColorsIdentifier searchBarBKGD;
		NamedColorsIdentifier searchBarEmptyText;
		NamedColorsIdentifier searchBarButtonBKGD;
		int searchBarFontIndex;


		NamedColorsIdentifier saveMenuTitle;
		NamedColorsIdentifier saveMenuLabel;

		float menuPopupWidthRatio;
		int menuItemFontIndex;

		GUI_Universals* universals;


	};

#define strokeThicc 0.05
	//class PresetManagerLNF : public LookAndFeel
	//{
	//public:
	//	PresetManagerLNF(GUI_Universals* universalsToUse, PresetManagerColors* presetColorsToUse)
	//		: LookAndFeel(universalsToUse)
	//	{
	//		pc = presetColorsToUse;
	//		GUI_UniversalsChanged();


	//	}

	//	PresetManagerLNF(PresetManagerLNF& other)
	//		:PresetManagerLNF(other.universals, other.pc)
	//	{

	//	}

	//	~PresetManagerLNF()
	//	{

	//	}




	//	void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)override
	//	{

	//	}

	//	void drawLabel(juce::Graphics& g, juce::Label& label) override
	//	{
	//		auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
	//		auto alpha = label.isEnabled() ? 1.0f : 0.5f;

	//		auto core = dynamic_cast<ComponentCore*>(&label);
	//		jassert(core != nullptr);


	//		auto Font = universals->Fonts[core->getFontIndex()].getFont();

	//		g.setFont(Font.withHeight(textArea.getHeight()));



	//		if (!label.isBeingEdited())
	//		{

	//			g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::backgroundColourId), core->isHovering()).withMultipliedAlpha(alpha));
	//			g.fillAll();


	//			g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::textColourId), core->isHovering()).withMultipliedAlpha(alpha));

	//			//g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
	//			//	juce::jmax(1, (int)((float)textArea.getHeight() / Font.getHeight())),
	//			//	label.getMinimumHorizontalScale());

	//			drawText(g, g.getCurrentFont(), label.getText(), textArea, false, label.getJustificationType());
	//		}

	//		if (label.getCurrentTextEditor() != nullptr)
	//		{

	//			g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::backgroundWhenEditingColourId), core->isHovering()).withMultipliedAlpha(alpha));
	//			g.fillAll();
	//			label.getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, label.findColour(juce::TextEditor::highlightColourId));
	//			label.getCurrentTextEditor()->applyFontToAllText(Font.withHeight(textArea.getHeight()));



	//			label.getCurrentTextEditor()->setJustification(juce::Justification::centred);
	//		}

	//	}

	//	void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override
	//	{
	//		juce::Rectangle<float> rect;
	//		if (isScrollbarVertical)
	//		{
	//			rect = juce::Rectangle<float>(x, thumbStartPosition, width, thumbSize).reduced(0, thumbSize * 0.01);
	//		}
	//		else
	//		{
	//			rect = juce::Rectangle<float>(thumbStartPosition, y, thumbSize, height).reduced(thumbSize * 0.01, 0);
	//		}

	//		g.setColour(getHoverColor(universals->colors.getColor(pc->scrollBarDown), universals->colors.getColor(pc->scrollBar), isMouseDown, isMouseOver, universals->hoverMixAmt));


	//		g.fillRoundedRectangle(rect, juce::jmin(rect.getWidth(), rect.getHeight()) / 2.0);
	//	}

	//	void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& te) override
	//	{
	//		g.setColour(te.findColour(juce::TextEditor::outlineColourId));
	//		g.drawRoundedRectangle(0, 0, width, height, universals->roundedRectangleCurve, 1);
	//	}

	//	void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
	//	{
	//		g.setColour(textEditor.findColour(juce::TextEditor::backgroundColourId));
	//		g.fillRoundedRectangle(0, 0, width, height, universals->roundedRectangleCurve);
	//	}




	//	void GUI_UniversalsChanged() override
	//	{
	//		setColour(juce::Label::textColourId, universals->colors.getColor(pc->presetText));
	//		setColour(juce::Label::textWhenEditingColourId, universals->colors.getColor(pc->presetText));


	//		setColour(juce::Label::ColourIds::outlineColourId, universals->colors.getColor(pc->outline));
	//		setColour(juce::Label::ColourIds::outlineWhenEditingColourId, universals->colors.getColor(pc->outline));









	//		setColour(juce::ScrollBar::ColourIds::thumbColourId, universals->colors.getColor(pc->scrollBar));
	//	}



	//	PresetManagerColors* pc;









	//};


	//class PresetMenuItemLNF : public PresetManagerLNF
	//{
	//public:
	//	PresetMenuItemLNF(GUI_Universals* universalsToUse, PresetManagerColors* presetColorsToUse)
	//		:PresetManagerLNF(universalsToUse, presetColorsToUse)
	//	{

	//	}

	//	void drawLabel(juce::Graphics& g, juce::Label& label) override
	//	{
	//		auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
	//		auto alpha = label.isEnabled() ? 1.0f : 0.5f;

	//		auto core = dynamic_cast<ComponentCore*>(&label);
	//		jassert(core != nullptr);


	//		auto Font = universals->Fonts[core->getFontIndex()].getFont();

	//		g.setFont(Font.withHeight(textArea.getHeight()));



	//		if (!label.isBeingEdited())
	//		{

	//			g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::backgroundColourId), core->isHovering()).withMultipliedAlpha(alpha));
	//			g.fillAll();


	//			g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::textColourId), core->isHovering()).withMultipliedAlpha(alpha));

	//			//g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
	//			//	juce::jmax(1, (int)((float)textArea.getHeight() / Font.getHeight())),
	//			//	label.getMinimumHorizontalScale());

	//			//drawText(g, g.getCurrentFont(), label.getText(), textArea, label.getJustificationType());
	//			g.drawText(label.getText(), textArea, label.getJustificationType());
	//		}

	//		if (label.getCurrentTextEditor() != nullptr)
	//		{

	//			g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::backgroundWhenEditingColourId), core->isHovering()).withMultipliedAlpha(alpha));
	//			g.fillAll();
	//			label.getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, label.findColour(juce::TextEditor::highlightColourId));
	//			label.getCurrentTextEditor()->applyFontToAllText(Font.withHeight(textArea.getHeight()));



	//			label.getCurrentTextEditor()->setJustification(juce::Justification::centred);
	//		}

	//	}


	//};
}// namnepace bdsp
