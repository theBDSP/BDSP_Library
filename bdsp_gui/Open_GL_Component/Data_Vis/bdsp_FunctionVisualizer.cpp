#include "bdsp_FunctionVisualizer.h"
namespace bdsp
{
	OpenGLFunctionVisualizer::OpenGLFunctionVisualizer(GUI_Universals* universalsToUse, bool shouldHaveZeroLine, bool shouldRenderAreaUnderCurve, int numOfSamplePoints)
		:OpenGLLineRenderer(universalsToUse, numOfSamplePoints),
		lineColor(universalsToUse, this),
		zeroLine(universalsToUse, this)
	{
		hasZeroLine = shouldHaveZeroLine;
		renderAreaUnderCurve = shouldRenderAreaUnderCurve;

		initArrays(numOfSamplePoints);

	}

	OpenGLFunctionVisualizer::~OpenGLFunctionVisualizer()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLFunctionVisualizer::setBipolar(bool newState)
	{
		if (isBipolar != newState)
		{
			isBipolar = newState;
			calculateZeroLine();
		}

	}



	void OpenGLFunctionVisualizer::resized()
	{
		if (!getBounds().isEmpty())
		{

			setThickness(0, universals->visualizerLineThickness);

			OpenGLLineRenderer::resized();

			scalingX = baseScalingX - (lineThicknessX.getFirst());
			scalingY = baseScalingY - (lineThicknessY.getFirst());
			calculateZeroLine();

		}
	}


	void OpenGLFunctionVisualizer::setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line, float topCurveOpacity, float botCurveOpacity)
	{
		lineColor.setColors(c, background.getColorID(false).mixedWith(&c, universals->disabledAlpha));

		zeroLine.setColors(line, background.getColorID(false).mixedWith(&line, universals->disabledAlpha));

		topCurveAlpha = topCurveOpacity;
		botCurveAlpha = botCurveOpacity;

		generateZeroLine();
		resized();
	}



	void OpenGLFunctionVisualizer::setScaling(float newXScalingFactor, float newYScalingFactor)
	{

		baseScalingX = newXScalingFactor;
		baseScalingY = newYScalingFactor == 0 ? newXScalingFactor : newYScalingFactor;

		resized();
		//universals->openGLContext->triggerRepaint();
	}

	void OpenGLFunctionVisualizer::generateZeroLine()
	{

		//if (hasZeroLine)
		//{

		//	float r, g, b, a;
		//	zeroLine.getComponents(r, g, b, a);

		//	vertexBuffer.set(zeroLineStart,
		//		{
		//			-1.0f, zeroY + BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
		//			r,g,b,a
		//		});

		//	vertexBuffer.set(zeroLineStart + 1,
		//		{
		//			1.0f, zeroY + BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
		//			r,g,b,a
		//		});
		//	vertexBuffer.set(zeroLineStart + 2,
		//		{
		//			-1.0f, zeroY - BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
		//			r,g,b,a
		//		});

		//	vertexBuffer.set(zeroLineStart + 3,
		//		{
		//			1.0f, zeroY - BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
		//			r,g,b,a
		//		});
		//}

	}

	void OpenGLFunctionVisualizer::initArrays(int numSamplePoints)
	{
		samplePoints = numSamplePoints;
		vertexBuffer.init((renderAreaUnderCurve ? 2 * samplePoints : 0) + (hasZeroLine ? 4 : 0));
		lineVertexBuffer[0]->init(samplePoints);
		indexBuffer.clear();
		zeroLineStart = vertexBuffer.getNumVerticies() - 4;
		if (hasZeroLine) // put behind function
		{
			indexBuffer.addArray(
				{
					zeroLineStart,zeroLineStart + 1,zeroLineStart + 2,
					zeroLineStart + 2, zeroLineStart + 3,zeroLineStart + 1
				}
			);
		}

		if (renderAreaUnderCurve)
		{
			for (int i = 0; i < samplePoints - 1; ++i)
			{
				indexBuffer.addArray(
					{
						//================================================================================================================================================================================================
						//Area Under Curve					
						//samplePoints + i,i,samplePoints + i + 1, //bl, tl, br
						//samplePoints + i + 1, i,i + 1, //br, tl, tr

						i, i + 1, samplePoints + i + 1,
						i, samplePoints + i, samplePoints + i + 1

					}
				);
			}
		}

		lineIndexBuffer[0]->clear();
		for (int i = 0; i < samplePoints; ++i)
		{
			lineIndexBuffer[0]->add(i);
		}
	}

	void OpenGLFunctionVisualizer::generateVertexBuffer()
	{
		float r, g, b, a;

		lineColor.getComponents(r, g, b, a);

		newFrameInit();
		//generateZeroLine();

		auto& p = *lineVertexBuffer[0];
		for (int i = 0; i < p.getNumVerticies(); ++i)
		{
			float normX = i / (samplePoints - 1.0f);
			float x = scalingX * (2.0f * normX - 1.0f);
			float y = scalingY * calculateFunctionSample(i, x, normX);
			if (renderAreaUnderCurve)
			{
				vertexBuffer.set(i,
					{
						x, y,
						r,g,b, calculateAlpha(x,y)
					} // AUC Top-i
				);
				vertexBuffer.set(samplePoints + i,
					{
						x, zeroY,
						r,g,b, calculateAlpha(x,zeroY)
					} // line AUC-i
				);
			}

			p.set(i,
				{
					x, y,
					r,g,b, 1.0f
				}
			);

		}

	}



	inline float OpenGLFunctionVisualizer::calculateAlpha(float x, float y)
	{
		return isBipolar ? juce::jmap(abs(y), 0.0f, 1.0f, botCurveAlpha, topCurveAlpha) : juce::jmap(y, -1.0f, 1.0f, botCurveAlpha, topCurveAlpha);
	}

	inline float OpenGLFunctionVisualizer::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		return 0.0f;
	}

	inline void OpenGLFunctionVisualizer::newFrameInit()
	{
	}



	void OpenGLFunctionVisualizer::calculateZeroLine()
	{
		zeroY = isBipolar ? 0.0f : -scalingY;
		generateZeroLine();
	}


} //namespace bdsp