#pragma once

namespace bdsp
{
	namespace dsp
	{
		namespace DistortionTypes
		{
			static const juce::Array<double> SaturationCompensationData = { 0.631858,0.634651,0.645243,0.651468,0.65943,0.665598,0.669227,0.677996,0.684346,0.692882,0.693714,0.701299,0.710521,0.71458,0.719969,0.723787,0.72796,0.732579,0.735451,0.739242,0.745028,0.748564,0.750901,0.754434,0.75829,0.763839,0.766319,0.76873,0.771557,0.775215,0.778759,0.782412,0.785089,0.785395,0.789489,0.79158,0.795413,0.797777,0.799966,0.802993,0.804044,0.805429,0.807122,0.808809,0.811608,0.816154,0.813875,0.815418,0.817781,0.8228,0.823288,0.825238,0.825615,0.827737,0.82724,0.830933,0.834493,0.832387,0.835398,0.838289,0.838582,0.840767,0.839293,0.841989,0.843609,0.845649,0.84552,0.847925,0.848657,0.849226,0.850808,0.852073,0.853243,0.85468,0.852677,0.85475,0.856741,0.857296,0.859527,0.859656,0.860052,0.861956,0.862829,0.863296,0.862637,0.86511,0.865448,0.866795,0.866919,0.86853,0.867748,0.87317,0.871422,0.871279,0.870702,0.873259,0.873923,0.874132,0.874847,0.874302,0.87506 };
			static const unsigned char SaturationPathData[] = { 110,109,0,0,128,191,0,0,128,63,108,164,112,125,191,209,206,127,63,108,72,225,122,191,166,156,127,63,108,236,81,120,191,116,105,127,63,108,144,194,117,191,54,53,127,63,108,52,51,115,191,225,255,126,63,108,216,163,112,191,110,201,126,63,108,124,20,110, 191,211,145,126,63,108,32,133,107,191,7,89,126,63,108,196,245,104,191,0,31,126,63,108,104,102,102,191,180,227,125,63,108,12,215,99,191,22,167,125,63,108,176,71,97,191,31,105,125,63,108,84,184,94,191,192,41,125,63,108,248,40,92,191,239,232,124,63,108, 156,153,89,191,156,166,124,63,108,64,10,87,191,188,98,124,63,108,228,122,84,191,64,29,124,63,108,136,235,81,191,27,214,123,63,108,44,92,79,191,59,141,123,63,108,208,204,76,191,147,66,123,63,108,116,61,74,191,16,246,122,63,108,24,174,71,191,158,167,122, 63,108,188,30,69,191,47,87,122,63,108,96,143,66,191,172,4,122,63,108,4,0,64,191,2,176,121,63,108,168,112,61,191,26,89,121,63,108,76,225,58,191,223,255,120,63,108,240,81,56,191,54,164,120,63,108,148,194,53,191,9,70,120,63,108,56,51,51,191,58,229,119,63, 108,220,163,48,191,175,129,119,63,108,128,20,46,191,74,27,119,63,108,36,133,43,191,233,177,118,63,108,200,245,40,191,110,69,118,63,108,108,102,38,191,178,213,117,63,108,16,215,35,191,146,98,117,63,108,180,71,33,191,231,235,116,63,108,88,184,30,191,132, 113,116,63,108,252,40,28,191,62,243,115,63,108,160,153,25,191,230,112,115,63,108,68,10,23,191,70,234,114,63,108,232,122,20,191,44,95,114,63,108,140,235,17,191,91,207,113,63,108,48,92,15,191,153,58,113,63,108,212,204,12,191,161,160,112,63,108,120,61,10, 191,46,1,112,63,108,28,174,7,191,246,91,111,63,108,192,30,5,191,166,176,110,63,108,100,143,2,191,235,254,109,63,108,8,0,0,191,100,70,109,63,108,88,225,250,190,178,134,108,63,108,160,194,245,190,102,191,107,63,108,232,163,240,190,13,240,106,63,108,48, 133,235,190,42,24,106,63,108,120,102,230,190,56,55,105,63,108,192,71,225,190,161,76,104,63,108,8,41,220,190,202,87,103,63,108,80,10,215,190,4,88,102,63,108,152,235,209,190,149,76,101,63,108,224,204,204,190,180,52,100,63,108,40,174,199,190,127,15,99,63, 108,112,143,194,190,5,220,97,63,108,184,112,189,190,61,153,96,63,108,0,82,184,190,5,70,95,63,108,72,51,179,190,29,225,93,63,108,144,20,174,190,37,105,92,63,108,216,245,168,190,155,220,90,63,108,32,215,163,190,211,57,89,63,108,104,184,158,190,247,126, 87,63,108,176,153,153,190,249,169,85,63,108,248,122,148,190,149,184,83,63,108,64,92,143,190,68,168,81,63,108,136,61,138,190,53,118,79,63,108,208,30,133,190,69,31,77,63,108,24,0,128,190,236,159,74,63,108,191,194,117,190,58,244,71,63,108,78,133,107,190, 196,23,69,63,108,221,71,97,190,142,5,66,63,108,108,10,87,190,255,183,62,63,108,251,204,76,190,200,40,59,63,108,138,143,66,190,206,80,55,63,108,25,82,56,190,14,40,51,63,108,168,20,46,190,133,165,46,63,108,55,215,35,190,25,191,41,63,108,198,153,25,190, 120,105,36,63,108,85,92,15,190,16,152,30,63,108,228,30,5,190,8,61,24,63,108,231,194,245,189,77,73,17,63,108,6,72,225,189,200,172,9,63,108,37,205,204,189,197,86,1,63,108,68,82,184,189,48,109,240,62,108,99,215,163,189,111,121,220,62,108,130,92,143,189, 120,184,198,62,108,65,195,117,189,221,25,175,62,108,126,205,76,189,153,155,149,62,108,187,215,35,189,249,158,116,62,108,241,195,245,188,65,192,58,62,108,108,216,163,188,103,79,252,61,108,206,217,35,188,52,97,126,61,108,164,240,48,181,142,186,137,54,108, 70,212,35,60,169,88,126,189,108,168,213,163,60,60,75,252,189,108,45,193,245,60,65,190,58,190,108,89,214,35,61,19,157,116,190,108,28,204,76,61,180,154,149,190,108,223,193,117,61,9,25,175,190,108,209,91,143,61,180,183,198,190,108,178,214,163,61,186,120, 220,190,108,147,81,184,61,140,108,240,190,108,116,204,204,61,121,86,1,191,108,85,71,225,61,131,172,9,191,108,54,194,245,61,14,73,17,191,108,140,30,5,62,206,60,24,191,108,253,91,15,62,220,151,30,191,108,110,153,25,62,72,105,36,191,108,223,214,35,62,237, 190,41,191,108,80,20,46,62,93,165,46,191,108,193,81,56,62,232,39,51,191,108,50,143,66,62,171,80,55,191,108,163,204,76,62,169,40,59,191,108,20,10,87,62,225,183,62,191,108,133,71,97,62,114,5,66,191,108,246,132,107,62,170,23,69,191,108,103,194,117,62,34, 244,71,191,108,216,255,127,62,213,159,74,191,108,164,30,133,62,48,31,77,191,108,92,61,138,62,34,118,79,191,108,20,92,143,62,50,168,81,191,108,204,122,148,62,133,184,83,191,108,132,153,153,62,233,169,85,191,108,60,184,158,62,232,126,87,191,108,244,214, 163,62,198,57,89,191,108,172,245,168,62,142,220,90,191,108,100,20,174,62,24,105,92,191,108,28,51,179,62,17,225,93,191,108,212,81,184,62,250,69,95,191,108,140,112,189,62,50,153,96,191,108,68,143,194,62,250,219,97,191,108,252,173,199,62,116,15,99,191,108, 180,204,204,62,170,52,100,191,108,108,235,209,62,141,76,101,191,108,36,10,215,62,252,87,102,191,108,220,40,220,62,192,87,103,191,108,148,71,225,62,153,76,104,191,108,76,102,230,62,49,55,105,191,108,4,133,235,62,35,24,106,191,108,188,163,240,62,6,240, 106,191,108,116,194,245,62,95,191,107,191,108,44,225,250,62,172,134,108,191,108,228,255,255,62,95,70,109,191,108,78,143,2,63,228,254,109,191,108,170,30,5,63,161,176,110,191,108,6,174,7,63,240,91,111,191,108,98,61,10,63,41,1,112,191,108,190,204,12,63, 155,160,112,191,108,26,92,15,63,147,58,113,191,108,118,235,17,63,86,207,113,191,108,210,122,20,63,39,95,114,191,108,46,10,23,63,66,234,114,191,108,138,153,25,63,224,112,115,191,108,230,40,28,63,58,243,115,191,108,66,184,30,63,128,113,116,191,108,158, 71,33,63,227,235,116,191,108,250,214,35,63,142,98,117,191,108,86,102,38,63,175,213,117,191,108,178,245,40,63,105,69,118,191,108,14,133,43,63,229,177,118,191,108,106,20,46,63,70,27,119,191,108,198,163,48,63,172,129,119,191,108,34,51,51,63,55,229,119,191, 108,126,194,53,63,6,70,120,191,108,218,81,56,63,51,164,120,191,108,54,225,58,63,220,255,120,191,108,146,112,61,63,23,89,121,191,108,238,255,63,63,255,175,121,191,108,74,143,66,63,170,4,122,191,108,166,30,69,63,45,87,122,191,108,2,174,71,63,155,167,122, 191,108,94,61,74,63,13,246,122,191,108,186,204,76,63,144,66,123,191,108,22,92,79,63,58,141,123,191,108,114,235,81,63,24,214,123,191,108,206,122,84,63,63,29,124,191,108,42,10,87,63,185,98,124,191,108,134,153,89,63,153,166,124,191,108,226,40,92,63,236, 232,124,191,108,62,184,94,63,190,41,125,191,108,154,71,97,63,28,105,125,191,108,246,214,99,63,21,167,125,191,108,82,102,102,63,177,227,125,191,108,174,245,104,63,253,30,126,191,108,10,133,107,63,4,89,126,191,108,102,20,110,63,208,145,126,191,108,194, 163,112,63,108,201,126,191,108,30,51,115,63,224,255,126,191,108,122,194,117,63,51,53,127,191,108,214,81,120,63,115,105,127,191,108,50,225,122,63,165,156,127,191,108,142,112,125,63,207,206,127,191,108,234,255,127,63,253,255,127,191,101,0,0 };
			/**
			 * Tape saturation style distortion.
			 * https://www.desmos.com/calculator/9u2sw1ijwm
			 */
			template<typename T>
			struct Saturation : public DistortionTypeBase<T>
			{
				Saturation()
					:DistortionTypeBase<T>(SaturationCompensationData, SaturationPathData, sizeof(SaturationPathData))
				{

				}
				T processSample(T in, T amt, bool isScaled) override
				{
					T a = 9 * amt + 1;

					return std::atan(a * in) / std::atan(a) * (isScaled ? DistortionTypeBase<T>::CompensationGain.processSample(amt) : 1);
				}

				inline static const juce::String Name{ "Saturation" };
			};

		}

	} //dsp namespace
} //bdsp namespace
