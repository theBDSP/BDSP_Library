#pragma once



namespace bdsp
{


	template<typename SampleType>
	class EnvelopeFollowerParameterListener : public juce::AudioParameterChoice::Listener
	{
	public:
		EnvelopeFollowerParameterListener()
		{

			outputBuffer.setSize(1, 1);
			outputBuffer.setSample(0, 0, 0);

		}
		virtual ~EnvelopeFollowerParameterListener()
		{

		}
		void setParameters(ControlParameter* attackParam, ControlParameter* decayParam, ControlParameter* boostParam, juce::AudioParameterChoice* sourceParam, dsp::SampleSourceList<SampleType>* sourceList)
		{


			sourceParam->addListener(this);
			sources = sourceList;

			sourceIndex = sourceParam->getIndex();

			currentSource = sources->getUnchecked(sourceIndex);


			sourceParameter = sourceParam;

			attackParameter = attackParam;
			decayParameter = decayParam;
			boostParameter = boostParam;

			parametersSet = true;
		}

		virtual void prepare(const juce::dsp::ProcessSpec& spec)
		{
			filter.prepare({ spec.sampleRate, spec.maximumBlockSize, 1 });
			outputBuffer.setSize(1, spec.maximumBlockSize);

		}


		void getEnvelopeValue()
		{
			if (parametersSet)
			{

				attack = attackParameter->getActualValue();
				decay = decayParameter->getActualValue();
				boost = boostParameter->getActualValue();
			}

			filter.setAttackTime(attack);
			filter.setReleaseTime(decay);

			auto buffer = currentSource.getBuffer(1, outputBuffer.getNumSamples());
			juce::AudioBuffer<SampleType> summedBuffer(1, buffer.getNumSamples());
			summedBuffer.clear();

			for (int i = 0; i < buffer.getNumChannels(); ++i)
			{
				summedBuffer.addFrom(0, 0, buffer.getReadPointer(i), buffer.getNumSamples());
			}

			summedBuffer.applyGain(boost / buffer.getNumChannels());

			juce::dsp::AudioBlock<SampleType> summedInput(summedBuffer);
			outputBuffer.clear();
			auto outBlock = juce::dsp::AudioBlock<SampleType>(outputBuffer);
			juce::dsp::ProcessContextNonReplacing<SampleType> context(summedInput, outBlock);

			filter.process(context);

			for (int i = 0; i < outputBuffer.getNumSamples(); i++)
			{
				outputBuffer.setSample(0, i, juce::jlimit(SampleType(0), SampleType(1), outputBuffer.getSample(0, i)));
			}


		} // called once per control rate sample


		float getV()
		{
			return v;
		}
		float getAttack()
		{
			return attack;
		}
		float getDecay()
		{
			return decay;
		}
		float getBoost()
		{
			return boost;
		}
		int getSourceIndex()
		{
			return sourceIndex;
		}




	protected:
		float v = 0;// cannot be named value b/c its ambiguous w. variable in linkable control
		float attack = 1;
		float decay = 1;
		float boost = 1;
		int sourceIndex = 0;
		dsp::SampleSourceList<SampleType>* sources = nullptr;
		dsp::SampleSourceWeakReference<SampleType> currentSource;

		// Inherited via Choice Listener
		void parameterValueChanged(int parameterIndex, float newValue) override
		{
			sourceIndex = floor(newValue * (sources->size() - 1));
			currentSource = sources->getUnchecked(sourceIndex);
		}
		void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
		{
		}

		juce::dsp::BallisticsFilter<SampleType> filter;

		ControlParameter* attackParameter = nullptr, * decayParameter = nullptr, * boostParameter = nullptr;
		juce::AudioBuffer<SampleType> outputBuffer;

		juce::AudioParameterChoice* sourceParameter = nullptr;


		bool parametersSet = false;
	};

	template<typename SampleType>
	class EnvelopeFollowerControllerObject : public LinkableControl, public EnvelopeFollowerParameterListener<SampleType>
	{
	public:
		EnvelopeFollowerControllerObject()
		{
		}
		~EnvelopeFollowerControllerObject()
		{
		}


		//================================================================================================================================================================================================
		float update() override
		{
			EnvelopeFollowerParameterListener<SampleType>::getEnvelopeValue();
			EnvelopeFollowerParameterListener<SampleType>::v = EnvelopeFollowerParameterListener<SampleType>::outputBuffer.getSample(0, 0);
			value = EnvelopeFollowerParameterListener<SampleType>::v;
			pushToListeners();
			return value;
		}


	};


	template<typename SampleType>
	class EnvelopeFollowerVisualizer;

	template<typename SampleType>
	class EnvelopeFollowerComponent : public GeneralControlComponent
	{
	public:
		EnvelopeFollowerComponent(GUI_Universals* universalsToUse, dsp::SampleSourceList<SampleType>* list, juce::ValueTree* nameValueLocation, int envIDX)
			:GeneralControlComponent(universalsToUse, BDSP_NUMBER_OF_MACROS + BDSP_NUMBER_OF_LFOS + envIDX, nameValueLocation, juce::Identifier("Env" + juce::String(envIDX + 1) + "NameID"))
		{


			displayName = "ENV " + juce::String(envIDX + 1);

			//defaultName = "Env " + juce::String(envIDX + 1);;


			ID = "EnvelopeFollower" + juce::String(envIDX + 1) + "ID";

			auto baseID = ID.toString().upToLastOccurrenceOf("ID", false, true);



			sourceList = list;




			//================================================================================================================================================================================================

			setInterceptsMouseClicks(true, true);


			attack = std::make_unique<RangedContainerSlider>(universals, baseID + " Attack");
			decay = std::make_unique<RangedContainerSlider>(universals, baseID + " Decay");
			boost = std::make_unique<RangedContainerSlider>(universals, baseID + " Range");


			attack->slider.setCornerCurves(CornerCurves(0));
			decay->slider.setCornerCurves(CornerCurves(0));
			boost->slider.setCornerCurves(CornerCurves(CornerCurves::topRight | CornerCurves::bottomRight));




			attack->slider.setHintBarText("How quickly " + defaultName + " responds to an amplitude change in the source");
			decay->slider.setHintBarText("How quickly " + defaultName + " decays to 0 after an amplitude change in the source");
			boost->slider.setHintBarText("Gain applied to " + defaultName + "'s output");

			source = std::make_unique<ComboBox>(dynamic_cast<juce::AudioParameterChoice*>(universalsToUse->APVTS->getParameter(baseID + "SourceID")), universals, juce::StringArray(), std::function<void(int)>());
			source->setHintBarText(defaultName + "'s audio source");
			source->setCornerCurves(CornerCurves(CornerCurves::topLeft | CornerCurves::bottomLeft));

			attackLabel = std::make_unique<Label>(universals);
			decayLabel = std::make_unique<Label>(universals);
			boostLabel = std::make_unique<Label>(universals);
			sourceLabel = std::make_unique<Label>(universals);

			attackLabel->setText("Attack", juce::sendNotification);
			decayLabel->setText("Decay", juce::sendNotification);
			boostLabel->setText("Boost", juce::sendNotification);
			sourceLabel->setText("Source", juce::sendNotification);

			attackLabel->addHoverPartner(attack.get());
			decayLabel->addHoverPartner(decay.get());
			boostLabel->addHoverPartner(boost.get());
			sourceLabel->addHoverPartner(source.get());

			addAndMakeVisible(attack.get());
			addAndMakeVisible(decay.get());
			addAndMakeVisible(boost.get());
			addAndMakeVisible(source.get());

			addAndMakeVisible(attackLabel.get());
			addAndMakeVisible(decayLabel.get());
			addAndMakeVisible(boostLabel.get());
			addAndMakeVisible(sourceLabel.get());



			EnvIndex = envIDX;



			addAndMakeVisible(dragComp);
			addAndMakeVisible(transferComp);


			addAndMakeVisible(hintBackground);
			hintBackground.toBack();

		}
		virtual ~EnvelopeFollowerComponent()
		{
		}
		void colorSet() override
		{
			attack->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, NamedColorsIdentifier(), NamedColorsIdentifier());
			decay->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, NamedColorsIdentifier(), NamedColorsIdentifier());
			boost->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, NamedColorsIdentifier(), NamedColorsIdentifier());

			//source->setColors(BDSP_COLOR_KNOB, BDSP_COLOR_BLACK, BDSP_COLOR_KNOB, BDSP_COLOR_KNOB, BDSP_COLOR_BLACK, BDSP_COLOR_MID, BDSP_COLOR_WHITE);
			source->setColorSchemeClassic(BDSP_COLOR_KNOB, NamedColorsIdentifier(), NamedColorsIdentifier(BDSP_COLOR_BLACK), c);

			attackLabel->setColour(juce::Label::textColourId, getColor(c));
			decayLabel->setColour(juce::Label::textColourId, getColor(c));
			boostLabel->setColour(juce::Label::textColourId, getColor(c));
			sourceLabel->setColour(juce::Label::textColourId, getColor(c));

			if (visualizer != nullptr)
			{
				visualizer->setColor(c, BDSP_COLOR_LIGHT, c.withMultipliedAlpha(0.25f), c.withMultipliedAlpha(0.25f));
			}

		}

		void setControlObject(LinkableControl* newControlObject) override
		{
			LinkableControlComponent::setControlObject(newControlObject);

			attach();
			auto EnvObject = dynamic_cast<EnvelopeFollowerControllerObject<SampleType>*>(newControlObject);





			EnvObject->setParameters(attack->getControlParamter(), decay->getControlParamter(), boost->getControlParamter(), source->getParameter(), sourceList);
			visualizer = std::make_unique<EnvelopeFollowerVisualizer<SampleType>>(universals, this);
			//source->parameter->sendValueChangedMessageToListeners(source->parameter->get());
			visualizer->setColor(c, BDSP_COLOR_LIGHT, c.withMultipliedAlpha(0.25f), c.withMultipliedAlpha(0.25f));
			addAndMakeVisible(visualizer.get());
		}

		void paint(juce::Graphics& g) override
		{
		}
		void paintOverChildren(juce::Graphics& g) override
		{
			auto w = juce::jmax(universals->dividerSize, 1.0f);



			g.setColour(getColor(BDSP_COLOR_DARK));
			g.drawRoundedRectangle(visualizer->getBounds().toFloat(), universals->roundedRectangleCurve, w);
			g.drawRoundedRectangle(juce::Rectangle<float>().leftTopRightBottom(source->getX(), source->getY(), visualizer->getRight(), source->getBottom()), universals->roundedRectangleCurve, w);

			if (w <= 1)
			{
				g.drawVerticalLine((attack->getRight() + decay->getX()) / 2, attack->getY(), attack->getBottom());
				g.drawVerticalLine((decay->getRight() + boost->getX()) / 2, decay->getY(), decay->getBottom());

			}
			else
			{
				g.fillRect(juce::Rectangle<float>((attack->getRight() + decay->getX()) / 2 - w / 2, attack->getY(), w, attack->getHeight()));
				g.fillRect(juce::Rectangle<float>((decay->getRight() + boost->getX()) / 2 - w / 2, decay->getY(), w, decay->getHeight()));
			}
		}


		void attach()
		{
			auto baseID = ID.toString().upToLastOccurrenceOf("ID", false, true);
			attack->slider.attach(*universals->APVTS, baseID + "AttackID");
			decay->slider.attach(*universals->APVTS, baseID + "DecayID");
			boost->slider.attach(*universals->APVTS, baseID + "BoostID");
			//source->attach(source->getParameter(),universals->undoManager);


			boost->slider.addCatchValues({ 1 }, false);

		}
		void resized() override
		{
			GeneralControlComponent::resized();




			auto gap = titleRect.getBottom() - title->getBottom();

			auto reduced = getLocalBounds().toFloat().withTop(titleRect.getBottom());


			visualizer->setBounds(shrinkRectangleToInt(reduced.getProportion(juce::Rectangle<float>(0, 0, 1, universals->controlUniversals.visualizerHieghtRatio)).withTrimmedBottom(gap / 2)));

			//reduced = reduced.getProportion(juce::Rectangle<float>(0, 0, 0.5, 1));


			auto y = visualizer->getBottom() + gap;
			auto h = (getHeight() - y) * universals->controlUniversals.containerSliderHeightRatio;

			//polarity->setBounds(shrinkRectangleToInt(buttonRect.getProportion(juce::Rectangle<float>(0, 0, 1, 0.5))));


			auto x = visualizer->getX();
			auto b = 0;

			auto sourceW = getWidth() * 0.25;
			auto othersW = (getWidth() - x - sourceW - 4 * b) / 3;

			source->setBounds(x, y, sourceW, h);
			attack->setBounds(source->getRight() + b, y, othersW, h);
			decay->setBounds(attack->getRight() + b, y, othersW, h);
			boost->setBounds(decay->getRight() + b, y, othersW, h);

			sourceLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(source->getX(), source->getBottom(), source->getRight(), getHeight()).reduced(0, universals->rectThicc));
			attackLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(attack->getX(), attack->getBottom(), attack->getRight(), getHeight()).reduced(0, universals->rectThicc));
			decayLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(decay->getX(), decay->getBottom(), decay->getRight(), getHeight()).reduced(0, universals->rectThicc));
			boostLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(boost->getX(), boost->getBottom(), boost->getRight(), getHeight()).reduced(0, universals->rectThicc));

		}

		void setSourceEnablement(int idx, bool shouldBeEnabled)
		{
			source->getListComponent()->List[idx]->setEnabled(shouldBeEnabled);
		}
		void setSourceEnablement(const juce::String& name, bool shouldBeEnabled)
		{
			for (int i = 0; i < source->getParameter()->choices.size(); i++)
			{
				if (source->getParameter()->choices[i].compare(name) == 0)
				{
					setSourceEnablement(i, shouldBeEnabled);
					break;
				}
			}
		}

		ComboBox* getSourceComponent()
		{
			return source.get();
		}

		Component* getVisualizer() override
		{
			return visualizer.get();
		}

		ComboBox* getSource()
		{
			return source.get();
		}

	private:


		std::unique_ptr<RangedContainerSlider> attack, decay, boost;
		std::unique_ptr<ComboBox> source;
		dsp::SampleSourceList<SampleType>* sourceList;

		std::unique_ptr<Label> attackLabel, decayLabel, boostLabel, sourceLabel;

		int EnvIndex = 0;

		BPMComponent* BPM;

		std::unique_ptr<EnvelopeFollowerVisualizer<SampleType>> visualizer;



		// Inherited via LinkableControlComponent
		juce::Array<juce::WeakReference<HoverableComponent>> getComponentsThatShareHover() override
		{
			juce::Array<juce::WeakReference<HoverableComponent>> out({ this, attack.get(), decay.get(), boost.get(), source.get(), visualizer.get(), &dragComp });

			return out;
		}






		JUCE_LEAK_DETECTOR(EnvelopeFollowerComponent)

	};


	template <typename SampleType>
	class EnvelopeFollowerSection : public TexturedContainerComponent, public juce::Timer
	{
	public:

		EnvelopeFollowerSection(GUI_Universals* universalsToUse, dsp::SampleSourceList<SampleType>* list, const NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation);

		virtual ~EnvelopeFollowerSection()
		{
			stopTimer();
			followers.clear();
		}
		EnvelopeFollowerComponent<SampleType>* getFollower(int idx)
		{
			return followers[idx];
		}

		void setVertical(bool shouldBeVertical)
		{
			isVertical = shouldBeVertical;
			resized();
		}

		void resized() override
		{

			auto w = 1.0 / followers.size();
			for (int i = 0; i < followers.size(); i++)
			{
				juce::Rectangle<float> propRect;
				if (isVertical)
				{
					propRect = juce::Rectangle<float>(0, i * w, 1, w);
				}
				else
				{
					propRect = juce::Rectangle<float>(i * w, 0, w, 1);
				}
				followers[i]->setBounds(shrinkRectangleToInt(getLocalBounds().getProportion(propRect).getProportion(juce::Rectangle<float>(0.05, 0.05, 0.9, 0.9))));
			}
		}
		void paint(juce::Graphics& g) override
		{
			TexturedContainerComponent::paint(g);

			/*	for (int i = 0; i < followers.size() - 1; i++)
			{
			auto x = (followers[i]->getRight() + followers[i + 1]->getX()) / 2;
			drawDivider(g, juce::Line<float>(x, universals->dividerBorder, x, getHeight() - universals->dividerBorder), universals->colors.getColor(BDSP_COLOR_LIGHT), universals->dividerSize);
			}*/
		}

		void setTitleFontIndex(int index)
		{
			for (auto e : followers)
			{
				e->getTitleComponent()->setFontIndex(index);
			}
		}
		void visibilityChanged() override
		{
			for (auto e : followers)
			{
				e->setVisible(isVisible());
			}
		}




	private:
		juce::OwnedArray<EnvelopeFollowerComponent<SampleType>> followers;

		bool isVertical = false;

		// Inherited via Timer
		void timerCallback() override;

	};













}// namnepace bdsp
