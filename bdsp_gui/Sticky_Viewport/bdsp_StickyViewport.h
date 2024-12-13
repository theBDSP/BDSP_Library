#pragma once



namespace bdsp
{

	class StickyViewport : public juce::Component, public ComponentCore
	{
	public:
		StickyViewport(GUI_Universals* universalsToUse, bool hasSeperator = true);
		~StickyViewport() override;

		void addSection(const juce::String& title);
		void addSections(const juce::StringArray& newTitles);
		void addAndMakeVisibleToSection(juce::Component& childToAdd, int viewportIndex, int zOrder = -1);
		void addAndMakeVisibleToSection(juce::Component* childToAdd, int viewportIndex, int zOrder = -1);

		void resized() override;

		int getViewportIndex(const juce::String& title);


		void paintOverChildren(juce::Graphics& g) override;

		void lookAndFeelChanged() override;

		void clear();

		double titleHRatio = 0.1;
		double titleH;
		double border;

		NamedColorsIdentifier titleColor;
	private:

		juce::OwnedArray<juce::Component> universalsComponents;
		juce::StringArray titles;

		Viewport vp;
		juce::Component viewComp;

		;

		bool seperator;
	};

} // namespace bdsp