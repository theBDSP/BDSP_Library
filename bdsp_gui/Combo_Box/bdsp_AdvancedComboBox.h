#pragma once
#include "bdsp_ComboBox.h"

namespace bdsp
{
	class AdvancedComboBox : public ComboBoxBase
	{
	public:


		AdvancedComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::Array<juce::Path>& paths, const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());


		AdvancedComboBox(const juce::StringArray& choiceNames, juce::RangedAudioParameter* param, GUI_Universals* universalsToUse, const juce::Array<juce::Path>& paths, const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());




		~AdvancedComboBox();



		void attach(juce::RangedAudioParameter* param, juce::UndoManager* undo = nullptr) override;


		void setItems(const juce::StringArray& items, const juce::Array<juce::Path>& paths, const juce::StringArray& dispNames = juce::StringArray());


		void setRowsAndCols(int rows, int cols);

		void setIconPlacement(juce::RectanglePlacement placement);
		void setJustification(juce::Justification justification);

	protected:

		class AdvancedList : public ListComponent
		{
		public:
			AdvancedList(AdvancedComboBox* p);
			~AdvancedList();

			class AdvancedListItem : public ListItem
			{
			public:
				AdvancedListItem(ListComponent* p, const juce::String& name, juce::Path path, int idx, int retValue = -1);

				void resized() override;
				void paint(juce::Graphics& g) override;

			private:
				juce::Path icon;
				AdvancedComboBox* parentACB;
			};


			void setItems(const juce::StringArray& names, const juce::Array<juce::Path>& paths, const juce::Array<int>& returnValues = juce::Array<int>());
			void addItem(const juce::String& name, juce::Path path, int returnValue);

			bool keyPressed(const juce::KeyPress& key) override;


		};
		std::unique_ptr<AdvancedList> advancedList;
		juce::Array<juce::Path> pathStorage;
		juce::RectanglePlacement iconPlacement;
		juce::Justification textJustification;


	};


	class GroupedComboBox : public ComboBoxBase
	{
	public:

		struct ItemGroupings : juce::Array<std::pair<juce::Range<int>, juce::String>>

		{
			ItemGroupings()
			{

			}

			ItemGroupings(const juce::Array<std::pair<int, juce::String>>& numPairs) // pair of num elements in group and title
			{
				int l = 0;

				for (auto p : numPairs)
				{
					add(std::pair<juce::Range<int>, juce::String>(juce::Range<int>(l, l + p.first), p.second));
					l += p.first;
				}
			}

			ItemGroupings(const juce::Array<std::pair<juce::Range<int>, juce::String>>& initArray) // pair of num elements in group and title
				:juce::Array<std::pair<juce::Range<int>, juce::String>>(initArray)
			{

			}


		};


		GroupedComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const ItemGroupings& groupings = ItemGroupings(), const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());


		GroupedComboBox(const juce::StringArray& choiceNames, juce::RangedAudioParameter* param, GUI_Universals* universalsToUse, const ItemGroupings& groupings = ItemGroupings(), const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());




		~GroupedComboBox();


		void resized() override;

		void attach(juce::RangedAudioParameter* param, juce::UndoManager* undo = nullptr) override;


		void setItems(const juce::StringArray& items, const ItemGroupings& groupings = ItemGroupings(), const juce::StringArray& dispNames = juce::StringArray());




	protected:

		class GroupedList : public ListComponent, public GUI_Universals::Listener
		{
		public:
			GroupedList(GroupedComboBox* p);
			~GroupedList();

			void setGroupBounds(int groupNum, juce::Rectangle<int> bounds, float border = -1);
			void paint(juce::Graphics& g) override;
			class GroupedListItem : public ListItem
			{
			public:
				GroupedListItem(ListComponent* p, const juce::String& name, juce::Justification just, int idx, int retValue = -1);

				void resized() override;
				void paint(juce::Graphics& g) override;

			private:
				GroupedComboBox* parentGCB;
				juce::Justification justification;
			};


			void setItems(const juce::StringArray& names, const ItemGroupings& groupings = ItemGroupings(), const juce::Array<int>& returnValues = juce::Array<int>());
			void addItem(const juce::String& name, juce::Justification just, int returnValue);

			bool keyPressed(const juce::KeyPress& key) override;

		private:
			GroupedComboBox* parentGCB;
			juce::OwnedArray<Label> Titles;


			// Inherited via Listener
			void GUI_UniversalsChanged() override;

		}groupedList;

		ItemGroupings groups;
	};
} // namespace bdsp