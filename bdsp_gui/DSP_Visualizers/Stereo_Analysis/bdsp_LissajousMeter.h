#pragma once
namespace bdsp
{
	template<typename SampleType>
	class LissajousMeterInternal : public OpenGLCircleRenderer
	{
	public:
		LissajousMeterInternal(GUI_Universals* universalsToUse, dsp::SampleSource<SampleType>* sourceToUse, int numOfBuffers = 5)
			:OpenGLCircleRenderer(universalsToUse, sourceToUse->getBuffer().getNumSamples()* numOfBuffers),
			color(universalsToUse, this)
		{
			source = sourceToUse;
			numBuffers = numOfBuffers;
			bufferSize = sourceToUse->getBuffer().getNumSamples();
		}
		void setColors(const NamedColorsIdentifier& newColor)
		{
			color.setColors(newColor, newColor.withMultipliedAlpha(universals->disabledAlpha));
		}


		void generateVertexBuffer() override
		{
			float r, g, b, a;
			color.getComponents(r, g, b, a);

			a *= 0.75;

			float rad = getWidth() / 50.0;

			for (int i = 0; i < circleVertexBuffer.getNumVerticies(); ++i)
			{
				auto x = circleVertexBuffer.getAttribute(i, 0);
				auto y = circleVertexBuffer.getAttribute(i, 1);
				auto alpha = circleVertexBuffer.getAttribute(i, 7);
				alpha -= a / numBuffers;

				circleVertexBuffer.set(i, { x,y,rad * a,rad * a,r,g,b,alpha });
			}

			auto& buffer = source->getBuffer();
			for (int i = 0; i < bufferSize; ++i)
			{
				auto left = buffer.getSample(0, i);
				auto right = buffer.getSample(1, i);

				auto mid = left + right;
				auto side = left - right;
				circleVertexBuffer.set(bufferPtr * bufferSize + i, { -side,mid,rad,rad,r,g,b,a });
			}

			bufferPtr = (bufferPtr + 1) % numBuffers;
		}
	private:
		dsp::SampleSource<SampleType>* source;
		OpenGLColor color;
		int numBuffers;
		int bufferPtr = 0;
		int bufferSize;

	};

	template<typename SampleType>
	class LissajousMeter : public OpenGlComponentWrapper<LissajousMeterInternal<SampleType>>
	{
	public:
		LissajousMeter(GUI_Universals* universalsToUse, dsp::SampleSource<SampleType>* sourceToUse)
			:OpenGlComponentWrapper<LissajousMeterInternal<SampleType>>(universalsToUse, sourceToUse)
		{

		}

		void resized() override
		{
            OpenGlComponentWrapper<LissajousMeterInternal<SampleType>>::setBounds(confineToAspectRatio(OpenGlComponentWrapper<LissajousMeterInternal<SampleType>>::getBounds(), 1));

			OpenGlComponentWrapper<LissajousMeterInternal<SampleType>>::resized();
		}
	};

} // namespace bdsp
