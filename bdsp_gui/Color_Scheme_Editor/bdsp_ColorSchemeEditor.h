//#pragma once
//
//
//#include"bdsp_gui/Color_Scheme_Editor/Color_Selector/bdsp_ColorSelector.h"
//
////TODO Add to Settings Menu
//// Add default schemes
//// add way to reload factory schemes
//namespace bdsp
//{
//	class ColorSchemeEditor : public DesktopComponent, public juce::ChangeListener
//	{
//	public:
//		ColorSchemeEditor(GUI_Universals* universalsToUse);
//		~ColorSchemeEditor() = default;
//
//		void paint(juce::Graphics& g) override;
//		void paintOverChildren(juce::Graphics& g) override; // outline over selected item
//		void resized() override;
//
//		void placeItems();
//		void writeSchemeToFile();
//		void readSchemeFromFile(juce::File f);
//		void readSchemeFromFile(const juce::String& fileName);
//
//		void changeListenerCallback(juce::ChangeBroadcaster* source) override;
//	private:
//
//		class ColorItem : public Component
//		{
//		public:
//			ColorItem(ColorSchemeEditor* parent, const juce::Identifier& id);
//
//			void paint(juce::Graphics& g) override;
//			void resized() override;
//
//			void mouseDown(const juce::MouseEvent& e) override;
//			void setColor(juce::Colour c);
//			juce::Colour getColor();
//			juce::String getColorID();
//		private:
//			ColorSchemeEditor* p = nullptr;
//			juce::Colour color;
//			juce::String colorID;
//			int universalsIndex = 0;
//		};
//
//		ColorSelector selector;
//
//
//		juce::OwnedArray<ColorItem> colorItems;
//		ColorItem* selectedItem = nullptr;
//
//		TextEditor name;
//		TextButton saveButton, cancelButton;
//
//		juce::Rectangle<float> itemArea;
//
//	public:
//		void selectItem(ColorItem* item);
//	};
//} // namespace bdsp