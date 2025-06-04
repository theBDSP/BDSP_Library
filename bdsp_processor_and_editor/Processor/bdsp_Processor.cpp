#include "bdsp_Processor.h"
//#include "bdsp_gui/Linkable_Control/Components/Envelope_Follower_Component/bdsp_envelopeFollowerComponent.cpp" // needed for weird linker error
namespace bdsp
{



	//==============================================================================

	Processor::Processor(bool hasSidechain)
		:
#ifndef JucePlugin_PreferredChannelConfigurations
		AudioProcessor(generateBusesLayout(hasSidechain)),
#endif
		subBlock(subBlockBuffer),
		hiddenDummyProcessor(this)
	{

		envelopeSourceList.add(new dsp::SampleSource<float>("Input"));
		inputSource = envelopeSourceList.getLast();
		envelopeSourceList.add(new dsp::SampleSource<float>("Side-Chain"));
		sidechainSource = envelopeSourceList.getLast();
		sidechainExists = hasSidechain;
		suspendProcessing(true);



		// needs to be instantiated outside attribute class because of settings call
		fracLambda = [=](int denominator)
		{
			std::function<juce::String(float, int)> out = [=](float value, int)
			{
				return settings.get("Reduce Fractions", 1) ? reduceFraction(int(value * denominator), denominator) + " Note" : juce::String(int(value * denominator)) + juce::String("/") + juce::String(denominator) + " Note";
			};
			return out;
		};



		fracLambdaText = [=](int denominator)
		{
			std::function<float(const juce::String&)> out = [=](const juce::String& text)
			{
				auto formatted = text.retainCharacters("0123456789/.");
				if (formatted.contains("/"))
				{
					auto top = formatted.upToFirstOccurrenceOf("/", false, true);
					auto bot = formatted.fromFirstOccurrenceOf("/", false, true);
					return top.getFloatValue() / bot.getFloatValue();
				}
				else if (!formatted.contains("."))
				{
					return formatted.getFloatValue() / float(denominator);
				}
				else
				{
					return formatted.getFloatValue();
				}
			};
			return out;
		};

		fracLambda16 = fracLambda(16);



		fracLambda16Text = fracLambdaText(16);


		fracLambda32 = fracLambda(32);

		fracLambda32Text = fracLambdaText(32);

		std::function<juce::NormalisableRange<float>(int, int, std::function<float(float, float, float)>, std::function<float(float, float, float)>)> rangeFunc = [=](int denominator, int maxNotes, std::function<float(float, float, float)> from0To1Func = std::function<float(float, float, float)>(), std::function<float(float, float, float)> to0To1Func = std::function<float(float, float, float)>())
		{

			std::function<float(float, float, float)> snapFunc = [=](float start, float end, float v)
			{
				return juce::jlimit(start, end, round(v * denominator) / float(denominator));
			};
			auto range = juce::NormalisableRange<float>(1 / float(denominator), maxNotes / float(denominator), from0To1Func, to0To1Func, snapFunc);
			//auto range = juce::NormalisableRange<float>(1 / float(denominator), maxNotes / float(denominator), from0To1Func, to0To1Func);
			//range.interval = 1 / float(denominator);
			return range;
		};


		parameterAttributes.addFloatAttribute("Time Fraction 16", fracLambda16, fracLambda16Text, rangeFunc(16, 16, nullptr, nullptr));
		parameterAttributes.addFloatAttribute("Time Fraction 32", fracLambda32, fracLambda32Text, rangeFunc(32, 16, nullptr, nullptr));
		//================================================================================================================================================================================================

		std::function<float(float, float, float)> fromFunc = [=](float rangeStart, float rangeEnd, float value)
		{
			float start = log2f(rangeStart);
			float end = log2f(rangeEnd);
			return pow(2, end - (end - start) * value);
		};

		std::function<float(float, float, float)> toFunc = [=](float rangeStart, float rangeEnd, float value)
		{
			float start = log2f(rangeStart);
			float end = log2f(rangeEnd);
			float logVal = log2f(value);
			if (logVal == end)
			{
				return 0.0f;
			}
			return (end - logVal) / (end - start);
		};


		rateSnapValues.clear();
		for (auto i = BDSP_RATE_MIN; i <= BDSP_RATE_MAX; ++i)
		{
			float power = pow(2, i);
			float prop = juce::jmap((float)i, (float)BDSP_RATE_MIN, (float)BDSP_RATE_MAX, 1.0f, 0.0f);
			rateSnapValues.add(juce::Point<float>(prop, power));
			if (i > BDSP_RATE_MIN + 1)
			{
				rateSnapValues.add(juce::Point<float>(toFunc(powf(2, BDSP_RATE_MIN), powf(2, BDSP_RATE_MAX), power * 0.75), power * 0.75));
			}
		}


		std::function<float(float, float, float)> snapFunc = [=](float start, float end, float v)
		{
			int idx = 0;
			float minDiff = abs(v - rateSnapValues.getFirst().y);

			for (int i = 1; i < rateSnapValues.size(); ++i)
			{
				auto diff = abs(v - rateSnapValues[i].y);

				if (diff < minDiff)
				{
					minDiff = diff;
					idx = i;
					if (juce::approximatelyEqual(diff, 0.0f))
					{
						break;
					}
				}
			}

			return rateSnapValues[idx].y;
		};
		auto range = juce::NormalisableRange<float>(pow(2, BDSP_RATE_MIN), pow(2, BDSP_RATE_MAX), fromFunc, toFunc, snapFunc);


		auto denominator = BDSP_RATE_MIN < 0 ? quickPow2(-BDSP_RATE_MIN) : 1;
		std::function<juce::String(float, int)> rateLambda = [=](float value, int)
		{
			auto v = snapFunc(0, 0, value);
			return settings.get("Reduce Fractions", 1) ? reduceFraction(int(v * denominator), denominator, " / ", true) : juce::String(int(v * denominator)) + juce::String(" / ") + juce::String(denominator);
		};



		parameterAttributes.addFloatAttribute("Rate Fraction", rateLambda, fracLambdaText(1), range);


		//================================================================================================================================================================================================


		defaultAPH.setBpm(120);

#if BDSP_DEMO_VERSION
		Demo = std::make_unique<bdsp::dsp::DemoTimer<float>>();
#endif


	}

	Processor::~Processor()
	{

	}





	//==============================================================================
	void Processor::prepareToPlay(double sampleRate, int samplesPerBlock)
	{
		prevPlayingState = false;
		const juce::dsp::ProcessSpec spec = { sampleRate, (juce::uint32)samplesPerBlock, (juce::uint32)getMainBusNumOutputChannels() };
		smpRate = sampleRate;
		smpsPerBlock = samplesPerBlock;
		outputChannels = spec.numChannels;
		specSet = true;
#if BDSP_DEMO_VERSION

		Demo->prepare(spec);
		Demo->setTimeIntervals(30, 5);
		Demo->setMix(1.0f);
#endif

		subBlockCounter = 0;
		subBlockTarget = floor(sampleRate / BDSP_CONTROL_SAMPLE_RATE) - 1;

		subBlockBuffer.setSize(outputChannels, subBlockTarget + 1);
		prevSubBlockBuffer.setSize(outputChannels, subBlockTarget + 1);

		prevSubBlockBuffer.clear();

		parameterAttributes.updateSampleRateDependentAttributes(smpRate);

		for (auto c : dspComps)
		{
			c->prepare(spec);
		}

		subBlockSpec = std::make_unique<juce::dsp::ProcessSpec>();
		subBlockSpec->sampleRate = spec.sampleRate;
		subBlockSpec->maximumBlockSize = subBlockTarget + 1;
		subBlockSpec->numChannels = spec.numChannels;
		for (auto c : controlObjects)
		{
			auto Env = dynamic_cast<EnvelopeFollowerControllerObject<float>*>(c);
			if (Env != nullptr)
			{
				Env->prepare(*subBlockSpec.get());
			}
		}

		if (editor.get() != nullptr)
		{
			editor->sampleRate = spec.sampleRate;
			editor->GUIUniversals.freqRange.end = spec.sampleRate / 2;
		}

		suspendProcessing(false);
	}


#ifndef JucePlugin_PreferredChannelConfigurations

	juce::AudioProcessor::BusesProperties Processor::generateBusesLayout(bool hasSidechain)
	{
		auto out = juce::AudioProcessor::BusesProperties();
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		out.addBus(true, "Input", juce::AudioChannelSet::stereo(), true);
#endif
		out.addBus(false, "Output", juce::AudioChannelSet::stereo(), true);
#endif

		if (hasSidechain)
		{
			out.addBus(true, "SideChain", juce::AudioChannelSet::stereo(), true);
		}
		return out;
	}
#endif

	void Processor::checkPlayHead()
	{

		APH = getPlayHead();


		if (APH == nullptr)
		{
			BPMFollow->setNoHost();
			currentPlayPosition = 0;
			prevPlayingState = false;
		}
		else
		{
			currentAPHPosition = APH->getPosition();
			jassert(currentAPHPosition.hasValue());
			currentPlayPosition = currentAPHPosition->getTimeInSamples().orFallback(0);

			if (currentAPHPosition->getIsPlaying())
			{
				if (!prevPlayingState || abs(currentPlayPosition - prevPlayPosition) > smpsPerBlock) //just started playing or play position jumped since last block
				{

					for (auto l : LFOParamListeners)
					{
						float start = truncf(currentAPHPosition->getTimeInSeconds().orFallback(0.0f) * BDSP_CONTROL_SAMPLE_RATE * l->getProgressIncrement());
						l->setProgress(start);
					}
					for (auto l : SequencerParamListeners)
					{
						float start = truncf(currentAPHPosition->getTimeInSeconds().orFallback(0.0f) * BDSP_CONTROL_SAMPLE_RATE * l->getProgressIncrement());
						l->setProgress(start);
					}

				}
			}
			prevPlayingState = APH->getPosition()->getIsPlaying();
		}



		prevPlayPosition = currentPlayPosition;
	}


	double Processor::updateBPM()
	{


		if (currentAPHPosition.operator bool())
		{
			BPMFollow->updateHostValue(currentAPHPosition.hasValue() ? *currentAPHPosition : defaultAPH);
		}

		BPMFollow->updateUserValue(*BPMVal);

		double val = BPMFollow->getValue();
		wholeNote = 240 * getSampleRate() / val; // 4 * 60

		return val;
	}

	void Processor::updateControls()
	{
		subBlockCounter++;
		if (subBlockCounter > subBlockTarget)
		{
			subBlockCounter = 0;
			//for (auto s : envelopeSourceList.sources)
			//{
			//	s->pushBufferToListeners();
			//}	
			//for (auto s : visualizerSourceList.sources)
			//{
			//	s->pushBufferToListeners();
			//}
			for (auto c : sampleRateLinkableControls)
			{
				c->update();
			}
			for (auto p : controlParameters)
			{
				p->valueChanged();
			}
		}
	}

	void Processor::getParameterValues()
	{
		for (auto p : controlParameters)
		{
			p->loadInfluenceValues();
		}
	}

	void Processor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
	{
		juce::ScopedNoDenormals noDenormals;


		// In case we have more outputs than inputs, this code clears any output
// channels that didn't contain input data, (because these aren't
// guaranteed to be empty - they may contain garbage).
// This is here to avoid people getting screaming feedback
// when they first compile a plugin, but obviously you don't need to keep
// this code if your algorithm always overwrites all the output channels.

		for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		{
			buffer.clear(i, 0, buffer.getNumSamples());
		}
		checkPlayHead();


		updateBPM();

		getParameterValues();


		auto mainBuffer = getBusBuffer(buffer, true, 0);
		auto sidechainBuffer = getBusBuffer(buffer, true, 1);

		sidechainEnabled = !(sidechainBuffer.getNumSamples() < mainBuffer.getNumSamples() || sidechainBuffer.getNumChannels() < 1 || sidechainBuffer == mainBuffer);



		if (!sidechainEnabled)
		{
			sidechainBuffer.setSize(mainBuffer.getNumChannels(), mainBuffer.getNumSamples(), false, false, false);
			sidechainBuffer.clear();
		}



		if (sidechainExists)
		{
			sideChainSubBlockBuffer.setSize(sidechainBuffer.getNumChannels(), subBlockTarget + 1);
		}
		else
		{
			sideChainSubBlockBuffer.setSize(mainBuffer.getNumChannels(), subBlockTarget + 1);
			sideChainSubBlockBuffer.clear();
		}

		processBlockInit(buffer, midiMessages);

		//================================================================================================================================================================================================


		for (mainBufferSample = 0; mainBufferSample < buffer.getNumSamples(); mainBufferSample++)
		{
			if (subBlockCounter > subBlockTarget)
			{
				subBlockCounter = 0;
				for (auto c : sampleRateLinkableControls)
				{
					c->update();
				}
				for (auto p : controlParameters)
				{
					p->valueChanged();
				}

				processSubBlock();
				prevSubBlockBuffer.makeCopyOf(subBlockBuffer);
			}


			for (int c = 0; c < subBlockBuffer.getNumChannels(); ++c)
			{
				subBlockBuffer.setSample(c, subBlockCounter, mainBuffer.getSample(c, mainBufferSample));
				buffer.setSample(c, mainBufferSample, prevSubBlockBuffer.getSample(c, subBlockCounter));
			}

			for (int c = 0; c < sideChainSubBlockBuffer.getNumChannels(); ++c)
			{
				sideChainSubBlockBuffer.setSample(c, subBlockCounter, sidechainBuffer.getSample(c, mainBufferSample));
			}



			subBlockCounter++;
		}

	}








	//==============================================================================
	bool Processor::hasEditor() const
	{
		return true; // (change this to false if you choose to not supply an editor)
	}



	//==============================================================================
	void Processor::getStateInformation(juce::MemoryBlock& destData)
	{


		auto xml = Handler->createSaveXML(true, Handler->presetName.toString());
		xml->setAttribute("WindowWidth", settingsTree.getProperty("WindowWidth").operator double());

		copyXmlToBinary(*xml.get(), destData);


		//for debugging
		//xml->writeTo(propertiesFolder.getFolder().getChildFile("Data").getChildFile("test.xml"));

	}

	void Processor::setStateInformation(const void* data, int sizeInBytes)
	{
		std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

		if (xmlState.get() != nullptr)
		{
			Handler->loadPreset(xmlState, false);

			parameters->undoManager->clearUndoHistory();
			settingsTree.setProperty("WindowWidth", xmlState->getDoubleAttribute("WindowWidth", BDSP_DEFAULT_PLUGIN_WIDTH), nullptr);
			Handler->alteredState.setValue(xmlState->getChildByName(InfoXMLTag)->getBoolAttribute("PresetAltered", false));

		}
	}

	juce::Array<ControlParameter*>& Processor::getControlParameters()
	{
		return controlParameters;

	}




	double Processor::getControlPeriodMs()
	{
		return 1000 * subBlockTarget / smpRate;
	}

	void Processor::updateUIProperties(float newWidth)
	{
		settingsTree.setPropertyExcludingListener(editor.get(), "WindowWidth", newWidth, nullptr);
	}

	int Processor::getBaseLatency()
	{
		return prevSubBlockBuffer.getNumSamples();
	}

	void Processor::init()
	{
		createLinkableIDs();

		parameters = std::make_unique<juce::AudioProcessorValueTreeState>(*this, &undoManager, juce::Identifier("APVTS"), createParameterLayout());

		hiddenDummyProcessor.initAPVTS();

		if (onParameterCreationCompletion.operator bool())
		{
			onParameterCreationCompletion();
		}

		//================================================================================================================================================================================================

		settingsTree = juce::ValueTree("UISettings");

		if (!settingsTree.hasProperty("PresetName"))
		{
			settingsTree.setProperty("PresetName", "init", nullptr);
		}

		if (!settingsTree.hasProperty("PresetAltered"))
		{
			settingsTree.setProperty("PresetAltered", false, nullptr);
		}



		auto macroTree = settingsTree.getOrCreateChildWithName("MacroNames", nullptr);

		for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
		{
			if (!macroTree.hasProperty("Macro" + juce::String(i + 1) + "NameID"))
			{
				macroTree.setProperty("Macro" + juce::String(i + 1) + "NameID", "Macro" + juce::String(i + 1), nullptr);
			}
			controlObjects.add(new MacroControllerObject(parameters->getParameter(linkableControlIDs[i])));

		}
		onSettingsTreeCreation();

		for (int i = 0; i < BDSP_NUMBER_OF_LFOS; ++i)
		{
			controlObjects.add(new LFOControllerObject(&lookups));
			sampleRateLinkableControls.add(controlObjects.getLast());
			LFOParamListeners.add(dynamic_cast<LFOParameterListener*>(controlObjects.getLast()));
			controlVisListeners.add(new OpenGLControlValuesListener(1000, controlObjects.getLast()));

		}
		for (int i = 0; i < BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS; ++i)
		{
			controlObjects.add(new EnvelopeFollowerControllerObject<float>());
			sampleRateLinkableControls.add(controlObjects.getLast());
			controlVisListeners.add(new OpenGLControlValuesListener(2000, controlObjects.getLast()));
		}
		for (int i = 0; i < BDSP_NUMBER_OF_SEQUENCERS; ++i)
		{
			controlObjects.add(new SequencerControllerObject(&lookups));
			sampleRateLinkableControls.add(controlObjects.getLast());
			SequencerParamListeners.add(dynamic_cast<SequencerParameterListener*>(controlObjects.getLast()));
		}

		//================================================================================================================================================================================================

		for (auto p : getParameters())
		{
			auto c = dynamic_cast<ControlParameter*>(p);
			if (c != nullptr)
			{
				c->setAPVTS(parameters.get());
				c->setControlObjects(&controlObjects);
				controlParameters.add(c);
			}

		}





		//================================================================================================================================================================================================


		BPMVal = parameters->getRawParameterValue("BPMID");
		BPMFollow = std::make_unique<BPMFollower>(parameters->getParameter("BPMID"), parameters->getParameter("BPMLinkID"));


		setFactoryPresets();

		Handler = std::make_unique<bdsp::StateHandler>(this, parameters.get(), hiddenDummyProcessor.apvts.get(), &settingsTree, &propertiesFolder, controlParameters, hiddenDummyProcessor.orderedParams, factoryPresets);

		if (!settingsTree.getProperty("PresetAltered").operator bool())
		{
			parameters->state.addListener(Handler.get());
			settingsTree.addListener(Handler.get());
		}



		Handler->loadStartupPreset();

		//================================================================================================================================================================================================


		const std::function<void(int)> ScaleFunc = [&](int idx)
		{

			if (editor.get() != nullptr)
			{
				if (!editor->getBounds().isEmpty())
				{
					auto top = editor->getTopLevelGUIComponent();

					//auto ratio = abs((double)editor->getWidth() / editor->getHeight());
					auto w = top->mainArea.getWidth() * ((idx - 2) * 0.25 + 1) / ((editor->prevUIScaleFactor - 2) * 0.25 + 1);
					editor->setMainAreaSize(w, w / top->aspectRatio);
					editor->prevUIScaleFactor = idx;

				}
			}
		};

		settings.arr.add(new bdsp::Settings::Setting("UI Scale", ScaleFunc, juce::StringArray({ "50%", "75%", "100%", "125%", "150%" }), 2, "UI Settings"));

		if (!settingsTree.hasProperty("WindowWidth"))
		{
			settingsTree.setProperty("WindowWidth", BDSP_DEFAULT_PLUGIN_WIDTH, nullptr);
		}

		//================================================================================================================================================================================================
		juce::StringArray bindingDescriptions = { "Slider Reset Value (w/ Single Click)", "Slider Fine Control (w/ Single Click)", "Slider Text Entry (w/ Double Click)" };
		juce::Array<juce::ModifierKeys::Flags> defaultBindings = { juce::ModifierKeys::altModifier,juce::ModifierKeys::commandModifier,juce::ModifierKeys::shiftModifier };

		std::function<std::function<void(int)>(int)> FuncFunc = [=](int b)
		{
			std::function<void(int)> Func = [=](int idx)
			{
				if (editor.get() != nullptr)
				{
					editor->GUIUniversals.bindings.set(b, settings.mods[idx + (b == 1 ? 0 : 1)]);
					editor->GUIUniversals.bindingChanged(b);
				}
			};
			return Func;
		};
		juce::StringArray modStringsAlt = settings.modStrings;
		modStringsAlt.remove(0);

		auto modCopy = settings.mods;
		modCopy.remove(0);

		settings.arr.add(new bdsp::Settings::Setting(bindingDescriptions[0], FuncFunc(0), modStringsAlt, modCopy.indexOf(defaultBindings[0]), "Key Bindings", std::pair<bool, float>({ true,1 }), true));
		settings.arr.add(new bdsp::Settings::Setting(bindingDescriptions[1], FuncFunc(1), settings.modStrings, settings.mods.indexOf(defaultBindings[1]), "Key Bindings", std::pair<bool, float>({ true,1 }), true));
		settings.exclusionGroups.add(juce::Array<int>({ settings.arr.size() - 1,settings.arr.size() - 2 }));

		settings.arr.add(new bdsp::Settings::Setting(bindingDescriptions[2], FuncFunc(2), modStringsAlt, modCopy.indexOf(defaultBindings[2]), "Key Bindings", std::pair<bool, float>({ true,1 }), true));
		settings.exclusionGroups.add(juce::Array<int>({ settings.arr.size() - 1,settings.arr.size() - 3 })); // single click reset and double click text edit cannot be the same

	}

	void Processor::onSettingsTreeCreation()
	{
	}

	void Processor::createBPMParameter(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int versionHint)
	{
		const auto& BPMAttribute = parameterAttributes.getFloatAttribute("BPM");
		layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("BPMID", versionHint), "BPM", BPMAttribute.range, 120.0, juce::AudioParameterFloatAttributes().withStringFromValueFunction(BPMAttribute.valueToTextLambda).withLabel("BPM")));

		layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("BPMLinkID", versionHint), "BPM Link", true));
	}

	void bdsp::Processor::createSyncParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, int denominator, int minBeats, int maxBeats, bool ranged, bool createLockParameter, juce::Array<int> includedDivisions, int versionHint, int snapVersionHint)
	{
		createSyncParameters(layout, baseID, baseName, 500.0f, 1.0 / 16.0, 1, denominator, minBeats, maxBeats, ranged, createLockParameter, includedDivisions, versionHint, snapVersionHint);
	}

	void Processor::createSyncParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, float defaultMsTime, float defaultFrac, int defaultDivision, int denominator, int minBeats, int maxBeats, bool ranged, bool createLockParameter, juce::Array<int> includedDivisions, int versionHint, int snapVersionHint)
	{
		baseID = baseID.upToLastOccurrenceOf("ID", false, false);

		snapVersionHint = snapVersionHint == -1 ? versionHint : snapVersionHint;

		bool hasMS = includedDivisions.contains(0) || includedDivisions.isEmpty();

		auto att = parameterAttributes.getFloatAttribute("Time Fraction " + juce::String(denominator));
		att.range.start = (float)minBeats / denominator;
		att.range.end = (float)maxBeats / denominator;

		if (ranged)
		{
			if (hasMS)
			{
				layout.add(std::make_unique<bdsp::ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "MsTimeID", baseName + " ms Time", defaultMsTime, "ms Time", versionHint));
			}

			layout.add(std::make_unique<bdsp::ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "FractionID", baseName + " Tempo Fraction", defaultFrac, att, versionHint));
			if (createLockParameter)
			{
				layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(baseID + "FractionSnapID", snapVersionHint), baseID + " Fraction Snapping", true));

				auto prevFunc = onParameterCreationCompletion;
				onParameterCreationCompletion = [=]()
				{
					if (prevFunc.operator bool())
					{
						prevFunc();
					}
					auto frac = dynamic_cast<ControlParameter*>(parameters->getParameter(baseID + "FractionID"));
					auto lock = dynamic_cast<juce::AudioParameterBool*>(parameters->getParameter(baseID + "FractionSnapID"));
					frac->setModulationSnapParameter(lock);
				};
			}

		}
		else
		{
			if (hasMS)
			{
				const auto& msAttributes = parameterAttributes.getFloatAttribute("ms Time");
				layout.add(std::make_unique < juce::AudioParameterFloat>(juce::ParameterID(baseID + "MsTimeID", versionHint), baseID + " ms Time", msAttributes.range, defaultMsTime, juce::AudioParameterFloatAttributes().withStringFromValueFunction(msAttributes.valueToTextLambda).withValueFromStringFunction(msAttributes.textToValueLambda)));
			}

			layout.add(std::make_unique < juce::AudioParameterFloat>(juce::ParameterID(baseID + "FractionID", versionHint), baseID + " Tempo Fraction", att.range, defaultFrac, att.toJuceAttributes()));
		}


		juce::StringArray divisionNames({ "ms", "Straight", "Triplet", "Quintuplet", "Septuplet" });

		if (!includedDivisions.isEmpty())
		{

			for (int i = divisionNames.size(); i >= 0; --i)
			{
				if (!includedDivisions.contains(i))
				{
					divisionNames.remove(i);
				}
			}
		}

		layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(baseID + "DivisionID", versionHint), baseName + " Division", divisionNames, defaultDivision));
	}

	void bdsp::Processor::createSyncRateParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, int minBeats, int maxBeats, bool ranged, bool createLockParameter, juce::Array<int> includedDivisions, int versionHint, int snapVersionHint)
	{
		createSyncRateParameters(layout, baseID, baseName, 1.0f, 1.0f / 4.0f, 1, minBeats, maxBeats, ranged, createLockParameter, includedDivisions, versionHint, snapVersionHint);
	}

	void Processor::createSyncRateParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, float defaultHzRate, float defaultFrac, int defaultDivision, int minBeats, int maxBeats, bool ranged, bool createLockParameter, juce::Array<int> includedDivisions, int versionHint, int snapVersionHint)
	{
		baseID = baseID.upToLastOccurrenceOf("ID", false, false);

		snapVersionHint = snapVersionHint == -1 ? versionHint : snapVersionHint;

		bool hasHz = includedDivisions.contains(0) || includedDivisions.isEmpty();

		auto denominator = BDSP_RATE_MIN < 0 ? quickPow2(-BDSP_RATE_MIN) : 1;

		auto att = parameterAttributes.getFloatAttribute("Rate Fraction");
		att.range.start = (float)minBeats / denominator;
		att.range.end = (float)maxBeats / denominator;

		if (ranged)
		{
			if (hasHz)
			{
				layout.add(std::make_unique<bdsp::ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "HzRateID", baseName + " Hz Rate", defaultHzRate, "Hz Rate", versionHint));
			}
			layout.add(std::make_unique<bdsp::ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "FractionID", baseName + " Tempo Rate Fraction", defaultFrac, att, versionHint));
			if (createLockParameter)
			{
				layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(baseID + "FractionSnapID", snapVersionHint), baseID + " Fraction Snapping", true));
				auto prevFunc = onParameterCreationCompletion;
				onParameterCreationCompletion = [=]()
				{
					if (prevFunc.operator bool())
					{
						prevFunc();
					}
					auto frac = dynamic_cast<ControlParameter*>(parameters->getParameter(baseID + "FractionID"));
					auto lock = dynamic_cast<juce::AudioParameterBool*>(parameters->getParameter(baseID + "FractionSnapID"));
					frac->setModulationSnapParameter(lock);
				};
			}
		}
		else
		{
			if (hasHz)
			{
				const auto& hzAttributes = parameterAttributes.getFloatAttribute("Hz Rate");
				layout.add(std::make_unique < juce::AudioParameterFloat>(juce::ParameterID(baseID + "HzRateID", versionHint), baseID + " Hz Rate", hzAttributes.range, defaultHzRate, juce::AudioParameterFloatAttributes().withStringFromValueFunction(hzAttributes.valueToTextLambda).withValueFromStringFunction(hzAttributes.textToValueLambda)));
			}


			layout.add(std::make_unique < juce::AudioParameterFloat>(juce::ParameterID(baseID + "FractionID", versionHint), baseID + " Tempo Rate Fraction", att.range, defaultFrac, att.toJuceAttributes()));
		}


		juce::StringArray divisionNames({ "Hz", "Straight", "Triplet", "Quintuplet", "Septuplet" });

		if (!includedDivisions.isEmpty())
		{

			for (int i = divisionNames.size(); i >= 0; --i)
			{
				if (!includedDivisions.contains(i))
				{
					divisionNames.remove(i);
				}
			}
		}

		layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(baseID + "DivisionID", versionHint), baseName + " Division", divisionNames, defaultDivision));
	}



	void Processor::createControlParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::Array<int> versionHints)
	{
		if (versionHints.isEmpty())
		{
			versionHints.resize(BDSP_NUMBER_OF_LINKABLE_CONTROLS);
			versionHints.fill(1);
		}
		const auto& percentAttribute = parameterAttributes.getFloatAttribute("Percent");
		const auto& shapeAttribute = parameterAttributes.getFloatAttribute("LFO Shape");


		juce::StringArray envelopeSourceNames;
		for (const auto& src : envelopeSourceList)
		{
			envelopeSourceNames.add(src->getName());
		}

		juce::StringArray seqShapeNames;

		for (int i = 0; i < SequencerShapes::NUM; ++i)
		{
			switch (SequencerShapes(i))
			{
			case SequencerShapes::Empty:
				seqShapeNames.add("Empty");
				break;
			case SequencerShapes::SawDown:
				seqShapeNames.add("Saw Down");
				break;
			case SequencerShapes::AccDown:
				seqShapeNames.add("Accelerate Down");
				break;
			case SequencerShapes::DecDown:
				seqShapeNames.add("Decelerate Down");
				break;
			case SequencerShapes::SawUp:
				seqShapeNames.add("Saw Up");
				break;
			case SequencerShapes::AccUp:
				seqShapeNames.add("Accelerate Up");
				break;
			case SequencerShapes::DecUp:
				seqShapeNames.add("Decelerate Up");
				break;
			case SequencerShapes::SquareFull:
				seqShapeNames.add("Square Full");
				break;
			case SequencerShapes::SquareHalf:
				seqShapeNames.add("Square Half");
				break;
			case SequencerShapes::Triangle:
				seqShapeNames.add("Triangle");
				break;
			}
		}

		auto denominator = BDSP_RATE_MIN < 0 ? quickPow2(-BDSP_RATE_MIN) : 1;

		for (int i = 0; i < linkableControlIDs.size(); ++i)
		{
			auto baseID = linkableControlIDs[i].upToLastOccurrenceOf("ID", false, true);
			if (linkableControlIDs[i].startsWith("Macro"))
			{
				layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(linkableControlIDs[i], versionHints[i]), linkableControlNames[i], percentAttribute.range, 0, juce::AudioParameterFloatAttributes().withStringFromValueFunction(percentAttribute.valueToTextLambda).withValueFromStringFunction(percentAttribute.textToValueLambda)));
			}
			else if (linkableControlIDs[i].startsWith("LFO"))
			{
				createSyncRateParameters(layout, baseID + "Rate", linkableControlNames[i] + " Rate", 1, 16 * denominator, true, true, juce::Array<int>(), versionHints[i]);
				layout.add(std::make_unique <ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "ShapeID", linkableControlNames[i] + " Shape", 0, shapeAttribute, versionHints[i]));
				layout.add(std::make_unique <ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "SkewID", linkableControlNames[i] + " Skew", 0, "Center Zero Percent", versionHints[i]));
				layout.add(std::make_unique <ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "AmplitudeID", linkableControlNames[i] + " Amplitude", 1, percentAttribute, versionHints[i]));
				layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(baseID + "PolarityID", versionHints[i]), linkableControlNames[i] + " Polarity", true));

			}
			else if (linkableControlIDs[i].startsWith("EnvelopeFollower"))
			{
				layout.add(std::make_unique <ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "AttackID", linkableControlNames[i] + " Attack", 1, "ms Time", versionHints[i]));
				layout.add(std::make_unique <ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "DecayID", linkableControlNames[i] + " Decay", 1, "ms Time", versionHints[i]));
				layout.add(std::make_unique <ControlParameter>(&parameterAttributes, layout, linkableControlIDs, linkableControlNames, baseID + "BoostID", linkableControlNames[i] + " Boost", 1, "Envelope Follower Boost", versionHints[i]));

				layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(baseID + "SourceID", versionHints[i]), baseID + " Source", envelopeSourceNames, 0));
			}
			else if (linkableControlIDs[i].startsWith("Sequencer"))
			{
				createSyncRateParameters(layout, baseID + "Rate", linkableControlNames[i] + " Rate", 1, 1 / 16.0f, 0, 1, denominator, true, false, juce::Array<int>(1, 2), versionHints[i]);

				for (int j = 0; j < BDSP_SEQUENCER_STEPS; ++j)
				{
					layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(baseID + "Shape" + juce::String(j + 1) + "ID", versionHints[i]), baseID + " Shape " + juce::String(j + 1), seqShapeNames, 0));
					layout.add(std::make_unique<Parameter>(juce::ParameterID(baseID + "Amt" + juce::String(j + 1) + "ID", versionHints[i]), baseID + " Amt " + juce::String(j + 1), juce::NormalisableRange<float>(), 1, FloatParameterAttribute()));
				}
				layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID(baseID + "NumStepsID", versionHints[i]), baseID + " Num Steps", 1, BDSP_SEQUENCER_STEPS, BDSP_SEQUENCER_STEPS));
			}
		}


	}




	void Processor::createLinkableIDs()
	{
		linkableControlIDs.clear();
		linkableControlNames.clear();
		for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
		{
			linkableControlIDs.add("Macro" + juce::String(i + 1) + "ID");
			linkableControlNames.add("Macro " + juce::String(i + 1));
		}
		for (int i = 0; i < BDSP_NUMBER_OF_LFOS; ++i)
		{
			linkableControlIDs.add("LFO" + juce::String(i + 1) + "ID");
			linkableControlNames.add("LFO " + juce::String(i + 1));
		}
		for (int i = 0; i < BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS; ++i)
		{
			linkableControlIDs.add("EnvelopeFollower" + juce::String(i + 1) + "ID");
			linkableControlNames.add("Envelope Follower " + juce::String(i + 1));
		}
		for (int i = 0; i < BDSP_NUMBER_OF_SEQUENCERS; ++i)
		{
			linkableControlIDs.add("Sequencer" + juce::String(i + 1) + "ID");
			linkableControlNames.add("Sequencer " + juce::String(i + 1));
		}
	}








	const juce::String Processor::getName() const
	{
		return JucePlugin_Name;
	}

	bool Processor::acceptsMidi() const
	{
#if JucePlugin_WantsMidiInput
		return true;
#else
		return false;
#endif
	}

	bool Processor::producesMidi() const
	{
#if JucePlugin_ProducesMidiOutput
		return true;
#else
		return false;
#endif
	}

	bool Processor::isMidiEffect() const
	{
#if JucePlugin_IsMidiEffect
		return true;
#else
		return false;
#endif
	}

	double Processor::getTailLengthSeconds() const
	{
		return 0.0;
	}

	void Processor::releaseResources()
	{
	}

	int Processor::getNumPrograms()
	{
		return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
		// so this should be at least 1, even if you're not really implementing programs.
	}

	int Processor::getCurrentProgram()
	{
		return 0;
	}

	void Processor::setCurrentProgram(int index)
	{
	}

	const juce::String Processor::getProgramName(int index)
	{
		return {};
	}

	void Processor::changeProgramName(int index, const juce::String& newName)
	{
	}






	}// namnepace bdsp
