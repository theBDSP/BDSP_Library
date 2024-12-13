#pragma once


namespace bdsp
{

	class SaveMenu : public DesktopComponent, public juce::ComponentListener, public GUI_Universals::Listener
	{
	public:

		SaveMenu(PresetManager* parent);
		~SaveMenu();

		void clear();
		void resized() override;
		void matchItem(PresetMenuItem* MI);
		void matchPreviousState(SaveMenuState* prevState);
		void createHandlerSaveMenuState(bool isUndoable = false);

		void rebuildTags();

		void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;


		void mouseDown(const juce::MouseEvent& e) override;



	private:
		MultiShapeButton fav;
		TextButton cancel, save;

		class TagButtonComparator
		{
		public:
			TagButtonComparator()
			{

			}
			~TagButtonComparator() = default;

			int compareElements(TagMenu::TagButton* first, TagMenu::TagButton* second)
			{
				int out;

				if (first->getText().trim() == "+")
				{
					out = 1;
				}
				else if (second->getText().trim() == "+")
				{
					out = -1;
				}
				else
				{
					out = first->getText().compareNatural(second->getText());
				}

				return out;
			}
		};

		juce::OwnedArray<TagMenu::TagButton> tagButtons;
		juce::Array<TagMenu::TagButton*> pendingTagButtons; // tags created that have yet to be saved to a preset

		juce::OwnedArray<juce::OwnedArray<TagMenu::TagButton>> superTagButtons;
		juce::Array<TagMenu::TagButton*> pendingSuperTagButtons; // supertags created that have yet to be saved to a preset

		TextEditor nameBar;

		Viewport vp;
		juce::OwnedArray<juce::Component> tagSections;
		juce::Component tagSectionuniversals;

		void centerSection(juce::OwnedArray<TagMenu::TagButton>& arr, int col);

		// repaints on alert hide/show
		void componentVisibilityChanged(juce::Component& c) override;

		bool darken = false;

		double titleH, border;

		std::function<void(juce::OwnedArray<TagMenu::TagButton>&, int, int)> centerRow;

		PresetManager* p;

		juce::Path saveIcon;




		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;

	};
} // namespace bdsp