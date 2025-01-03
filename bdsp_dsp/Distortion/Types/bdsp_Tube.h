#pragma once

namespace bdsp
{
	namespace dsp
	{
		namespace DistortionTypes
		{
			static const juce::Array<double> TubeCompensationData = { 0.676531,0.687251,0.690584,0.695374,0.699234,0.703813,0.710596,0.716312,0.721865,0.722545,0.729295,0.731618,0.735723,0.741017,0.742392,0.744686,0.750301,0.752383,0.755241,0.756857,0.761922,0.765264,0.766122,0.769934,0.77565,0.773632,0.777828,0.779005,0.783877,0.783354,0.789005,0.788488,0.790566,0.790052,0.794436,0.794853,0.799531,0.798469,0.800771,0.803056,0.806152,0.805904,0.809833,0.808744,0.812209,0.811613,0.814773,0.8183,0.817472,0.817419,0.822643,0.820626,0.823904,0.824416,0.826376,0.825794,0.827489,0.82995,0.8315,0.831064,0.83297,0.834798,0.834526,0.834109,0.835652,0.837456,0.838119,0.840431,0.840476,0.841225,0.84309,0.844228,0.844579,0.845064,0.844219,0.848371,0.847899,0.848617,0.850134,0.849501,0.851065,0.853291,0.853917,0.854614,0.854424,0.856625,0.853815,0.856411,0.858058,0.857837,0.858597,0.861811,0.85839,0.861728,0.862688,0.863001,0.862914,0.865034,0.864064,0.864807,0.86719 };
			static const unsigned char TubePathData[] = { 110,109,0,0,128,191,0,0,128,63,108,164,112,125,191,32,184,127,63,108,72,225,122,191,242,110,127,63,108,236,81,120,191,108,36,127,63,108,144,194,117,191,135,216,126,63,108,52,51,115,191,54,139,126,63,108,216,163,112,191,113,60,126,63,108,124,20,110,191, 44,236,125,63,108,32,133,107,191,94,154,125,63,108,196,245,104,191,250,70,125,63,108,104,102,102,191,243,241,124,63,108,12,215,99,191,62,155,124,63,108,176,71,97,191,206,66,124,63,108,84,184,94,191,150,232,123,63,108,248,40,92,191,135,140,123,63,108, 156,153,89,191,147,46,123,63,108,64,10,87,191,172,206,122,63,108,228,122,84,191,192,108,122,63,108,136,235,81,191,193,8,122,63,108,44,92,79,191,155,162,121,63,108,208,204,76,191,62,58,121,63,108,116,61,74,191,153,207,120,63,108,24,174,71,191,148,98,120, 63,108,188,30,69,191,31,243,119,63,108,96,143,66,191,34,129,119,63,108,4,0,64,191,136,12,119,63,108,168,112,61,191,58,149,118,63,108,76,225,58,191,30,27,118,63,108,240,81,56,191,28,158,117,63,108,148,194,53,191,25,30,117,63,108,56,51,51,191,249,154,116, 63,108,220,163,48,191,157,20,116,63,108,128,20,46,191,232,138,115,63,108,36,133,43,191,183,253,114,63,108,200,245,40,191,235,108,114,63,108,108,102,38,191,95,216,113,63,108,16,215,35,191,237,63,113,63,108,180,71,33,191,108,163,112,63,108,88,184,30,191, 178,2,112,63,108,252,40,28,191,150,93,111,63,108,160,153,25,191,229,179,110,63,108,68,10,23,191,114,5,110,63,108,232,122,20,191,5,82,109,63,108,140,235,17,191,104,153,108,63,108,48,92,15,191,98,219,107,63,108,212,204,12,191,181,23,107,63,108,120,61,10, 191,28,78,106,63,108,28,174,7,191,85,126,105,63,108,192,30,5,191,22,168,104,63,108,100,143,2,191,13,203,103,63,108,8,0,0,191,233,230,102,63,108,88,225,250,190,81,251,101,63,108,160,194,245,190,230,7,101,63,108,232,163,240,190,63,12,100,63,108,48,133, 235,190,245,7,99,63,108,120,102,230,190,142,250,97,63,108,192,71,225,190,146,227,96,63,108,8,41,220,190,122,194,95,63,108,80,10,215,190,183,150,94,63,108,152,235,209,190,175,95,93,63,108,224,204,204,190,186,28,92,63,108,40,174,199,190,41,205,90,63,108, 112,143,194,190,57,112,89,63,108,184,112,189,190,25,5,88,63,108,0,82,184,190,232,138,86,63,108,72,51,179,190,176,0,85,63,108,144,20,174,190,102,101,83,63,108,216,245,168,190,234,183,81,63,108,32,215,163,190,252,246,79,63,108,104,184,158,190,71,33,78, 63,108,176,153,153,190,79,53,76,63,108,248,122,148,190,114,49,74,63,108,64,92,143,190,236,19,72,63,108,136,61,138,190,196,218,69,63,108,208,30,133,190,205,131,67,63,108,24,0,128,190,163,12,65,63,108,191,194,117,190,152,114,62,63,108,78,133,107,190,179, 178,59,63,108,221,71,97,190,164,201,56,63,108,108,10,87,190,176,179,53,63,108,251,204,76,190,171,108,50,63,108,138,143,66,190,218,239,46,63,108,25,82,56,190,230,55,43,63,108,168,20,46,190,188,62,39,63,108,55,215,35,190,107,253,34,63,108,198,153,25,190, 254,107,30,63,108,85,92,15,190,71,129,25,63,108,228,30,5,190,166,50,20,63,108,231,194,245,189,183,115,14,63,108,6,72,225,189,243,53,8,63,108,37,205,204,189,53,104,1,63,108,68,82,184,189,58,236,243,62,108,99,215,163,189,133,142,227,62,108,130,92,143,189, 66,124,209,62,108,65,195,117,189,124,109,189,62,108,126,205,76,189,122,9,167,62,108,187,215,35,189,156,225,141,62,108,241,195,245,188,59,212,98,62,108,108,216,163,188,190,223,33,62,108,206,217,35,188,182,37,174,61,108,164,240,48,181,131,243,201,54,108, 70,212,35,60,80,32,174,189,108,168,213,163,60,100,221,33,190,108,45,193,245,60,46,210,98,190,108,89,214,35,61,181,224,141,190,108,28,204,76,61,174,8,167,190,108,223,193,117,61,198,108,189,190,108,209,91,143,61,156,123,209,190,108,178,214,163,61,239,141, 227,190,108,147,81,184,61,182,235,243,190,108,116,204,204,61,249,103,1,191,108,85,71,225,61,187,53,8,191,108,54,194,245,61,131,115,14,191,108,140,30,5,62,119,50,20,191,108,253,91,15,62,27,129,25,191,108,110,153,25,62,213,107,30,191,108,223,214,35,62, 69,253,34,191,108,80,20,46,62,152,62,39,191,108,193,81,56,62,197,55,43,191,108,50,143,66,62,187,239,46,191,108,163,204,76,62,141,108,50,191,108,20,10,87,62,149,179,53,191,108,133,71,97,62,138,201,56,191,108,246,132,107,62,155,178,59,191,108,103,194,117, 62,129,114,62,191,108,216,255,127,62,141,12,65,191,108,164,30,133,62,185,131,67,191,108,92,61,138,62,176,218,69,191,108,20,92,143,62,218,19,72,191,108,204,122,148,62,96,49,74,191,108,132,153,153,62,61,53,76,191,108,60,184,158,62,54,33,78,191,108,244, 214,163,62,238,246,79,191,108,172,245,168,62,219,183,81,191,108,100,20,174,62,87,101,83,191,108,28,51,179,62,162,0,85,191,108,212,81,184,62,219,138,86,191,108,140,112,189,62,14,5,88,191,108,68,143,194,62,45,112,89,191,108,252,173,199,62,29,205,90,191, 108,180,204,204,62,175,28,92,191,108,108,235,209,62,164,95,93,191,108,36,10,215,62,172,150,94,191,108,220,40,220,62,112,194,95,191,108,148,71,225,62,136,227,96,191,108,76,102,230,62,133,250,97,191,108,4,133,235,62,234,7,99,191,108,188,163,240,62,54,12, 100,191,108,116,194,245,62,221,7,101,191,108,44,225,250,62,73,251,101,191,108,228,255,255,62,226,230,102,191,108,78,143,2,63,5,203,103,191,108,170,30,5,63,14,168,104,191,108,6,174,7,63,79,126,105,191,108,98,61,10,63,21,78,106,191,108,190,204,12,63,174, 23,107,191,108,26,92,15,63,92,219,107,191,108,118,235,17,63,99,153,108,191,108,210,122,20,63,255,81,109,191,108,46,10,23,63,107,5,110,191,108,138,153,25,63,225,179,110,191,108,230,40,28,63,144,93,111,191,108,66,184,30,63,173,2,112,191,108,158,71,33,63, 102,163,112,191,108,250,214,35,63,231,63,113,191,108,86,102,38,63,90,216,113,191,108,178,245,40,63,231,108,114,191,108,14,133,43,63,179,253,114,191,108,106,20,46,63,226,138,115,191,108,198,163,48,63,152,20,116,191,108,34,51,51,63,244,154,116,191,108, 126,194,53,63,21,30,117,191,108,218,81,56,63,25,158,117,191,108,54,225,58,63,26,27,118,191,108,146,112,61,63,54,149,118,191,108,238,255,63,63,133,12,119,191,108,74,143,66,63,31,129,119,191,108,166,30,69,63,27,243,119,191,108,2,174,71,63,145,98,120,191, 108,94,61,74,63,148,207,120,191,108,186,204,76,63,59,58,121,191,108,22,92,79,63,152,162,121,191,108,114,235,81,63,188,8,122,191,108,206,122,84,63,189,108,122,191,108,42,10,87,63,168,206,122,191,108,134,153,89,63,144,46,123,191,108,226,40,92,63,132,140, 123,191,108,62,184,94,63,146,232,123,191,108,154,71,97,63,203,66,124,191,108,246,214,99,63,58,155,124,191,108,82,102,102,63,239,241,124,191,108,174,245,104,63,246,70,125,191,108,10,133,107,63,91,154,125,191,108,102,20,110,63,42,236,125,191,108,194,163, 112,63,111,60,126,191,108,30,51,115,63,51,139,126,191,108,122,194,117,63,132,216,126,191,108,214,81,120,63,106,36,127,191,108,50,225,122,63,239,110,127,191,108,142,112,125,63,30,184,127,191,108,234,255,127,63,254,255,127,191,101,0,0 };
			/**
			 * Less agressive clipping distortion. Distortion amount reduces clipping threshold.
			 * https://www.desmos.com/calculator/prk2vmd2my
			 */
			template<typename T>
			struct Tube : public DistortionTypeBase<T>
			{
				Tube()
					:DistortionTypeBase<T>(TubeCompensationData, TubePathData, sizeof(TubePathData))
				{

				}
				T processSample(T in, T amt, bool isScaled) override
				{
					T a = 9 * amt + 1;

					return signum(in) * (1 - 1 / (abs(a * in) + 1)) / (1 - 1 / (a + 1)) * (isScaled ? DistortionTypeBase<T>::CompensationGain.processSample(amt) : 1);
				}

				inline static const juce::String Name{ "Tube" };

			};

		}

	} //dsp namespace
} //bdsp namespace
