#pragma once



namespace bdsp
{
#define fracCenter 0.25
#define fracSideHRatio 3/8.0
#define fracBarExtra 1/6.0


#define divAlt {"Ms", "Straight", "Triplet", "Quint", "Sept"}



	class SyncFractionBase : public Component, public BPMComponent::Listener
	{
	public:
		SyncFractionBase(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRateFrac = false);
		~SyncFractionBase() noexcept override;


		void init();

		virtual void attach(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterIDBase);
		juce::String getBaseID();

		virtual float getTimeInMs(double bpm, bool isRate = false); // if rate gives cycle period in ms


		class Listener
		{
		public:
			Listener() = default;
			virtual ~Listener()
			{
				for (SyncFractionBase* f : fracsListeningTo)
				{
					f->removeListener(this);
				}
			}


			virtual void valueChanged(SyncFractionBase* sf) = 0;

			juce::Array<SyncFractionBase*> fracsListeningTo;

			JUCE_DECLARE_WEAK_REFERENCEABLE(Listener)


		};

		void addListener(Listener* listenerToAdd);
		void removeListener(Listener* listenerToRemove);


	protected:

		juce::Array<juce::WeakReference<Listener>> listeners;

		BaseSlider* fractionSlider, * msTimeSlider;
		juce::Component* fraction, * msTime;
		ChoiceComponentCore* division;


		// Inherited via BPM Listener
		void valueChanged() override;

		bool attached = false;

		bool isRate = false;

	private:

		juce::AudioParameterChoice* divisionParam;
		;
		BPMComponent* bpmComp;
	};





	template <class fracType = CircleSlider>
	class SyncFraction : public SyncFractionBase
	{
	public:


		SyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate = false);
		~SyncFraction();





		void resized() override;






		void setFracColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text, const NamedColorsIdentifier& textHighlight, const NamedColorsIdentifier& textEdit, const NamedColorsIdentifier& caret, const NamedColorsIdentifier& highlight);
		void setFracColors(const NamedColorsIdentifier& value, const NamedColorsIdentifier& accent, float opacity = 0.25f);








		void paint(juce::Graphics& g) override;


		void lookAndFeelChanged() override;







		fracType* getFraction();
		fracType* getMsSlider();



		ComboBox* getDivision();









        class DivLabel : public Label 
        {

		public:

			DivLabel(ComboBox* p, fracType* sib)
				:Label(p->universals)
			{
				parentBox = p;

				auto cast = dynamic_cast<RangedSlider*>(sib);

				if (cast != nullptr)
				{
					siblingSlider = cast->parent;
					ranged = true;
				}
				else
				{
					siblingSlider = dynamic_cast<BaseSlider*>(sib);
					ranged = false;
				}

				parentBox->addComponentListener((juce::ComponentListener*)this);
			}

			~DivLabel()
			{

			}


			void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override
			{
				auto y = (ranged ? siblingSlider->getParentComponent()->getY() : siblingSlider->getY()) + siblingSlider->mainLabel->getY();
				auto h = siblingSlider->mainLabel->getHeight();
				setBounds(parentBox->getX(), y, parentBox->getWidth(), h);
			}

		private:

			ComboBox* parentBox;
			BaseSlider* siblingSlider;
			bool ranged;
		};
		std::unique_ptr<DivLabel> divisionLabel;



		void setSpacing(float timeXPos, float timeWidth, float divXPos, float divWidth, juce::RectanglePlacement timePlacement = juce::RectanglePlacement::centred);
		void setSpacing(float border, float width, juce::RectanglePlacement placement, juce::RectanglePlacement timePlacement = juce::RectanglePlacement::centred);

	protected:


		std::unique_ptr<fracType> circleFrac, circleTime;



		std::unique_ptr<ComboBox> divisionCombo;

		float timeX = 0.1, timeW = 0.35, divX = 0.55, divW = 0.35;

		juce::RectanglePlacement timeRectanglePlacement;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncFraction)


	};


}// namnepace bdsp

