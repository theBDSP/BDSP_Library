#pragma once


#define BDSP_DISTORTION_TYPE_GRID_ASPECT_RATIO 0.8
namespace bdsp
{

	class DistortionTypeComboBox : public ComboBoxBase
	{
	public:
		DistortionTypeComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const std::function<void(int)>& changFunc = std::function<void(int)>());
		void setCurveColor(const NamedColorsIdentifier& c);

		void setRowsAndCols(int rows, int cols);



	private:
		class DistortionList : public ListComponent
		{
		public:
			DistortionList(DistortionTypeComboBox* p);
			~DistortionList();



			class DistortionListItem : public ListItem
			{
			public:
				DistortionListItem(ListComponent* parentList, const juce::String& name, juce::Path path, juce::Path fillPath, int idx, int retValue = -1);

				void resized() override;
				void paint(juce::Graphics& g) override;

			private:
				juce::Path p, fill;
				juce::Rectangle<float> pathBounds;
			};




		}DistortionList;


	public:
		NamedColorsIdentifier curve;
	};


	class DistortionTypeSelectorGrid : public IncrementalComboBox<>, public juce::ComponentListener
	{
	public:
		DistortionTypeSelectorGrid(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, Component* newGridSibling);
		~DistortionTypeSelectorGrid();

		void colorsChanged() override;


		void setRowsAndCols(int rows, int cols);

		void setGridSibling(Component* newGridSibling);

		void resized() override;

		void paint(juce::Graphics& g) override;
	private:

		class Grid : public ListComponent
		{
		public:

			Grid(DistortionTypeSelectorGrid* p);
			void paint(juce::Graphics& g) override;
			void resized() override;

			juce::String label;
			bool showIcon;
		private:
			class Item : public ListItem
			{
			public:
				Item(ListComponent* parentList, const juce::String& name, juce::Path path, int idx, int retValue = -1);

				void resized() override;
				void paint(juce::Graphics& g) override;

				void mouseEnter(const juce::MouseEvent& e) override;
				void mouseExit(const juce::MouseEvent& e) override;

			private:
				juce::Path p;
				juce::Rectangle<float> pathBounds;
			};


		};
		std::unique_ptr<Grid> grid;


		void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
		void parameterChanged(int idx) override;
		Component* gridSibling = nullptr;
		Shape icon;
		juce::String funcName;
	};

} // namespace bdsp