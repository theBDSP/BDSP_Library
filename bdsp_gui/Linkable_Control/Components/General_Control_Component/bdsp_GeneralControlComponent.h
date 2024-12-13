#pragma once 



namespace bdsp
{
	class GeneralControlComponent : public Component, public LinkableControlComponent, public juce::ValueTree::Listener, public GUI_Universals::Listener
	{
	public:
		GeneralControlComponent(GUI_Universals* universalsToUse, int idx, juce::ValueTree* nameValueLocation, const juce::Identifier& nameID);
		virtual ~GeneralControlComponent();

		// Inherited via Listener
		void GUI_UniversalsChanged() override;
		void resized() override;

		void setName(const juce::String& newName) override;
		Component* getHintBackground();

		virtual Component* getVisualizer() = 0;

		CenteredEditableLabel* getTitleComponent();
	protected:
		;

		std::unique_ptr<CenteredEditableLabel> title;

		juce::ValueTree* nameValueLoc;
		juce::Identifier nameValueID;
		void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& propertyID) override;

		juce::Rectangle<float> titleRect;

		Component hintBackground;// provides space behind other comps to recieve mouse events to trigger hint signals


	};

} // namespace bdsp