#pragma once

#include "BDSP_Library/bdsp_preset_manager/bdsp_preset_manager.h"

namespace bdsp
{

	class PresetManager : public juce::Value::Listener, public juce::ChangeListener
	{
	public:

		PresetManager(GUI_Universals* universalsToUse, StateHandler* handlerToUse, AlertWindow* alertToUse);
		~PresetManager();



		void setFontIndex(int idx, float revertRatio);

		void saveNewPreset();
		void updateFavorite(PresetMenuItem* MI, bool state);
		void overwritePreset(PresetMenuItem* MI);
		void loadPreset(PresetMenuItem* MI, bool performOnLoadFunction = true, bool isUndoable = true);

		void initializePlugin(bool isUndoable = true);

		void openBrowser();
		void closeBrowser();

		PresetManagerColors pc;
		std::unique_ptr<TagMenu> tagMenu;

		StateHandler* Handler;
		GUI_Universals* universals = nullptr;
		PropertiesFolder* pFolder;

		std::unique_ptr<PresetMenu> menu;

		MultiShapeButton fav, left, right;

		juce::WeakReference<AlertWindow> alert;

		std::function<void(bool)> browserOpenClose;



		float scrollBarW = 0;

		class Browser : public juce::Component
		{
		public:
			Browser(PresetManager* p, GUI_Universals* universalsToUse);
			~Browser() = default;

			void resized() override;
			void paint(juce::Graphics& g) override;

			void setMacroH(int newH);


		private:
			PresetManager* parent;
			int macroH = 0;

		} browser;


		//================================================================================================================================================================================================
		class TitleBar : public Component
		{
		public:
			TitleBar(PresetManager* p, GUI_Universals* universalsToUse);
			~TitleBar() = default;
			void resized() override;
			void paint(juce::Graphics& g) override;

			class TitleBarButton : public Button
			{
			public:
				TitleBarButton(TitleBar* p, GUI_Universals* universalsToUse);
				void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
				TitleBar* parent;
			};
			std::unique_ptr<TitleBarButton> nameButton;

		private:
			PresetManager* parent;
		} titleBar;





		PresetMenuItem* loadedPreset = nullptr;

		std::unique_ptr<SaveMenu> saveMenu;


		MultiShapeButton titleBarHamburgerMenu;

		std::unique_ptr<PopupMenu> titleBarPopup;


		std::function<void()> onPresetLoad;

		juce::String numItems, totalItems;
	private:


		// Inherited via Listener
		void valueChanged(juce::Value& value) override; // updates preset name and altered state in titlebar

		void updateLoadedPreset(PresetMenuItem* MI);





		// Inherited via ChangeListener
		virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override; // for updating altered state on undo/redo

		JUCE_LEAK_DETECTOR(PresetManager)


	};
} // namespace bdsp