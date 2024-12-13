#pragma once



namespace bdsp
{
	class TabsComponent : public Component, public juce::DragAndDropTarget
	{
	public:

		TabsComponent(GUI_Universals* universalsToUse, const juce::StringArray& tabNames);
		~TabsComponent() = default;


		virtual void setVertical(bool shouldBeVertical);
		void setTabRatio(float newTabRatio);
		void setCorners(CornerCurves newCorners);
		virtual void setNames(const juce::StringArray& newNames);

		void setHasCutout(bool shouldHaveCutout, float ratio = 1);
		juce::Rectangle<int> getCutout() const;

		void resized() override;
		void paint(juce::Graphics& g) override;



		void setColors(const NamedColorsIdentifier& bkgdColor, const NamedColorsIdentifier& buttonColor);
		void setColors(const NamedColorsIdentifier& bkgdColor, const NamedColorsIdentifier& textUp,const NamedColorsIdentifier& textDown);
		void setColors(const NamedColorsIdentifier& bkgdColor, const juce::Array<NamedColorsIdentifier>& buttonColors);

		TextButton* getButton(int idx);

		juce::Rectangle<int> getUsableArea();
		juce::Rectangle<int> getTabArea();
		juce::Rectangle<int> getTabArea(int idx);

		juce::Component* getPage(int idx);

		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override;

		int getSelectedTab() const;
		void selectTab(int idx);
		void notifyTabSelection(int idx);
		std::function<void(int)> onTabChange;

		class Listener
		{
		public:
			Listener() = default;
			virtual ~Listener() = default;

			virtual void tabSelected(TabsComponent* componentThatHadTabChanged) = 0;
			virtual void tabHovered(TabsComponent* componentThatHadTabChanged) = 0;
		};

		void addListener(Listener* listenerToAdd);
		void removeListener(Listener* listenerToRemove);



		double border = 0;

	protected:

		juce::OwnedArray<juce::Component> pages;

		;

		NamedColorsIdentifier bkgd;

		bool scaleTitles = true;
		juce::OwnedArray<TextButton> buttons;
		juce::Array<Listener*> listeners;


		juce::Rectangle<int> tabBar;
		juce::StringArray tabNames;

		CornerCurves corners = CornerCurves(CornerCurves::topLeft | CornerCurves::topRight | CornerCurves::bottomLeft | CornerCurves::bottomRight);

		int selectedTab = 0;

		bool isVertical = false;
		float tabRatio = 0.05;

		bool hasCutout = false;
		juce::Rectangle<int> cutoutRect;
		float cutoutRatio = 1;

		bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;


		void itemDragEnter(const SourceDetails& dragSourceDetails) override;
		void itemDragMove(const SourceDetails& dragSourceDetails) override;



		// Inherited via DragAndDropTarget
		void itemDropped(const SourceDetails& dragSourceDetails) override;

	};

} // namespace bdsp
