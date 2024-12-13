#pragma once



namespace bdsp
{
	class RearrangeableTabsComponent : public TabsComponent
	{
	public:

		RearrangeableTabsComponent(GUI_Universals* universalsToUse, const juce::StringArray& tabNames);
		~RearrangeableTabsComponent() = default;

		void setVertical(bool shouldBeVertical) override;

		void setNames(const juce::StringArray& newNames) override;
		void setNumCompsPerTab(int num);

		void resized() override;
		//void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;

		juce::Rectangle<int> getRelativeRearrangeBounds();

		juce::Rectangle<int> getRelativeTabTitleBounds();


		void addComponent(juce::Component* newComp, int page);

	

		void setTabTitleHeightRatio(float newRatio);

		RearrangableComponentManagerBase* getRearrangeComp();


	protected:

		float tabTitleH = 0.5;
		
		int numPerTab;

		std::unique_ptr<RearrangableComponentManagerBase> rearrangeComp;

	};

} // namespace bdsp
