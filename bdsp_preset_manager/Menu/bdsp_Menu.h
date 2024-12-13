#pragma once

namespace bdsp
{

	class PresetManager;
	class TagMenu;
	class PresetMenu : public juce::Component, public GUI_Universals::Listener, juce::ComponentListener
	{
	public:
		PresetMenu(PresetManager* p);

		~PresetMenu();
		void reBuildMenu(bool performOnLoadFunction = true);
		void resized() override;

		void goToItem(PresetMenuItem* MI);
		void goToItem(int idx);

		void keepItemInSafeArea(PresetMenuItem* MI);

		void reSort(int columnIDX, bool reversed);

		void tagFilter(const juce::StringArray& currentTags, const juce::StringArray& currentSuperTags); // filters based on tags, if empty arrays are passed in no filtering is applied

		void updateItem(int index, const juce::File& newFile);

		PresetMenuItem* findItem(const juce::File& file);
		PresetMenuItem* findItem(const juce::String& name);

		int findItemIndex(const juce::File& file);

		void setItemFontIndex(int index);





		void deleteItems(juce::Array<PresetMenuItem*>& itemsToDelete);




		void updateIndecies();

		void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;

		void mouseDown(const juce::MouseEvent& e) override;
		void mouseDoubleClick(const juce::MouseEvent& e) override;
		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override;
		bool keyPressed(const juce::KeyPress& key) override;


		void scroll(bool up, bool multi = false);


		void singleClick(PresetMenuItem* MI);
		void modClick(PresetMenuItem* MI, bool ctrl, bool shift);
		void popupClick(juce::Point<int> mouseDownPosition); // mouse position relative to desktop manager
		void doubleClick(PresetMenuItem* MI);
		void favClick(PresetMenuItem* MI, bool state);

		// x value of splits between columns (includes boundary @ end == getWidth())
		juce::Array<int> getColumnSplits();


		void resetHighlights();
		void hideAllPopupMenus();

		void previewItem(PresetMenuItem* MI, bool perfromOnLoadFunction = true, bool isUndoable = true); // single click
		void toggleSingleHighlight(PresetMenuItem* MI); // ctrl click

		void groupHighlight(PresetMenuItem* MI, bool Ctrl);  // shift clicking

		void updateHighlights();

		void createPopupMenu(juce::Point<int>& clickPos, bool highlighted);

		void rename(PresetMenuItem* MI);

		juce::Array<PresetMenuItem*>& getHighlightQueue();



		void setDefaultPreset(PresetMenuItem* MI);
		void reInstallFactoryPresets();

		void initToProperItem();

		void updateItemCountText();

		PresetManager* parentManager;


		std::unique_ptr<PopupMenu> Popup;

		TextEditor search;
		MultiShapeButton clearSearch;

		// normal vp that doesnt react to mouse scrolls
		class passthroughVP : public Viewport
		{
		public:
			passthroughVP(PresetMenu* parent);
			~passthroughVP() = default;


			bool keyPressed(const juce::KeyPress& key) override;
			void paint(juce::Graphics& g) override;


		private:
			PresetMenu* p;

		}vp;

		juce::OwnedArray<PresetMenuItem> items;

		juce::Component list;

		TagMenu* tags = nullptr;
		float tagMenuH = 0;

		struct sorting { int col; bool rev; bool fav; } currentSorting = { 0,false, false };
		float itemHRatio = 0.075;
	private:


		juce::Rectangle<int> safeRect;

		juce::Array<PresetMenuItem*> highlightQueue;

		class ColumnSorter
		{
		public:
			ColumnSorter(int col, bool reversed, bool favorite)
			{
				column = col;
				rev = reversed;
				fav = favorite;
			}

			int compareElements(PresetMenuItem* first, PresetMenuItem* second)
			{
				juce::int64 result = 0;
				if (first->isVisible() == second->isVisible()) // items have same visibility
				{
					if (fav)
					{
						result = (second->isFavorite() ? 1 : 0) - (first->isFavorite() ? 1 : 0);
					}


					if (result == 0) // same fav state	|| not sorting by favs				
					{
						if (column == 0)
						{
							result = first->label.getText().compareNatural(second->label.getText());
						}
						else if (column == first->getSuperTags().size()) // Date
						{
							result = (second->getFile().getLastModificationTime() - first->getFile().getLastModificationTime()).inMilliseconds();
						}
						else
						{
							result = first->getSuperTag(column - 1).compareNatural(second->getSuperTag(column - 1));
						}

						result *= rev ? -1 : 1;

					}

				}
				else
				{
					result = (second->isVisible() ? 1 : 0) - (first->isVisible() ? 1 : 0);
				}

				return signum(result);
			}


		private:
			int column;
			bool rev, fav;

		};
		MultiShapeButton favColumnHeader;

		juce::OwnedArray<TextButton> columnHeaders;

		juce::StringArray selectedTags, selectedSuperTags;

		//================================================================================================================================================================================================


		bool deleteItemInternal(PresetMenuItem* MI);


		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;

		virtual void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;


		PresetMenuItem* pendingGoToItem = nullptr; // if goToItem is called before the menu has a size the item is stored here to be called after resized

		int numItems = 0;
		int numItemsVisible = 0;
	};



} // namespace bdsp
