#pragma once



namespace bdsp
{
	class NumberSlider : public BaseSlider
	{
	public:
		NumberSlider(GUI_Universals* universalsToUse, const juce::String& baseName);
		NumberSlider(juce::Component* parent, GUI_Universals* universalsToUse, const juce::String& baseName, bool setListen = true);
		~NumberSlider() override;

		void showTextEditor() override;



		void setListener(juce::Slider* sliderToListenTo);
		juce::Slider::Listener* getListener();

		void resized() override;


		void mouseEnter(const juce::MouseEvent& e)override;
		void mouseExit(const juce::MouseEvent& e)override;




		void clearLookAndFeels() override;
		void resetLookAndFeels(bdsp::GUI_Universals* universalsToUse) override;


		Label numbers;

	private:

		class listener : public juce::Slider::Listener
		{
		public:
			listener(Label* l);
			~listener() override;
			void sliderValueChanged(Slider* slider) override;


		private:
			Label* label;
		}listen;

		juce::LookAndFeel* numbersLNF = nullptr;
		juce::Slider* sliderBeingListenedTo = nullptr;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NumberSlider)
	};

}// namnepace bdsp
