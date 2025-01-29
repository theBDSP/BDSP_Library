#pragma once 


#define BDSP_POPUP_MENU_ITEM_HEIGHT_RATIO 0.2
#define BDSP_POPUP_MENU_TITLE_HEIGHT_RATIO 0.2

#define BDSP_POPUP_MENU_OUTLINE_RATIO 0.05

namespace bdsp
{


	class ListItem;


	class ListComponent : public Component
	{
	public:
		ListComponent(ComponentCore* p);
		~ListComponent();

		void setRowsAndCols(int r, int c);

		void setColSplits(const juce::Array<std::pair<juce::Range<int>, int>>& splits);


		void postionItemsWithinRectangle(const juce::Rectangle<float>& rect);
		void resized() override;
		void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;

		float getIdealHeight(float width);

		void setPreviewOnHighlight(bool newState);

		void selectItem(int idx, bool sendUpdate = true);
		void highlightItem(int idx);
		void clearHighlights();
		int getHighlightedIndex();
		void scroll(bool up, int num = 1);

		int getNum();
		int getRows();
		int getCols();
		void renameItem(int idx, const juce::String& newText);
		virtual void clearItems();

		void setMaxText(const juce::String& newText);



		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)override;

		void setColorSchemeClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const juce::Array<NamedColorsIdentifier>& newHighlight);
		void setColorSchemeInvertedClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText);
		void setColorSchemeMinimal(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const NamedColorsIdentifier& newDivider, float nonHighlightedAlpha = 0.75f);
		void setAllColors(const NamedColorsIdentifier& newBkgd, const juce::Array<NamedColorsIdentifier>& newHighlight, const juce::Array<NamedColorsIdentifier>& newText, const juce::Array<NamedColorsIdentifier>& newHighlightedText, const NamedColorsIdentifier& newBorder, const NamedColorsIdentifier& newDivider);

		void pushColorsToNewItem();


		std::function<void(int)> onSelect;
		std::function<void(int, const juce::MouseEvent&, const juce::MouseWheelDetails&)> onMouseWheelMove;
		std::function<void(int, const juce::MouseEvent&)> onMouseEnter;
		std::function<void(int, const juce::MouseEvent&)> onMouseExit;
		ComponentCore* parent;

		melatonin::DropShadow dropShadow;

		NamedColorsIdentifier background, borderColor, divider;
		juce::String maxText = juce::String();

		juce::String title;
		float titleHeightRatio = 0;
		float heightRatio = BDSP_POPUP_MENU_ITEM_HEIGHT_RATIO;
		float itemBorder = BDSP_POPUP_MENU_OUTLINE_RATIO;

	protected:

		int rows = 1, cols = 1;
		int colsInLastRow = 0;
		juce::Array<std::pair<juce::Range<int>, int>> colSplits;
		juce::Rectangle<float> insideBorder;
		int HighlightedIndex = -1;

		std::function<void(int)> newItemColorAssigner;


	public:
		bool previewOnHighlight = true;
		bool roundHighlights = true;
		juce::OwnedArray<ListItem> List;
		juce::Justification justification;

	};


	class ListItem : public Component
	{
	public:
		ListItem(ListComponent* p, int idx, int retValue = -1);
		~ListItem();

		void mouseUp(const juce::MouseEvent& e)override;


		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)override;


		void mouseEnter(const juce::MouseEvent& e)override;

		void mouseExit(const juce::MouseEvent& e)override;


		bool keyPressed(const juce::KeyPress& key) override;

		void enablementChanged() override;
		bool isFirst();
		bool isLast();

		bool isHighlighted();

		void setText(const juce::String& s);
		juce::String getText();

		int returnValue;

		NamedColorsIdentifier highlightColor, textColor, textHighlightedColor;
	protected:
		ListComponent* parent;

		juce::String text;
		int index;


	};


} // namespace bdsp
