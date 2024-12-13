#pragma once




namespace bdsp
{


	//#define currentPopup "Current Popup Menu ID"

	class PopupMenu : public DesktopComponent, public GUI_Universals::Listener
	{
	public:
		PopupMenu(juce::Component* compToAttachTo, GUI_Universals* universalsToUse, bool preview = false);
		~PopupMenu();

		void resized() override;
		juce::Rectangle<int> ensureBoundsAreContained(juce::Rectangle<int> bounds, int border);
		void setBoundsContained(juce::Rectangle<int> bounds, int border);
		void setHeightRatio(float newRatio);
		void setItemBorderRatio(float newRatio);

		void dismissAllActiveMenus();

		void scroll(bool up);



		bool keyPressed(const juce::KeyPress& key) override;

		void lookAndFeelChanged() override;

		void resetLookAndFeels(GUI_Universals* universalsToUse) override;

		void setFontIndex(int idx) override;

		void addTitle(const juce::String& text, float ratio = 1);



		class MenuList :public ListComponent
		{
		public:
			MenuList(PopupMenu* p);
			~MenuList();


			void resized() override;

			void addItem(const juce::String& text, int returnValue);
			void setItems(const juce::StringArray& items, const juce::Array<int>& returnValues = juce::Array<int>());





			bool keyPressed(const juce::KeyPress& key) override;






		private:






			class MenuItem : public ListItem
			{
			public:
				MenuItem(MenuList* p, const juce::String& name, int idx, int r);
				~MenuItem();


				void paint(juce::Graphics& g) override;



			};


		}List;



	protected:



		Viewport vp;




		// Inherited via Listener
		void GUI_UniversalsChanged() override;

	};



}// namnepace bdsp
