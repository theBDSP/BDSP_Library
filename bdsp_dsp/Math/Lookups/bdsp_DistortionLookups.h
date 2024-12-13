#pragma once

constexpr double ln2 = 0.69314718056;

#define BDSP_BIT_CRUSH_MIN_DEPTH 4

namespace bdsp
{
	namespace dsp
	{

		enum class DistortionTypes :int { ArcSinH, BitCrush, HardClip, SoftClip, TanH, ArcTan, InverseProportion, InversePower, SineWaveFold, TriWaveFold, PositiveClip, DoubleBump, TanHPinch };
		// holds useful lookup tables to reduce computational load during process block
		template<typename T>
		class DistortionLookups
		{
		public:

			DistortionLookups();
			virtual ~DistortionLookups() = default;






			inline bool testDistortionFlag(DistortionTypes flag, DistortionTypes flagToTestFor)
			{
				return testConsecutiveFlag(flag, flagToTestFor);
			}

			inline juce::String distortionTypeToString(DistortionTypes type)
			{

				return flagMap[type].first;
			}

			inline DistortionTypes stringToDistortionType(const juce::String& s)
			{
				for (int i = 0; i < flagMap.size(); ++i)
				{
					if (flagMap[i].name.compare(s) == 0)
					{
						return DistortionTypes(i);
					}
				}
				return DistortionTypes(-1); // error
			}


			juce::StringArray getDistortionNamesForFlags(DistortionTypes flags)
			{
				juce::StringArray out;
				for (auto f : getAllSetConsecutiveFlags(flags))
				{
					out.add(flagMap[f].name);
				}

				return out;
			}

			juce::StringArray getOrderedDistortionNamesForFlags(juce::Array<DistortionTypes> flags)
			{
				juce::StringArray out;
				for (int i = 0; i < flags.size(); ++i)
				{
					out.add(flagMap[static_cast<int>(flags[i])].name);
				}
				return out;
			}


			void generateSingleTable(DistortionTypes flag)
			{
				(this->*(flagMap[static_cast<int>(flag)].generatorFunc))();
			}
			void generateTables(DistortionTypes flags)
			{
				for (int f : getAllSetConsecutiveFlags(static_cast<int>(flags)))
				{
					generateSingleTable(DistortionTypes(f));
				}
			}


			//================================================================================================================================================================================================

			inline T getArcSinHDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;
				return asinh(drive * in) / ArcSinHNormalizationConstant.processSample(drive) * (scaled ? ArcSinHCompensationGain.processSample(drive) : 1);
			}
			inline T getBitCrushDistortion(T in, T drive, bool scaled)
			{
				T bitDepth = juce::jmap(drive, (T)16, (T)BDSP_BIT_CRUSH_MIN_DEPTH);
				//bitDepth = log2f(pow(2, bitDepth) - 1);
				return  trunc(in * pow(2, bitDepth - 1)) / pow(2, bitDepth - 1) * (scaled ? BitCrushDistortionCompensationGain.processSample(drive) : 1);
			}
			inline T getHardClipDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;
				return signum(in) * juce::jmin(abs(drive * in), 1.0f) * (scaled ? HardClipCompensationGain.processSample(drive) : 1);
			}

			inline T getSoftClipDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;

				return SoftClipDistrtionLookup.processSample(drive * in / 10) * (scaled ? SoftClipCompensationGain.processSample(drive) : 1);
			}
			inline T getTanHDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;
				return TanHDistortionLookup.processSample(drive * in / 10) * (scaled ? TanHDistortionCompensationGain.processSample(drive) : 1);
			}

			inline T getArcTanDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;
				return atan(drive * in) / ArcTanDistortionNormalizationConstant.processSample(drive) * (scaled ? ArcTanDistortionCompensationGain.processSample(drive) : 1);
			}

			inline T getInverseProportionDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;
				return signum(in) * (1 - 1 / (abs(drive * in) + 1)) / (1 - 1 / (drive + 1)) * (scaled ? InverseProportionDistortionCompensationGain.processSample(drive) : 1);
			}

			inline T getInversePowerDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;
				return signum(in) * (1 - pow(abs(in) + 1, -drive)) / InversePowerDistortionNormalizationConstant(drive) * (scaled ? InversePowerDistortionCompensationGain.processSample(drive) : 1);
			}
			inline T getSineWaveFoldDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;
				return SineWaveFoldLookup.processSample(drive * in / 10) * (scaled ? SineWaveFoldDistortionCompensationGain.processSample(drive) : 1);
			}
			inline T getTriWaveFoldDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;

				return TriWaveFoldLookup.processSample(drive * in / 10) * (scaled ? TriWaveFoldDistortionCompensationGain.processSample(drive) : 1);
			}

			inline T getPositiveClipDistortion(T in, T drive, bool scaled)
			{
				//drive = 9 * drive + 1;
				drive = juce::jmap(drive, (T)1.5, (T)10.0);

				return juce::jmin(log(exp(drive * in) + 1) - (T)ln2, (T)1) * (scaled ? PositiveClipDistortionCompensationGain.processSample(drive) : 1);
			}
			inline T getDoubleBumpDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;

				return DoubleBumpDistortionLookup.processSample(drive * in / 10) * (scaled ? DoubleBumpDistortionCompensationGain.processSample(drive) : 1);
			}
			inline T getTanHPinchDistortion(T in, T drive, bool scaled)
			{
				drive = 9 * drive + 1;

				return TanHPinchDistortionLookup.processSample(drive * in / 10) * (scaled ? TanHPinchDistortionCompensationGain.processSample(drive) : 1);
			}





		private:

			bool ArcSinHTablesInitialized = false, BitCrushDistortionTablesInitialized = false, HardClipTablesInitialized = false, SoftClipTablesInitialized = false, TanHDistortionTablesInitialized = false, ArcTanDistortionTablesInitialized = false, InverseProportionDistortionTablesInitialized = false, InversePowerDistortionTablesInitialized = false, SineWaveFoldDistortionTablesInitialized = false, TriWaveFoldDistortionTablesInitialized = false, PositiveClipDistortionTablesInitialized = false, DoubleBumpDistortionTablesInitialized = false, TanHPinchDistortionTablesInitialized = false;
			juce::dsp::LookupTableTransform<T> ArcSinHCompensationGain, BitCrushDistortionCompensationGain, HardClipCompensationGain, SoftClipCompensationGain, TanHDistortionCompensationGain, ArcTanDistortionCompensationGain, InverseProportionDistortionCompensationGain, InversePowerDistortionCompensationGain, SineWaveFoldDistortionCompensationGain, TriWaveFoldDistortionCompensationGain, PositiveClipDistortionCompensationGain, DoubleBumpDistortionCompensationGain, TanHPinchDistortionCompensationGain;
			juce::dsp::LookupTableTransform<T> ArcSinHNormalizationConstant, ArcTanDistortionNormalizationConstant, InverseProportionDistortionNormalizationConstant, InversePowerDistortionNormalizationConstant, SineWaveFoldDistortionNormalizationConstant, TriWaveFoldDistortionNormalizationConstant, PositiveClipDistortionNormalizationConstant, DoubleBumpDistortionNormalizationConstant, TanHPinchDistortionNormalizationConstant;
			juce::dsp::LookupTableTransform<T> SoftClipDistrtionLookup, TanHDistortionLookup, SineWaveFoldLookup, TriWaveFoldLookup, DoubleBumpDistortionLookup, TanHPinchDistortionLookup;


			double delta = 0.001; // difference between sample points



			void generateArcSinHDistortionTables()
			{
				if (!ArcSinHTablesInitialized)
				{
					ArcSinHTablesInitialized = true;


					T c0 = 0.276849;
					T c1 = -0.0712149;
					T c2 = 0.159739;
					T c3 = -0.128887;
					T c4 = 0.0610935;
					T c5 = -0.0181915;
					T c6 = 0.00348653;
					T c7 = -0.000429222;
					T c8 = 0.0000327894;
					T c9 = -0.00000141483;
					T c10 = 2.6359e-8;

					T norm = 0.282751;


					// normalizes RMS
					//https://www.desmos.com/calculator/p38ams9nlh
					const std::function<T(T)> ArcSinHCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};
					ArcSinHCompensationGain.initialise(ArcSinHCompensationGainFunc, 1, 10, 900);
					//================================================================================================================================================================================================

					std::function<T(T)> ArcSinHNormalizationConstantFunc = [=](T amt)
					{
						return asinh(amt);
					};

					ArcSinHNormalizationConstant.initialise(ArcSinHNormalizationConstantFunc, 1, 10, 900);


				}
			}
			void generateBitCrushDistortionTables()
			{
				if (!BitCrushDistortionTablesInitialized)
				{
					BitCrushDistortionTablesInitialized = true;
					T c0 = 0.107426;
					T c1 = 0.223734;
					T c2 = -0.130234;
					T c3 = 0.0954322;
					T c4 = -0.0359259;
					T c5 = 0.00751419;
					T c6 = -0.000935552;
					T c7 = 0.0000705898;
					T c8 = -0.00000317978;
					T c9 = 8.2671e-8;
					T c10 = -1.1077e-9;

					T norm = 0.2675;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> BitCrushDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					BitCrushDistortionCompensationGain.initialise(BitCrushDistortionCompensationGainFunc, 1, 10, 900);
				}
			}
			void generateHardClipTables()
			{
				if (!HardClipTablesInitialized)
				{
					HardClipTablesInitialized = true;
					T c0 = 0.107426;
					T c1 = 0.223734;
					T c2 = -0.130234;
					T c3 = 0.0954322;
					T c4 = -0.0359259;
					T c5 = 0.00751419;
					T c6 = -0.000935552;
					T c7 = 0.0000705898;
					T c8 = -0.00000317978;
					T c9 = 8.2671e-8;
					T c10 = -1.1077e-9;

					T norm = 0.2675;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> HardClipCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					HardClipCompensationGain.initialise(HardClipCompensationGainFunc, 1, 10, 900);
				}
			}

			void generateSoftClipTables()
			{
				if (!SoftClipTablesInitialized)
				{
					SoftClipTablesInitialized = true;
					T c0 = 0.465743;
					T c1 = -0.351892;
					T c2 = 0.458812;
					T c3 = -0.32619;
					T c4 = 0.155569;
					T c5 = -0.0478032;
					T c6 = 0.00941593;
					T c7 = -0.00118257;
					T c8 = 0.0000916335;
					T c9 = -0.00000399414;
					T c10 = 7.49455e-8;

					T norm = 0.362708;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> SoftClipCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					SoftClipCompensationGain.initialise(SoftClipCompensationGainFunc, 1, 10, 900);




					//================================================================================================================================================================================================

					//================================================================================================================================================================================================

					std::function<T(T)> SoftClipFunc = [=](T in)
					{
						T kx = 10 * in;
						return kx / pow(1 + pow(abs(kx), 10), 1 / 10.0);
					};
					SoftClipDistrtionLookup.initialise(SoftClipFunc, -1, 1, 2 / delta);
				}
			}
			void generateTanHDistortionTables()
			{
				if (!TanHDistortionTablesInitialized)
				{
					TanHDistortionTablesInitialized = true;
					T c0 = 0.212184;
					T c1 = -0.0560771;
					T c2 = 0.174918;
					T c3 = -0.0825561;
					T c4 = 0.0196561;
					T c5 = -0.00200846;
					T c6 = -0.000136228;
					T c7 = 0.0000655918;
					T c8 = -0.00000803318;
					T c9 = 4.5763e-7;
					T c10 = -1.0349e-8;

					T norm = 0.265194;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> TanHDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					TanHDistortionCompensationGain.initialise(TanHDistortionCompensationGainFunc, 1, 10, 900);


					//================================================================================================================================================================================================

					std::function<T(T)> TanHFunction = [=](T x)
					{
						return tanh(10 * x);
					};
					TanHDistortionLookup.initialise(TanHFunction, -1, 1, 2 / delta + 1);



				}
			}

			void generateArcTanDistortionTables()
			{
				if (!ArcTanDistortionTablesInitialized)
				{
					ArcTanDistortionTablesInitialized = true;
					T c0 = 0.312879;
					T c1 = -0.117143;
					T c2 = 0.196539;
					T c3 = -0.119679;
					T c4 = 0.0464523;
					T c5 = -0.0122655;
					T c6 = 0.00221997;
					T c7 = -0.000269917;
					T c8 = 0.0000209675;
					T c9 = -9.3607e-7;
					T c10 = 1.821e-8;

					T norm = 0.308693;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> ArcTanDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1
					};

					ArcTanDistortionCompensationGain.initialise(ArcTanDistortionCompensationGainFunc, 1, 10, 900);

					//================================================================================================================================================================================================
					std::function<T(T)> ArcTanNormalizationConstantFunc = [=](T amt)
					{
						return atan(amt);
					};

					ArcTanDistortionNormalizationConstant.initialise(ArcTanNormalizationConstantFunc, 1, 10, 900);

				}
			}

			void generateInverseProportionDistortionTables()
			{
				if (!InverseProportionDistortionTablesInitialized)
				{
					InverseProportionDistortionTablesInitialized = true;
					T c0 = 0.225597;
					T c1 = -0.102208;
					T c2 = 0.234968;
					T c3 = -0.144331;
					T c4 = 0.0488384;
					T c5 = -0.00977439;
					T c6 = 0.00113192;
					T c7 = -0.0000645369;
					T c8 = 1.8605e-7;
					T c9 = 1.6375e-7;
					T c10 = -5.7576e-9;

					T norm = 0.253415;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> InverseProportionDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					InverseProportionDistortionCompensationGain.initialise(InverseProportionDistortionCompensationGainFunc, 1, 10, 900);



				}
			}
			void generateInversePowerDistortionTables()
			{
				if (!InversePowerDistortionTablesInitialized)
				{
					InversePowerDistortionTablesInitialized = true;
					T c0 = 0.241638;
					T c1 = 0.270014;
					T c2 = -0.308045;
					T c3 = 0.227469;
					T c4 = -0.0995958;
					T c5 = 0.0275615;
					T c6 = -0.00496104;
					T c7 = 0.000580002;
					T c8 = -0.0000425013;
					T c9 = 0.00000177383;
					T c10 = -3.2177e-8;

					T norm = 0.353972;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> InversePowerDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					InversePowerDistortionCompensationGain.initialise(InversePowerDistortionCompensationGainFunc, 1, 10, 900);


					//================================================================================================================================================================================================

					std::function<T(T)> InversePowerNormalizationConstantFunc = [=](T amt)
					{
						return 1 - pow(2, -amt);
					};

					InversePowerDistortionNormalizationConstant.initialise(InversePowerNormalizationConstantFunc, 1, 10, 900);


				}
			}
			void generateSineWaveFoldDistortionTables()
			{
				if (!SineWaveFoldDistortionTablesInitialized)
				{
					SineWaveFoldDistortionTablesInitialized = true;
					T c0 = 0.307781;
					T c1 = -0.331245;
					T c2 = 0.701463;
					T c3 = -0.484367;
					T c4 = 0.200261;
					T c5 = -0.0541094;
					T c6 = 0.00964981;
					T c7 = -0.00112155;
					T c8 = 0.0000815994;
					T c9 = -0.00000337147;
					T c10 = 6.0336e-8;

					T norm = 0.347634;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> SineWaveFoldDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					SineWaveFoldDistortionCompensationGain.initialise(SineWaveFoldDistortionCompensationGainFunc, 1, 10, 900);


					//================================================================================================================================================================================================
					std::function<T(T)> SineWaveFoldFunc = [=](T x)
					{
						return std::sin(PI * 5 * x);
					};

					SineWaveFoldLookup.initialise(SineWaveFoldFunc, -1, 1, 2 / delta + 1);

				}
			}
			void generateTriWaveFoldDistortionTables()
			{
				if (!TriWaveFoldDistortionTablesInitialized)
				{
					TriWaveFoldDistortionTablesInitialized = true;
					T c0 = 0.00454675;
					T c1 = 0.577915;
					T c2 = -0.5818;
					T c3 = 0.393939;
					T c4 = -0.148282;
					T c5 = 0.0326028;
					T c6 = -0.00431288;
					T c7 = 0.000333326;
					T c8 = -0.0000130898;
					T c9 = 1.2578e-7;
					T c10 = 4.427e-9;

					T norm = 0.276529;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> TriWaveFoldDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					TriWaveFoldDistortionCompensationGain.initialise(TriWaveFoldDistortionCompensationGainFunc, 1, 10, 900);


					//================================================================================================================================================================================================
					std::function<T(T)> TriWaveFoldFunc = [=](T x)
					{
						T p = 4 / 10.0;
						int f = floor(2 * x / p + 0.5);
						return 4 / p * (x - p / 2 * f) * (f % 2 == 0 ? 1 : -1);

					};

					TriWaveFoldLookup.initialise(TriWaveFoldFunc, -1, 1, 900);
				}
			}
			void generatePositiveClipDistortionTables()
			{
				if (!PositiveClipDistortionTablesInitialized)
				{
					PositiveClipDistortionTablesInitialized = true;
					T c0 = -0.11677;
					T c1 = 0.616213;
					T c2 = -0.469426;
					T c3 = 0.196681;
					T c4 = -0.0392765;
					T c5 = 0.00134248;
					T c6 = 0.00104071;
					T c7 = -0.00023486;
					T c8 = 0.0000234633;
					T c9 = -0.00000117341;
					T c10 = 2.3837e-8;

					T norm = 0.23526;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> PositiveClipDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					PositiveClipDistortionCompensationGain.initialise(PositiveClipDistortionCompensationGainFunc, 1, 10, 900);


					//================================================================================================================================================================================================


				}
			}
			void generateDoubleBumpDistortionTables()
			{
				if (!DoubleBumpDistortionTablesInitialized)
				{
					DoubleBumpDistortionTablesInitialized = true;
					T c0 = 0.278877;
					T c1 = -0.294066;
					T c2 = 0.506532;
					T c3 = -0.334692;
					T c4 = 0.136215;
					T c5 = -0.0365391;
					T c6 = 0.00654974;
					T c7 = -0.000774966;
					T c8 = 0.0000579773;
					T c9 = -0.00000247983;
					T c10 = 4.6127e-8;

					T norm = 0.261568;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> DoubleBumpDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					DoubleBumpDistortionCompensationGain.initialise(DoubleBumpDistortionCompensationGainFunc, 1, 10, 900);


					//================================================================================================================================================================================================

					std::function<T(T)> DoubleBumpFunction = [=](T x)
					{
						T a = 2.0 / 3.0;
						T b = 1.0 / 3.0;
						return 1 / 1.364 * ((exp(a * 10 * x) - exp(-b * 10 * x)) / (exp(10 * x) + exp(-10 * x)) + (exp(10 * x) - exp(-a * 10 * x)) / (exp(10 * x) + exp(-a * 10 * x)));

					};
					DoubleBumpDistortionLookup.initialise(DoubleBumpFunction, -1.5, 3, 4.5 / delta + 1);
				}
			}
			void generateTanHPinchDistortionTables()
			{
				if (!TanHPinchDistortionTablesInitialized)
				{
					TanHPinchDistortionTablesInitialized = true;
					T c0 = 0.108846;
					T c1 = 0.844712;
					T c2 = -0.839693;
					T c3 = 0.460713;
					T c4 = -0.141114;
					T c5 = 0.0249638;
					T c6 = -0.00240089;
					T c7 = 0.0000794536;
					T c8 = 0.00000633851;
					T c9 = -6.8454e-7;
					T c10 = 1.8739e-8;

					T norm = 0.455615;
					// normalizes RMS
					//https://www.desmos.com/calculator/p41medfiip
					const std::function<T(T)> TanHPinchDistortionCompensationGainFunc = [=](T k)
					{
						T out = c0;
						T factor = k;
						//================================================================================================================================================================================================
						//1
						out += c1 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//2
						out += c2 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//3
						out += c3 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//4
						out += c4 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//5
						out += c5 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//6
						out += c6 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//7
						out += c7 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//8
						out += c8 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//9
						out += c9 * factor;
						factor *= k;
						//================================================================================================================================================================================================
						//10
						out += c10 * factor;

						//================================================================================================================================================================================================

						return norm / out; // normalizes relative to RMS @ amount = 1

					};

					TanHPinchDistortionCompensationGain.initialise(TanHPinchDistortionCompensationGainFunc, 1, 10, 900);


					//================================================================================================================================================================================================

					std::function<T(T)> TanHPinchFunction = [=](T x)
					{
						int sign = 0;
						if (x != 0)
						{
							sign = abs(x) / x;
						}
						return sign * juce::jmax(juce::jmin(abs(10 * x), (T)1), abs(tanh(PI * PI * 10 * x) / 2));

					};
					TanHPinchDistortionLookup.initialise(TanHPinchFunction, -1, 1, 2 / delta + 1);



				}
			}



			struct DistortionType
			{
				DistortionType()
				{

				}

				DistortionType(const juce::String& s, void(DistortionLookups<T>::* genFunc)(), T(DistortionLookups<T>::* lookFunc)(T, T, bool))
					:name(s),
					generatorFunc(genFunc),
					lookupFunc(lookFunc)
				{

				}

				juce::String name;
				void(DistortionLookups<T>::* generatorFunc)();
				T(DistortionLookups<T>::* lookupFunc)(T, T, bool);
			};
			juce::Array<DistortionType> flagMap;

		public:
			inline decltype(DistortionType::lookupFunc) getDistortionFunc(DistortionTypes type)
			{
				if (static_cast<int>(type) < flagMap.size())
				{
					return flagMap[static_cast<int>(type)].lookupFunc;
				}
				else
				{
					return nullptr;
				}
			}


			inline T getDistortion(T in, T drive, bool scaled, DistortionTypes type)
			{
				return (this->*(flagMap[static_cast<int>(type)].lookupFunc))(in, drive, scaled);
			}
			JUCE_LEAK_DETECTOR(DistortionLookups)
		};

	}// namespace dsp

}//namespace bdsp
