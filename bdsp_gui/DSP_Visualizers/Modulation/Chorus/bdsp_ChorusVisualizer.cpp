#include "bdsp_ChorusVisualizer.h"

namespace bdsp
{


	ChorusVisualizerInternal::ChorusVisualizerInternal(GUI_Universals* universalsToUse, dsp::Chorus<float>* chorusToUse, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGLComponent(universalsToUse),
		color(universalsToUse, this)
	{
		chorus = chorusToUse;
		lookup = lookupsToUse;

		vertexBuffer.init(BDSP_CHORUS_MAX_VOICES * 8 * 2 + 8);

		for (int i = 0; i < 2 * BDSP_CHORUS_MAX_VOICES + 1; ++i)
		{


			int n = 8 * i;
			indexBuffer.addArray({
				n,n + 1,n + 4,
				n + 1,n + 4,n + 5,

				n + 1,n + 2,n + 5,
				n + 2,n + 5,n + 6,


				n + 2,n + 3,n + 6,
				n + 3,n + 6,n + 7

				});
		}




	}

	ChorusVisualizerInternal::~ChorusVisualizerInternal()
	{

	}

	void ChorusVisualizerInternal::setColor(const NamedColorsIdentifier& newColor)
	{
		color.setColors(newColor, NamedColorsIdentifier(newColor).withMultipliedAlpha(universals->disabledAlpha));
	}

	void ChorusVisualizerInternal::generateVertexBuffer()
	{
		float r, g, b, a;
		color.getComponents(r, g, b, a);


		//================================================================================================================================================================================================
		numVoices = numVoicesParam != nullptr ? juce::jmap(numVoicesParam->getValue(), (float)BDSP_CHORUS_MIN_VOICES, (float)BDSP_CHORUS_MAX_VOICES) : BDSP_CHORUS_MIN_VOICES;


		depth = depthParam != nullptr ? depthParam->getActualValue() / BDSP_CHORUS_DEPTH_MAX_MS : 1;


		stereoWidth = stereoWidthParam != nullptr ? stereoWidthParam->getActualValue() : 0;


		mix = mixParam != nullptr ? mixParam->getActualValue() : 0.5f;



		//================================================================================================================================================================================================




		indexBuffer.resize((2 * numVoices) * 18 + 12);
		vertexBuffer.resize((2 * numVoices) * 8 + 8);


		auto barW = 0.01f;

		auto barH = yScale * 0.35f;

		float alpha = a * (1 - mix);
		vertexBuffer.set(0,
			{
				-barW, 1,
				r,g,b, 0
			});
		vertexBuffer.set(1,
			{
				-barW / 2, 1,
				r,g,b, alpha
			});
		vertexBuffer.set(2,
			{
				barW / 2, 1,
				r,g,b, alpha
			});
		vertexBuffer.set(3,
			{
				barW, 1,
				r,g,b, 0
			});
		vertexBuffer.set(4,
			{
				-barW, -1,
				r,g,b, 0
			});
		vertexBuffer.set(5,
			{
				-barW / 2, -1,
				r,g,b, alpha
			});
		vertexBuffer.set(6,
			{
				barW / 2, -1,
				r,g,b, alpha
			});
		vertexBuffer.set(7,
			{
				barW, -1,
				r,g,b, 0
			});




		alpha = a * mix;
		for (int i = 0; i < numVoices; ++i)
		{
			auto wL = xScale * getModPosition(i);
			auto wR = xScale * getModPosition(i, false);



			float x = wL;
			float c = yScale * 0.5f;
			int n = 16 * i + 8;

			vertexBuffer.set(n,
				{
					x - 2 * barW, c + barH,
					r,g,b,0
				});
			vertexBuffer.set(n + 1,
				{
					x - barW , c + barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 2,
				{
					x + barW , c + barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 3,
				{
					x + 2 * barW, c + barH,
					r,g,b,0
				});
			vertexBuffer.set(n + 4,
				{
					x - 2 * barW, c - barH,
					r,g,b,0
				});
			vertexBuffer.set(n + 5,
				{
					x - barW , c - barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 6,
				{
					x + barW , c - barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 7,
				{
					x + 2 * barW, c - barH,
					r,g,b,0
				});



			c *= -1;
			x = wR;
			vertexBuffer.set(n + 8,
				{
					x - 2 * barW, c + barH,
					r,g,b,0
				});
			vertexBuffer.set(n + 9,
				{
					x - barW , c + barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 10,
				{
					x + barW , c + barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 11,
				{
					x + 2 * barW, c + barH,
					r,g,b,0
				});
			vertexBuffer.set(n + 12,
				{
					x - 2 * barW, c - barH,
					r,g,b,0
				});
			vertexBuffer.set(n + 13,
				{
					x - barW , c - barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 14,
				{
					x + barW , c - barH,
					r,g,b,alpha
				});
			vertexBuffer.set(n + 15,
				{
					x + 2 * barW, c - barH,
					r,g,b,0
				});


		}

	}

	void ChorusVisualizerInternal::setScaling(float xScaling, float yScaling)
	{
		xScale = xScaling;
		yScale = yScaling;
	}

	float ChorusVisualizerInternal::getModPosition(int voice, bool left)
	{
		return chorus->getCurrentModPhaseProportionally(voice, left);
	}


} // namespace bdsp