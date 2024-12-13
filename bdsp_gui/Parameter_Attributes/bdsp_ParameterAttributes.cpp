#include "bdsp_ParameterAttributes.h"
namespace bdsp
{

	GenericParameterAttributes::GenericParameterAttributes()
	{
		auto bypassLambdaText = [](juce::String s)
		{
			return s.containsIgnoreCase("Bypassed");
		};


		auto bypassLambda = [=](bool value, int maximumStringLength)
		{
			return value ? "Bypassed" : "Not-Bypassed";
		};


		boolAttributes.add(new BoolParameterAttribute("Bypass", bypassLambda, bypassLambdaText));






		//================================================================================================================================================================================================

		auto simplifyNumber = [=](float value, int numDecimalPlaces, bool forceDecimal)
		{
			auto roundingFactor = pow(10, numDecimalPlaces);
			auto out = juce::String(round(value * roundingFactor) / roundingFactor);
			auto endPos = out.indexOfChar('.');
			if (endPos != -1)
			{
				if (numDecimalPlaces == 0)
				{
					out = out.substring(0, endPos);
				}
				else
				{
					out = out.substring(0, endPos + numDecimalPlaces + 1);
				}
			}
			else if (forceDecimal && numDecimalPlaces > 0)
			{
				juce::String forcedDecimal = ".0";
				for (int i = 1; i < numDecimalPlaces; ++i)
				{
					forcedDecimal.append("0", 1);
				}
				out.append(forcedDecimal, forcedDecimal.length());
			}

			return out;

		};


		auto basicLambdaText = [](juce::String s)
		{
			return s.retainCharacters("0123456789.-").getFloatValue();
		};


		auto basicLambda = [=](float value, int maximumStringLength)
		{
			auto out = simplifyNumber(value, 1, false);

			return out;
		};


		floatAttributes.add(new FloatParameterAttribute("Basic", basicLambda, basicLambdaText, juce::NormalisableRange<float>(0.0f, 1.0f)));
		//================================================================================================================================================================================================


		auto percentLambdaText = [](juce::String s)
		{
			return s.retainCharacters("0123456789.-").getFloatValue() / 100;
		};


		auto percentLambda = [=](float value, int maximumStringLength)
		{
			auto out = simplifyNumber(value * 100.0, 0, false);

			out.append("%", 1);

			return out;
		};


		floatAttributes.add(new FloatParameterAttribute("Percent", percentLambda, percentLambdaText, juce::NormalisableRange<float>(0.0f, 1.0f)));

		auto envRange = juce::NormalisableRange<float>(0.0f, 10.0f);
		envRange.skew = 0.5;
		floatAttributes.add(new FloatParameterAttribute("Envelope Follower Boost", percentLambda, percentLambdaText, envRange));


		auto centerZeroRange = juce::NormalisableRange<float>(-1.0f, 1.0f);
		floatAttributes.add(new FloatParameterAttribute("Center Zero Percent", percentLambda, percentLambdaText, centerZeroRange));

		//================================================================================================================================================================================================


		auto gainLambdaText = [=](juce::String s)
		{
			if (s.containsIgnoreCase("-inf"))
			{
				return 0.0f;
			}
			else
			{
				return juce::Decibels::decibelsToGain(percentLambdaText(s) * 100);
			}
		};


		auto gainLambda = [=](float value, int maximumStringLength)
		{
			return juce::Decibels::toString(juce::Decibels::gainToDecibels(value));
		};

		floatAttributes.add(new FloatParameterAttribute("Gain", gainLambda, gainLambdaText, juce::NormalisableRange<float>(0.0f, 1.0f)));


		auto EQgainLambdaText = [=](juce::String s)
		{
			if (s.containsIgnoreCase("-inf"))
			{
				return 0.0f;
			}
			else
			{
				return percentLambdaText(s) * 100;
			}
		};


		auto EQgainLambda = [=](float value, int maximumStringLength)
		{
			return juce::Decibels::toString(value);
		};

		floatAttributes.add(new FloatParameterAttribute("EQ Gain", EQgainLambda, EQgainLambdaText, juce::NormalisableRange<float>(-24.0f, 24.0f)));



		//================================================================================================================================================================================================


		auto timeLambda = [](float value, int maximumStringLength)
		{
			auto out = juce::String(round(value));
			auto endPos = out.indexOfChar('.');
			if (endPos != -1)
			{
				out = out.substring(0, endPos);
			}
			out.append(" ms", 3);

			return out;

		};

		auto timeLambdaText = [](juce::String s)
		{
			return s.retainCharacters("0123456789.-").getFloatValue();
		};


		auto timeRange = juce::NormalisableRange <float>(1.0f, 5000.0f);
		timeRange.skew = 0.4;

		floatAttributes.add(new FloatParameterAttribute("ms Time", timeLambda, timeLambdaText, timeRange)); // range can change based of needs

		//================================================================================================================================================================================================





		auto panLambda = [=](float value, int maximumStringLength)
		{
			auto out = simplifyNumber(round(abs(value * 100.0)), 0, false);

			out.append("%", 1);





			juce::String dir;
			if (value < 0)
			{
				dir = " L";
			}

			else if (value > 0)
			{
				dir = " R";
			}

			else
			{
				dir = "";
				out = "C";
			}


			return out + dir;
		};

		auto panLambdaText = [](const juce::String& s)
		{
			auto clean = s.retainCharacters("-0123456789.lLrRcC");
			auto num = clean.retainCharacters("-0123456789.").getFloatValue() / 100.0;
			float out = num;
			if (clean.containsAnyOf("lL"))
			{
				out = -abs(num);
			}
			else if (clean.containsAnyOf("rR"))
			{
				out = abs(num);
			}

			else if (clean.containsAnyOf("cC"))
			{
				out = 0;
			}

			return out;
		};

		floatAttributes.add(new FloatParameterAttribute("Pan", panLambda, panLambdaText, juce::NormalisableRange<float>(-1.0f, 1.0f)));

		//================================================================================================================================================================================================



		auto freqLambda = [=](float value, int maximumStringLength)
		{
			juce::String out;

			if (value < 1000)
			{
				out = juce::String(value, 2);
				if (out.compare("1000.00") == 0)
				{
					return juce::String("1 kHz");
				}

				out.append(" Hz", 3);
			}
			else
			{
				out = juce::String(value / 1000, 2);

				out.append(" kHz", 4);
			}


			return out;
		};

		//float noteVals[12] = {
		//	261.63f, //C
		//	277.18f, //C# || Db
		//	293.66f, //D
		//	311.13f, //D# || Eb
		//	329.63f, //E || Fb
		//	349.23f, //E# || F
		//	369.99f, //F# || Gb
		//	392.0f, //G
		//	415.30f, //G# || Ab
		//	440.0f, //A
		//	466.16, //A

		//}


		std::function<juce::String(juce::String)> hashConversionFunction = [=](juce::String key)
		{

			auto txt = key.substring(0, 1).toUpperCase() + key.substring(1);

			if (txt.length() > 1)
			{
				if (txt[1] == 'b') //flats
				{
					switch (txt[0])
					{
					case 'C':
						txt = "B";
						break;
					case 'F':
						txt = "E";
						break;
					}
				}
				if (txt[1] == '#') //flats
				{
					switch (txt[0])
					{
					case 'C':
						txt = "Db";
						break;
					case 'D':
						txt = "Eb";
						break;
					case 'E':
						txt = "F";
						break;
					case 'F':
						txt = "Gb";
						break;
					case 'G':
						txt = "Ab";
						break;
					case 'A':
						txt = "Bb";
						break;
					case 'B':
						txt = "C";
						break;
					}
				}
			}

			return txt;
		};


		static bdsp::DoubleHashedMap<juce::String, float> noteNameMap(hashConversionFunction);

		noteNameMap.set("C", 440 * pow(2, -9 / 12.0));
		noteNameMap.set("Db", 440 * pow(2, -8 / 12.0));
		noteNameMap.set("D", 440 * pow(2, -7 / 12.0));
		noteNameMap.set("Eb", 440 * pow(2, -6 / 12.0));
		noteNameMap.set("E", 440 * pow(2, -5 / 12.0));
		noteNameMap.set("F", 440 * pow(2, -4 / 12.0));
		noteNameMap.set("Gb", 440 * pow(2, -3 / 12.0));
		noteNameMap.set("G", 440 * pow(2, -2 / 12.0));
		noteNameMap.set("Ab", 440 * pow(2, -1 / 12.0));
		noteNameMap.set("A", 440);
		noteNameMap.set("Bb", 440 * pow(2, 1 / 12.0));
		noteNameMap.set("B", 440 * pow(2, 2 / 12.0));


		auto freqLambdaText = [=](const juce::String& s)
		{
			float base;
			auto txt = s.retainCharacters("0123456789.-abcdefgABCDEFG#");
			if (isBetweenInclusive(static_cast<char>(txt[0]), 'a', 'g') || isBetweenInclusive(static_cast<char>(txt[0]), 'A', 'G')) // checks if string starts with valid note name
			{
				int factor = txt.retainCharacters("0123456789").getIntValue() - 4;
				txt = txt.retainCharacters("abcdefgABCDEFG#");
				const juce::String key = txt;
				return noteNameMap[key] * powf(2, factor);
			}


			auto factor = (s.endsWithIgnoreCase("k") || s.endsWithIgnoreCase("kHz")) ? 1000 : 1;
			return txt.getFloatValue() * factor;
		};


		std::function<float(float, float, float)> from0To1Func = [=](float start, float end, float val)
		{
			return start + (pow(2, 10 * val) - 1) / (pow(2, 10) - 1) * (end - start);
		};

		std::function<float(float, float, float)> to0To1Func = [=](float start, float end, float val)
		{
			return log2((val - start) / (end - start) * (pow(2, 10) - 1) + 1) / 10;
		};

		auto freqRange = juce::NormalisableRange<float>(20.0f, 20000.0f, from0To1Func, to0To1Func);

		floatAttributes.add(new FloatParameterAttribute("Frequency", freqLambda, freqLambdaText, freqRange));


		//================================================================================================================================================================================================



		auto filterTypeLambda = [=](float value, int maximumStringLength)
		{
			auto LPAmt = juce::jlimit(0.0f, 1.0f, -2 * value + 1);
			auto BPAmt = juce::jlimit(0.0f, 1.0f, 1 - abs(2 * value - 1));
			auto HPAmt = juce::jlimit(0.0f, 1.0f, 2 * value - 1);

			juce::String out;
			if (LPAmt >= 0.01)
			{
				out = percentLambda(LPAmt, 0) + juce::String(" LP");
			}
			else if (HPAmt >= 0.01)
			{
				out = percentLambda(HPAmt, 0) + juce::String(" HP");
			}

			if (BPAmt > 0)
			{
				if (out.isEmpty())
				{
					out = juce::String("BP");
				}
			}
			return out;
		};

		auto filterTypeLambdaText = [=](juce::String s)
		{
			float out = 0;
			s = s.retainCharacters("-0123456789.hHlL");
			if (s.containsChar('l') || s.containsChar('L') || s.startsWith("-"))
			{
				out = 0.5 - abs(percentLambdaText(s)) / 2;
			}
			else if (s.containsChar('h') || s.containsChar('H'))
			{
				out = percentLambdaText(s) / 2 + 0.5;
			}
			else
			{
				out = percentLambdaText(s) / 2 + 0.5;
			}
			return out;
		};

		floatAttributes.add(new FloatParameterAttribute("Filter Type", filterTypeLambda, filterTypeLambdaText, juce::NormalisableRange<float>(0, 1)));

		//================================================================================================================================================================================================


		auto filterQLambda = [=](float value, int maximumStringLength)
		{
			return simplifyNumber(value, 2, true);
		};

		auto QRange = juce::NormalisableRange<float>(0.1, 10);
		QRange.skew = 0.5;

		floatAttributes.add(new FloatParameterAttribute("Filter Q", filterQLambda, std::function<float(const juce::String&)>(), QRange));


		//================================================================================================================================================================================================






		floatAttributes.add(new FloatParameterAttribute("Saturation", percentLambda, percentLambdaText, juce::NormalisableRange<float>(0.0f, 1.0f)));


		//================================================================================================================================================================================================



		auto BPMLambda = [=](float value, int maximumStringLength)
		{
			return simplifyNumber(round(value * 10) / 10, 1, true) + " BPM";
		};

		//BPMRange.skew = 0.05;

		float c0 = -0.177869;
		float c1 = -0.999974;

		std::function<float(float, float, float)> from0To1BPMFunc = [=](float start, float end, float val)
		{
			return start + c0 * atanh(c1 * val) * (end - start);
		};

		std::function<float(float, float, float)> to0To1BPMFunc = [=](float start, float end, float val)
		{
			float x = (val - start) / (end - start);
			return  1 / c1 * tanh(x / c0);
		};

		auto BPMRange = juce::NormalisableRange<float>(10.0, 1000.0, from0To1BPMFunc, to0To1BPMFunc);
		floatAttributes.add(new FloatParameterAttribute("BPM", BPMLambda, std::function<float(const juce::String&)>(), BPMRange));


		//================================================================================================================================================================================================

		auto LFOShapeLambda = [=](float value, int maximumStringLength)
		{
			float num = 3.0f;
			auto SinAmt = juce::jlimit(0.0f, 1.0f, 1 - (num - 1) * value);
			auto TriAmt = juce::jlimit(0.0f, 1.0f, 1 - abs(1 - (num - 1) * value));
			auto SqrAmt = juce::jlimit(0.0f, 1.0f, 1 - abs(2 - (num - 1) * value));
			juce::String out;
			if (SinAmt >= 0.01)
			{
				out = percentLambda(SinAmt, 0) + juce::String(" Sin");
			}
			else if (SqrAmt >= 0.01)
			{
				out = percentLambda(SqrAmt, 0) + juce::String(" Sqr");
			}

			if (TriAmt > 0)
			{
				if (out.isEmpty())
				{
					out = juce::String("Tri");
				}
			}
			return out;
		};

		auto LFOShapeLambdaText = [=](juce::String s)
		{
			float out = 0;
			s = s.retainCharacters("-0123456789.sSiInNqQrRtT");
			if (s.startsWithIgnoreCase("Si") || s.startsWith("-"))
			{
				out = 0.5 - abs(percentLambdaText(s)) / 2;
			}
			else
			{
				out = percentLambdaText(s) / 2 + 0.5;
			}
			return out;
		};



		floatAttributes.add(new FloatParameterAttribute("LFO Shape", LFOShapeLambda, LFOShapeLambdaText, juce::NormalisableRange<float>(0, 1)));





		//================================================================================================================================================================================================

		auto rateLambda = freqLambda;

		auto rateLambdaText = freqLambdaText;


		auto rateRange = juce::NormalisableRange <float>(0.01f, 200.0f);
		rateRange.skew = 0.2;
		floatAttributes.add(new FloatParameterAttribute("Hz Rate", rateLambda, rateLambdaText, rateRange)); // range can change based of needs


		//================================================================================================================================================================================================

		std::function< std::function<juce::String(float, int)>(double)> bitCrushRateLambdaGenerator = [=](double sampleRate)
		{

			std::function<juce::String(float, int)> bitCrushRateLambda = [=](float value, int maximumStringLength)
			{
				return freqLambda(sampleRate / value, maximumStringLength);
			};

			return bitCrushRateLambda;
		};

		std::function< std::function<float(juce::String)>(double)> bitCrushRateLambdaTextGenerator = [=](double sampleRate)
		{
			std::function<float(juce::String)> bitCrushRateLambdaText = [=](const juce::String& s)
			{
				return sampleRate / freqLambdaText(s);
			};

			return bitCrushRateLambdaText;
		};

		std::function<juce::NormalisableRange<float>(double)> bitCrushRateRangeGenerator = [=](double sampleRate)
		{
			return juce::NormalisableRange<float>(1.0, 32.0);
		};




		sampleRateDependentAttributes.add(new SampleRateDependentFloatParameterAttribute("Bit Crush Sampling Rate", bitCrushRateLambdaGenerator, bitCrushRateLambdaTextGenerator, bitCrushRateRangeGenerator));


		//================================================================================================================================================================================================

		auto bitCrushDepthLambda = [=](float value, int maximumStringLength)
		{
			return simplifyNumber(juce::jmap(value, 16.0f, (float)BDSP_BIT_CRUSH_MIN_DEPTH), 1, false);
		};

		auto bitCrushDepthLambdaText = [=](const juce::String& s)
		{
			return juce::jmap(s.retainCharacters("0123456789.-").getFloatValue(), 16.0f, (float)BDSP_BIT_CRUSH_MIN_DEPTH, 0.0f, 1.0f);
		};

		floatAttributes.add(new FloatParameterAttribute("Bit Crush Depth", bitCrushDepthLambda, bitCrushDepthLambdaText, juce::NormalisableRange<float>(0, 1)));
		//================================================================================================================================================================================================




		auto pitchShiftLambda = [=](float value, int maximumStringLength)
		{
			return simplifyNumber(value, 1, false);
		};

		auto pitchShiftLambdaText = [=](const juce::String& s)
		{

			return s.retainCharacters("0123456789.-").getFloatValue();
		};




		auto pitchShiftRange = juce::NormalisableRange<float>(-12.0f, 12.0f);

		floatAttributes.add(new FloatParameterAttribute("Pitch Shift", pitchShiftLambda, pitchShiftLambdaText, pitchShiftRange));

	}

	const FloatParameterAttribute& GenericParameterAttributes::getFloatAttribute(const juce::Identifier& ID)
	{
		for (auto a : floatAttributes)
		{
			if (a->ID == ID)
			{
				return *a;
			}
		}
		for (auto a : sampleRateDependentAttributes)
		{
			auto n = a->attribute.ID;
			if (n == ID)
			{
				return a->attribute;
			}
		}

	}


	SampleRateDependentFloatParameterAttribute* GenericParameterAttributes::getSampleRateDependentAttribute(const juce::Identifier& ID)
	{
		for (auto a : sampleRateDependentAttributes)
		{
			if (a->attribute.ID == ID)
			{
				return a;
			}
		}
		return nullptr;
	}

	bool GenericParameterAttributes::isSampleRateDependent(const juce::Identifier& ID)
	{
		for (auto a : sampleRateDependentAttributes)
		{
			if (a->attribute.ID == ID)
			{
				return true;
			}
		}
		return false;
	}

	void GenericParameterAttributes::addFloatAttribute(const FloatParameterAttribute& attributeToAdd)
	{
		addFloatAttribute(attributeToAdd.ID, attributeToAdd.valueToTextLambda, attributeToAdd.textToValueLambda, attributeToAdd.range);
	}

	void GenericParameterAttributes::addFloatAttribute(const juce::Identifier& id, const std::function<juce::String(float, int)>& V2T, const std::function<float(const juce::String&)>& T2V, const juce::NormalisableRange<float>& nRange)
	{

		for (auto a : floatAttributes) // make sure this attribute isn't already in here
		{
			if (a->ID == id)
			{
				return;
			}
		}
		floatAttributes.add(new FloatParameterAttribute(id, V2T, T2V, nRange));
	}

	void GenericParameterAttributes::updateSampleRateDependentAttributes(double sampleRate)
	{
		for (auto a : sampleRateDependentAttributes)
		{
			a->setNewSampleRate(sampleRate);
		}
	}

	const BoolParameterAttribute& GenericParameterAttributes::getBoolAttribute(const juce::Identifier& ID)
	{
		for (auto a : boolAttributes)
		{
			if (a->ID == ID)
			{
				return *a;
			}
		}

		return BoolParameterAttribute();
	}

	void GenericParameterAttributes::addBoolAttribute(const BoolParameterAttribute& attributeToAdd)
	{
		addBoolAttribute(attributeToAdd.ID, attributeToAdd.valueToTextLambda, attributeToAdd.textToValueLambda);

	}

	void GenericParameterAttributes::addBoolAttribute(const juce::Identifier& id, const std::function<juce::String(bool, int)>& V2T, const std::function<bool(const juce::String&)>& T2V)
	{
		for (auto a : boolAttributes) // make sure this attribute isn't already in here
		{
			if (a->ID == id)
			{
				return;
			}
		}
		boolAttributes.add(new BoolParameterAttribute(id, V2T, T2V));
	}

	void SampleRateDependentFloatParameterAttribute::setNewSampleRate(double newRate)
	{
		attribute.valueToTextLambda = lambdaGenerator(newRate);
		attribute.textToValueLambda = lambdaTextGenerator(newRate);
		attribute.range = rangeGenerator(newRate);

		for (auto p : effectedParameters)
		{
			p->updateLambdas(attribute.valueToTextLambda, attribute.textToValueLambda, attribute.range);
		}

	}

} // namespace bdsp