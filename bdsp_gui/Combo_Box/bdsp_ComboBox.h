#pragma once






namespace bdsp
{
	class ComboBoxBase : public Component, public ChoiceComponentCore
	{
	public:
		ComboBoxBase(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());


		ComboBoxBase(const ComboBoxBase& other);

		virtual ~ComboBoxBase();

		void initMenuComponents(); // call after menu components have been created




		DesktopComponent* getListuniversals();
		ListComponent* getListComponent();




		virtual void highlightItem(int i);
		void scrollItems(bool up) override;

		void setItemEnablement(int idx, bool enabled);

		void showList();

		void hideList();

		bool isListVisible();

		void resized() override;



		void setColorSchemeClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const juce::Array<NamedColorsIdentifier>& newHighlight);
		void setColorSchemeInvertedClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText);
		void setColorSchemeMinimal(const NamedColorsIdentifier& newMenuBkgd, const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const NamedColorsIdentifier& newDivider, float nonHighlightedAlpha = 0.75f);
		virtual void setAllColors(const NamedColorsIdentifier& newBackgroundUp, const NamedColorsIdentifier& newBackgroundDown, const NamedColorsIdentifier& newTextUp, const NamedColorsIdentifier& newTextDown, const NamedColorsIdentifier& newMenuHighlight, const NamedColorsIdentifier& newMenuHighlightedText, const NamedColorsIdentifier& menuBackground, const NamedColorsIdentifier& menuText, const NamedColorsIdentifier& newBorder, const NamedColorsIdentifier& newDivider);


		virtual void colorsChanged();




		void clearLookAndFeels() override;
		void resetLookAndFeels(GUI_Universals* universalsToUse) override;


		void setFontIndex(int idx) override;







		void setItemBorderRatio(float newRatio);


		void setRoundedHighlights(bool shouldRoundHighlights);

		void setMenuWidth(float newW);
		void setMenuTextHRatio(float newH);

		void setDropDownArrow(bool shoudlHaveDropDownArrow);


		void setIcons(const juce::Array<juce::Path>& iconsToAdd, const juce::BorderSize<float>& border = juce::BorderSize<float>());
		void setIconBorderSize(juce::BorderSize<float> newBorder);
		void paint(juce::Graphics& g)override;

		void setCornerCurves(CornerCurves curveValues);


		const juce::StringArray& getDisplayNames();


		std::function<void(int)> onChange;




	protected:

		juce::StringArray displayNames;
		juce::StringArray altNames;


		juce::Colour Highlight;


		CornerCurves curvesVal = CornerCurves(topLeft | topRight | bottomLeft | bottomRight); // defualt all curves



		;


		NamedColorsIdentifier backgroundUp, backgroundDown, textUp, textDown;
		NamedColorsIdentifier definedTextColorUp, definedTextColorDown; // if empty will set textUP/DOWN to menu item color

		bool dropdownArrow = true;


		float menuW = 1.75;
		float menuTextHRatio = 0.9;

		juce::OwnedArray<juce::Path> icons;





		juce::BorderSize<float> iconPadding;



		juce::String boxText;

		std::unique_ptr<DesktopComponent> Listuniversals; // must create in subclass constructor
		ListComponent* List = nullptr;
		//============================================================================

		class DropdownButton : public Component
		{
		public:
			DropdownButton(ComboBoxBase* parent);
			void mouseDown(const juce::MouseEvent& e) override;
			void mouseUp(const juce::MouseEvent& e) override;

			void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override;

		private:
			ComboBoxBase* p = nullptr;
			bool preClickState = false;
		};
		std::unique_ptr<DropdownButton> button;



		//============================================================================

	public:


		DropdownButton* getButton();







		// Inherited via ChoiceComponentCore
		void parameterChanged(int idx) override;


	protected:
		void updateColors();

	private:
		void pushNewColors();
	};


	class ComboBox : public ComboBoxBase
	{
	public:


		ComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());


		ComboBox(const juce::StringArray& choiceNames, juce::RangedAudioParameter* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());




		~ComboBox();







		void setHeightRatio(float newRatio);




















	protected:

















		//============================================================================
	public:


		void attach(juce::RangedAudioParameter* param, juce::UndoManager* undo = nullptr) override;


		void setItems(const juce::StringArray& items, const juce::StringArray& dispNames = juce::StringArray());












	};


}// namnepace bdsp

