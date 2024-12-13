#pragma once




namespace bdsp
{
	class RangedSlider;
	class RangedSliderInfluence : public CircleSlider, public LinkableControlComponent::colorListener
	{
	public:
		RangedSliderInfluence(RangedSlider* p, int idx);
		~RangedSliderInfluence();

		bool isLinked();

		void setColor(NamedColorsIdentifier col);
		NamedColorsIdentifier getColor();

		void resized() override;
		void paint(juce::Graphics& g) override;



		void createLink();
		void removeLink();

		int getIndex();



		//==================================================================================================================

		LinkableControlComponent* getControlComp();
		NamedColorsIdentifier getLinkedControlColor();



	private:


		RangedSlider* parent = nullptr;


		int index;

		bool linked = false;

		NamedColorsIdentifier c;


		juce::String ID;
		juce::Identifier controlCompID;
		LinkableControlComponent* controlComp = nullptr;



		// Inherited via colorListener
		void colorChanged(LinkableControlComponent* controlThatChanged) override;

	};







}// namnepace bdsp