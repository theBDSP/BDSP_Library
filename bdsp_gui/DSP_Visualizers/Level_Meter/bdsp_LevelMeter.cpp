#include "bdsp_LevelMeter.h"

// Make it Horizontal too
namespace bdsp
{

	template<typename SampleType>
	LevelMeterController<SampleType>::LevelMeterController(dsp::SampleSource<SampleType>* sourceToTrack, SampleType attackMs, SampleType decayMs)
		:source(sourceToTrack)
	{
		filter.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);
		filter.setAttackTime(attackMs);
		filter.setReleaseTime(decayMs);
	}
	template<typename SampleType>
	dsp::StereoSample<SampleType> LevelMeterController<SampleType>::getStereoLevel()
	{
		juce::AudioBuffer<SampleType> outBuff = source.getBuffer(2,numSamples);
		auto outBlock = juce::dsp::AudioBlock<SampleType>(outBuff);
		juce::dsp::ProcessContextReplacing<SampleType> context(outBlock);

		filter.process(context);

		return dsp::StereoSample<SampleType>(context.getOutputBlock().getSample(0, 0), context.getOutputBlock().getSample(1, 0));
	}
	template<typename SampleType>
	void LevelMeterController<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
	{
		filter.prepare(spec);
		numSamples = spec.maximumBlockSize;
	}


	template class LevelMeterController<float>;
	template class LevelMeterController<double>;

	//================================================================================================================================================================================================


	template<typename SampleType>
	LevelMeter<SampleType>::LevelMeter(GUI_Universals* universalsToUse, LevelMeterController<SampleType>* controllerToUse, bool isVertical, int roundingTriangles)
		:OpenGLComponent(universalsToUse),
		color(universalsToUse, this)
	{
		vertical = isVertical;
		cornerTriangles = roundingTriangles;

		controller = controllerToUse;

		indexBuffer.clear();
		n = 2 * cornerTriangles + 4;

		vertexBuffer.resize(4 * n + (cornerTriangles > 0 ? 4 : 0));

		indexBuffer.addArray({
			0,1,2,
			1,2,3,

			n,n + 1,n + 2,
			n + 1,n + 2,n + 3,

			1,3, n + 1,
			3, n + 1, n + 3,

			2 * n,2 * n + 1,2 * n + 2,
			2 * n + 1,2 * n + 2,2 * n + 3,

			3 * n,3 * n + 1,3 * n + 2,
			3 * n + 1,3 * n + 2,3 * n + 3,

			2 * n + 1,2 * n + 3, 3 * n + 1,
			2 * n + 3, 3 * n + 1, 3 * n + 3

			});

		if (cornerTriangles > 0)
		{
			indexBuffer.addArray({


			2,3,4,
			3,4,5,
			3,5, 4 * n,

			n + 2,n + 3,n + 4,
			n + 3,n + 4,n + 5,
			n + 3,n + 5,4 * n + 1,

			n - 2, n - 1, 2 * n - 2,
			n - 1, 2 * n - 2, 2 * n - 1,
			n - 1, 4 * n, 2 * n - 1,
			4 * n, 2 * n - 1, 4 * n + 1,


			2 * n + 2,2 * n + 3,2 * n + 4,
			2 * n + 3,2 * n + 4,2 * n + 5,
			2 * n + 3,2 * n + 5,4 * n + 2,

			3 * n + 2,3 * n + 3,3 * n + 4,
			3 * n + 3,3 * n + 4,3 * n + 5,
			3 * n + 3,3 * n + 5,4 * n + 3,


			3 * n - 2, 3 * n - 1, 4 * n - 2,
			3 * n - 1, 4 * n - 2, 4 * n - 1,
			3 * n - 1, 4 * n + 2, 4 * n - 1,
			4 * n + 2, 4 * n - 1, 4 * n + 3

				});
		}

		for (int i = 0; i < cornerTriangles - 1; ++i)
		{
			for (int k = 0; k < 4; k++)
			{

				int j = k * n + 4 + 2 * i;
				int c = 4 * n + k;
				indexBuffer.addArray({
					j , j + 1, j + 2,
					j + 1, j + 2, j + 3,
					j + 1, j + 3, c,
					});

			}
		}
	}

	template<typename SampleType>
	LevelMeter<SampleType>::~LevelMeter()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}



	template<typename SampleType>
	void LevelMeter<SampleType>::setColors(const NamedColorsIdentifier& mainColor)
	{
		color.setColors(mainColor, background.getColorID(false).mixedWith(mainColor, universals->disabledAlpha));
	}

	template<typename SampleType>
	void LevelMeter<SampleType>::resized()
	{
		if (cornerTriangles > 0)
		{
			cornerX = 2 * universals->roundedRectangleCurve / getWidth(); //converts rrcurve to openGL space [-1,1]
			cornerY = 2 * universals->roundedRectangleCurve / getHeight(); //converts rrcurve to openGL space [-1,1]
		}

		float barRadius = (1 - 2 * border) / 2; // half the width/height of a signle bar
		if (vertical && cornerX > barRadius)
		{
			float scaling = barRadius / cornerX;
			cornerX *= scaling;
			cornerY *= scaling;
		}
		else if (!vertical && cornerY > barRadius)
		{
			float scaling = barRadius / cornerY;
			cornerX *= scaling;
			cornerY *= scaling;
		}
	}




	// Inherited via OpenGLComponent
	template<typename SampleType>
	inline void LevelMeter<SampleType>::generateVertexBuffer()
	{
		jassert(controller != nullptr);

		auto smp = controller->getStereoLevel();


		float propL = juce::jmap(juce::Decibels::gainToDecibels(float(smp.left)), juce::Decibels::gainToDecibels(0.0f), juce::Decibels::gainToDecibels(1.0f), -1.0f, 1.0f);
		float propR = juce::jmap(juce::Decibels::gainToDecibels(float(smp.right)), juce::Decibels::gainToDecibels(0.0f), juce::Decibels::gainToDecibels(1.0f), -1.0f, 1.0f);

		float r, g, b, a;
		color.getComponents(r, g, b, a);

		float featherX, featherY, amtL, amtR;



		if (vertical)
		{
			amtL = (1.0f - (cornerY / abs(-1 - propL))) * a;
			amtR = (1.0f - (cornerY / abs(-1 - propR))) * a;
			propL -= cornerY;
			propR -= cornerY;

			featherX = juce::jmin(2.0f / getWidth(), border / 2);
			featherY = 2.0f / getHeight();

			//================================================================================================================================================================================================

			vertexBuffer.set(0,
				{
					-1.0f + border - featherX, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(1,
				{
					-1.0f + border, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2,
				{
					-1.0f + border - featherX, propL,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3,
				{
					-1.0f + border , propL,
					r, g, b, amtL
				});


			vertexBuffer.set(n,
				{
					-border + featherX, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(n + 1,
				{
					-border, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(n + 2,
				{
					-border + featherX, propL,
					r, g, b, 0.0f
				});
			vertexBuffer.set(n + 3,
				{
					-border , propL,
					r, g, b, amtL
				});




			//================================================================================================================================================================================================
			//R

			vertexBuffer.set(2 * n,
				{
					border - featherX, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2 * n + 1,
				{
					border, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2 * n + 2,
				{
					border - featherX, propR,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2 * n + 3,
				{
					border, propR,
					r, g, b, amtR
				});

			vertexBuffer.set(3 * n,
				{
					1.0f - border + featherX, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3 * n + 1,
				{
					1.0f - border, -1.0f,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3 * n + 2,
				{
					1.0f - border + featherX, propR,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3 * n + 3,
				{
					1.0f - border, propR,
					r, g, b, amtR
				});

			//================================================================================================================================================================================================


			if (cornerTriangles > 0)
			{
				float centerXLL = -1.0f + border + cornerX;
				float centerXLR = -border - cornerX;
				vertexBuffer.set(4 * n,
					{
						 centerXLL, propL ,
						 r, g, b, amtL
					});	//rounded Center L-L

				vertexBuffer.set(4 * n + 1,
					{
						 centerXLR, propL ,
						 r, g, b, amtL
					});	//rounded Center L-R


				float centerXRL = border + cornerX;
				float centerXRR = 1.0f - border - cornerX;
				vertexBuffer.set(4 * n + 2,
					{
						 centerXRL, propR ,
						 r, g, b, amtR
					});	//rounded Center R-L

				vertexBuffer.set(4 * n + 3,
					{
						 centerXRR, propR ,
						 r, g, b, amtR
					});	//rounded Center R-R

				for (int i = 0; i < cornerTriangles; ++i)
				{
					float angle = 3 * PI / 2 + float(i + 1) / cornerTriangles * PI / 2;
					float sinAngle = sin(angle);
					float cosAngle = cos(angle);
					float dx = cornerX * sinAngle;
					float dy = cornerY * cosAngle;
					float featherDx = (cornerX + featherX) * sinAngle;
					float featherDy = (cornerY + featherY) * cosAngle;
					float L = (amtL + (1 - amtL) * cosAngle) * a;
					float R = (amtR + (1 - amtR) * cosAngle) * a;
					vertexBuffer.set(4 + 2 * i,
						{
							centerXLL + featherDx, propL + featherDy,
							r, g, b, 0.0f
						});
					vertexBuffer.set(4 + 2 * i + 1,
						{
							centerXLL + dx, propL + dy,
							r, g, b, L
						});
					vertexBuffer.set(n + 4 + 2 * i,
						{
							centerXLR - featherDx, propL + featherDy,
							r, g, b, 0.0f
						});
					vertexBuffer.set(n + 4 + 2 * i + 1,
						{
							centerXLR - dx, propL + dy,
							r, g, b, L
						});

					vertexBuffer.set(2 * n + 4 + 2 * i,
						{
							centerXRL + featherDx, propR + featherDy,
							r, g, b, 0.0f
						});

					vertexBuffer.set(2 * n + 4 + 2 * i + 1,
						{
							centerXRL + dx, propR + dy,
							r, g, b, R
						});

					vertexBuffer.set(3 * n + 4 + 2 * i,
						{
							centerXRR - featherDx, propR + featherDy,
							r, g, b, 0.0f
						});

					vertexBuffer.set(3 * n + 4 + 2 * i + 1,
						{
							centerXRR - dx, propR + dy,
							r, g, b, R
						});


				}
			}
		}
		else
		{
			amtL = (1.0f - (cornerX / abs(-1 - propL))) * a;
			amtR = (1.0f - (cornerX / abs(-1 - propR))) * a;
			propL -= cornerX;
			propR -= cornerX;

			featherX = 2.0f / getWidth();
			featherY = juce::jmin(2.0f / getHeight(), border / 2);

			float featherOut = 1.0f - border + featherY;
			float featherIn = border - featherY;
			float edgeOut = 1.0f - border;
			float edgeIn = border;
			//================================================================================================================================================================================================





			vertexBuffer.set(0,
				{
					-1.0f, featherOut,
					r, g, b, 0.0f
				});
			vertexBuffer.set(1,
				{
					-1.0f, edgeOut,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2,
				{
					propL, featherOut,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3,
				{
					propL, edgeOut,
					r, g, b, amtL
				});


			vertexBuffer.set(n,
				{
					-1.0f, featherIn,
					r, g, b, 0.0f
				});
			vertexBuffer.set(n + 1,
				{
					-1.0f, edgeIn,
					r, g, b, 0.0f
				});
			vertexBuffer.set(n + 2,
				{
					propL, featherIn,
					r, g, b, 0.0f
				});
			vertexBuffer.set(n + 3,
				{
					propL, edgeIn ,
					r, g, b, amtL
				});




			//================================================================================================================================================================================================
			//R

			vertexBuffer.set(2 * n,
				{
					-1.0f, -featherIn,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2 * n + 1,
				{
					-1.0f, -edgeIn,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2 * n + 2,
				{
					propR, -featherIn,
					r, g, b, 0.0f
				});
			vertexBuffer.set(2 * n + 3,
				{
					propR,-edgeIn ,
					r, g, b, amtR
				});

			vertexBuffer.set(3 * n,
				{
					-1.0f, -featherOut,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3 * n + 1,
				{
					-1.0f, -edgeOut,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3 * n + 2,
				{
					propR, -featherOut,
					r, g, b, 0.0f
				});
			vertexBuffer.set(3 * n + 3,
				{
					propR, -edgeOut,
					r, g, b, amtR
				});

			//================================================================================================================================================================================================


			if (cornerTriangles > 0)
			{
				float centerYLT = edgeOut - cornerY;
				float centerYLB = edgeIn + cornerY;
				vertexBuffer.set(4 * n,
					{
						 propL, centerYLT ,
						 r, g, b, amtL
					});	//rounded Center L-L

				vertexBuffer.set(4 * n + 1,
					{
						 propL, centerYLB ,
						 r, g, b, amtL
					});	//rounded Center L-R


				float centerYRT = -centerYLB;
				float centerYRB = -centerYLT;
				vertexBuffer.set(4 * n + 2,
					{
						 propR, centerYRT ,
						 r, g, b, amtR
					});	//rounded Center R-L

				vertexBuffer.set(4 * n + 3,
					{
						 propR, centerYRB ,
						 r, g, b, amtR
					});	//rounded Center R-R

				for (int i = 0; i < cornerTriangles; ++i)
				{
					float angle = float(i + 1) / cornerTriangles * PI / 2;
					float sinAngle = sin(angle);
					float cosAngle = cos(angle);
					float dx = cornerX * sinAngle;
					float dy = cornerY * cosAngle;
					float featherDx = (cornerX + featherX) * sinAngle;
					float featherDy = (cornerY + featherY) * cosAngle;
					float L = (amtL + (1 - amtL) * sinAngle) * a;
					float R = (amtR + (1 - amtR) * sinAngle) * a;
					vertexBuffer.set(4 + 2 * i,
						{
							propL + featherDx,centerYLT + featherDy,
							r, g, b, 0.0f
						});
					vertexBuffer.set(4 + 2 * i + 1,
						{
							propL + dx,centerYLT + dy,
							r, g, b, L
						});
					vertexBuffer.set(n + 4 + 2 * i,
						{
							propL + featherDx,centerYLB - featherDy,
							r, g, b, 0.0f
						});
					vertexBuffer.set(n + 4 + 2 * i + 1,
						{
							propL + dx,centerYLB - dy,
							r, g, b, L
						});

					vertexBuffer.set(2 * n + 4 + 2 * i,
						{
							propR + featherDx,centerYRT + featherDy,
							r, g, b, 0.0f
						});

					vertexBuffer.set(2 * n + 4 + 2 * i + 1,
						{
							propR + dx,centerYRT + dy,
							r, g, b, R
						});

					vertexBuffer.set(3 * n + 4 + 2 * i,
						{
							propR + featherDx,centerYRB - featherDy,
							r, g, b, 0.0f
						});

					vertexBuffer.set(3 * n + 4 + 2 * i + 1,
						{
							propR + dx,centerYRB - dy,
							r, g, b, R
						});


				}
			}
		}


	}

	template class LevelMeter<float>;
	template class LevelMeter<double>;
} // namespace bdsp
