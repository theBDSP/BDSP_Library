#pragma once



namespace bdsp
{

	constexpr auto HeaderRatio = 0.15f;
	constexpr auto BodyRatio = 0.7f;

	enum class AlertItemPriority { Loweset, Normal, Highest };
	struct AlertWindowItem
	{
	public:
		AlertWindowItem()
			:Priority(AlertItemPriority::Normal)
		{
			DismissOnHide = false;
		}
		AlertWindowItem(const juce::String& leftName, const juce::String& centerName, const juce::String& rightName, const juce::String& headerText, const juce::String& bodyText, const std::function<void(int)>& selectFunction = std::function<void(int)>(), AlertItemPriority priority = AlertItemPriority::Normal, bool dismissOnHide = true)
		{
			LeftName = leftName;
			CenterName = centerName;
			RightName = rightName;
			HeaderText = headerText;
			BodyText = bodyText;
			SelectFunction = selectFunction;
			Priority = priority;
			DismissOnHide = dismissOnHide;
		}


		bool operator<(const AlertWindowItem& other) const
		{
			return Priority < other.Priority;
		}

		bool operator==(const AlertWindowItem& other)
		{
			return HeaderText == other.HeaderText;
		}

		bool operator!=(const AlertWindowItem& other)
		{
			return !(*this == other);
		}

		juce::String LeftName;
		juce::String CenterName;
		juce::String RightName;
		juce::String HeaderText;
		juce::String BodyText;
		std::function<void(int)> SelectFunction;
		AlertItemPriority Priority;
		bool DismissOnHide;

	};

	class AlertWindow : public DesktopComponent, public juce::ComponentListener
	{
	public:
		AlertWindow(std::function<void(bool)>& ShowHideFunction, GUI_Universals* universalsToUse);
		void reset(); // resets the window to a null state

		void addItemToQueue(const AlertWindowItem& item);
		void addMessageToQueue(const juce::String& buttonText, const juce::String& headerText, const juce::String& bodyText, AlertItemPriority priority = AlertItemPriority::Normal);

		AlertWindowItem getVisibleItem();


		void show() override;
		void hide() override;

		void resize();

		void setRelativeSize(float relativeWidth, float relativeHeight);
		void paint(juce::Graphics& g) override;

		void setColors(const NamedColorsIdentifier& newBKGD, const NamedColorsIdentifier& newHeaderBKGD, const NamedColorsIdentifier& newHeaderText, const NamedColorsIdentifier& newBodyText, const NamedColorsIdentifier& newButtonBKGDUp, const NamedColorsIdentifier& newButtonBKGDDown, const NamedColorsIdentifier& newButtonTextUp, const NamedColorsIdentifier& newButtonTextDown);

		void setFonts(int headerIndex, int bodyIndex);

		bool keyPressed(const juce::KeyPress& key) override;

		void setHighlighted(int idx);

		juce::Array<int> getAllValidIndecies();

		void lookAndFeelChanged() override;





		bool itemQueueIsEmpty();

		NamedColorsIdentifier BKGD, HeaderBKGD, HeaderText, BodyText, ButtonBKGDUp, ButtonBKGDDown, ButtonTextUp, ButtonTextDown;


	private:

		virtual void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;

		juce::Array<AlertWindowItem> ItemQueue;
		std::function<void(int)> onSelect;

		void makeNew(const AlertWindowItem& item);

		void removeFromQueue(const AlertWindowItem& item, bool showNext = true);

		class IndexedButton : public TextButton
		{
		public:
			IndexedButton(AlertWindow* p, int idx, const juce::String& text);
			~IndexedButton() override;

			void mouseEnter(const juce::MouseEvent& e) override;

			bool keyPressed(const juce::KeyPress& key)override;

			bool forceHighlight = false;
		private:
			//void paintButton(juce::Graphics& g, bool highlighted, bool down)override;
			int index;
			AlertWindow* parent;

		};

		std::unique_ptr<IndexedButton> leftButton, centerButton, rightButton;

		MousePassthrough<Label> Header, Body;

		melatonin::DropShadow dropShadow;



		float relWidth = 0.5f, relHeight = 0.5f;

		int highlightedButton;

		int headerFontIndex = 1;
		int bodyFontIndex = 2;


		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AlertWindow)
			JUCE_DECLARE_WEAK_REFERENCEABLE(AlertWindow)
	};

}// namnepace bdsp
