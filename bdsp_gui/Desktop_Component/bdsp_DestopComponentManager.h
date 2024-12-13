#pragma once




namespace bdsp
{
	class DesktopComponent;
	class DesktopComponentManager :public juce::Component
	{
	public:

		DesktopComponentManager();
		~DesktopComponentManager();

		void addComponent(DesktopComponent* componentToAdd);
		void removeComponent(DesktopComponent* componentToRemove);
		void hideAllComponents(bool excludeCompsThatDisreagardLossOfFocus = false);
		void hideAllComponentsExcept(const juce::Array<DesktopComponent*>& componentsToExclude);
		void paintOverChildren(juce::Graphics& g) override;

		std::function<void()> onHide;
		std::function<void(juce::Graphics&)> paintOverGUIFunction;

		juce::Rectangle<int> mainArea; // used for keeping sizing consistent

	private:

		juce::Array<DesktopComponent*> componentsToManage;


	};


}// namnepace bdsp
