#pragma once

namespace bdsp
{
	class OpenGLFFTLineVisualizer : public OpenGLFunctionVisualizer
	{
	public:
		OpenGLFFTLineVisualizer(GUI_Universals* universalsToUse, SpectrogramController* controller, bool isStereo = false, bool useLogFrequency = true, int numFramesToAvg = 3)
			:OpenGLFunctionVisualizer(universalsToUse, false, true, controller->getSize())
		{
			vals.resize(numFramesToAvg);
			controller->setBufferToPushTo(&vals);

		}
		virtual ~OpenGLFFTLineVisualizer()
		{
		}


	protected:
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override
		{
			auto arr = vals.getInternalArray();
			float tot = 0;

			auto fftDataIndex = juce::jmap(universals->freqRange.convertFrom0to1(normX), universals->freqRange.start, universals->freqRange.end, 0.0f, (float)samplePoints - 1);

			for (int i = 0; i < vals.getSize(); ++i)
			{
				tot += juce::jmap(fftDataIndex - (int)fftDataIndex, arr[i][floor(fftDataIndex)], arr[i][ceil(fftDataIndex)]);
			}

			auto level = juce::jmap(juce::Decibels::gainToDecibels(tot / vals.getSize()) - juce::Decibels::gainToDecibels((float)samplePoints), -60.0f, 0.0f, -1.0f, 1.0f);
			return level;
		}
		inline void newFrameInit() override
		{
		}


		CircularBuffer<juce::Array<float>> vals;

	private:
	};
} //namespace bdsp