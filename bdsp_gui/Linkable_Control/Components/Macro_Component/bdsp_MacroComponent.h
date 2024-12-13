#pragma once



namespace bdsp
{

	class MacroControllerObject : public LinkableControl, public juce::RangedAudioParameter::Listener
	{
	public:
		MacroControllerObject(juce::RangedAudioParameter* macroParameter);
		juce::RangedAudioParameter* getMacroParameter();


		float update() override;

	private:
		juce::RangedAudioParameter* macroParam = nullptr;


		// Inherited via Listener
		void parameterValueChanged(int parameterIndex, float newValue) override;

		void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;


	};

	class MacroComponent : public juce::Component, public ComponentCore, public LinkableControlComponent, public juce::ValueTree::Listener, public GUI_Universals::Listener
	{
	public:
		MacroComponent(GUI_Universals* universalsToUse, int macroIdx, juce::ValueTree* nameValueLocation);
		~MacroComponent();
		void colorSet() override;

		void resized() override;

		void parentHierarchyChanged() override;


		CircleSlider* getSlider();


		void setName(const juce::String& newName) override;


	private:





		float prevVal = 0;

		int macroIndex = 0;


		void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& propertyID) override;

		CircleSlider slider;




		// Inherited via LinkableControlComponent
		juce::Array<juce::WeakReference<HoverableComponent>> getComponentsThatShareHover() override;

		juce::ValueTree* nameValueLoc;


		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;

	};


	class MacroSection : public TexturedContainerComponent
	{
	public:

		MacroSection(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation);
		virtual ~MacroSection() = default;

		MacroComponent* getMacro(int idx);



		void resized() override;
		void paint(juce::Graphics& g) override;

		void setLayout(juce::Array<int>& newLayout);

		void setTitleFontIndex(int index)
		{
			for (auto m : macros)
			{
				m->getSlider()->mainLabel->getTitle().setFontIndex(index);
			}
		}


		bool shouldDrawDivider = true;
	private:
		juce::OwnedArray<MacroComponent> macros;

		juce::Array<int> layout;

		;

	};



}// namnepace bdsp
