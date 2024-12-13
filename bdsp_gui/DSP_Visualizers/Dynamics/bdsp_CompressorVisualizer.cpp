#include "bdsp_CompressorVisualizer.h"

namespace bdsp
{


	template<typename SampleType>
	CompressorVisualizerInternal<SampleType>::CompressorVisualizerInternal(GUI_Universals* universalsToUse, dsp::Compressor<SampleType>* compToVis)
		:OpenGLFunctionVisualizer(universalsToUse, false)
	{
		compressor = compToVis;

		isBipolar = false;

		//int verts = vertexBuffer.getNumVerticies();
		//indexBuffer.addArray({
		//	verts, verts + 1, verts + 2,
		//	verts + 1, verts + 2, verts + 3
		//	});

		//vertexBuffer.init(verts + 4);
	}

	template<typename SampleType>
	CompressorVisualizerInternal<SampleType>::~CompressorVisualizerInternal()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	template<typename SampleType>
	void CompressorVisualizerInternal<SampleType>::resized()
	{
		OpenGLFunctionVisualizer::resized();
	}

	template<typename SampleType>
	void CompressorVisualizerInternal<SampleType>::newFrameInit()
	{
		envVal = compressor->getEnvValue();
		//gainReduction = compressor->getGainReduction();
	}

	template<typename SampleType>
	void CompressorVisualizerInternal<SampleType>::generateVertexBuffer()
	{
		OpenGLFunctionVisualizer::generateVertexBuffer();

	/*	int n = vertexBuffer.getNumVerticies() - 4;

		float r, g, b, a;

		lineColor.getComponents(r, g, b, a);

		float y = scalingY * (1 - 2 * gainReduction);
		vertexBuffer.set(n,
			{
				scalingX, scalingY,
				r,g,b,0
			});

		vertexBuffer.set(n + 1,
			{
				1.0, scalingY,
				r,g,b,0
			});

		vertexBuffer.set(n + 2,
			{
				scalingX, y,
				r,g,b,1
			});

		vertexBuffer.set(n + 3,
			{
				1.0, y,
				r,g,b,1
			});*/

	}

	template<typename SampleType>
	float CompressorVisualizerInternal<SampleType>::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		return 2 * compressor->calculateTransferCurve(normX) - 1;
	}

	template<typename SampleType>
	float CompressorVisualizerInternal<SampleType>::calculateAlpha(float x, float y)
	{
		return OpenGLFunctionVisualizer::calculateAlpha(x, y) * (x < envVal ? 1.5 : 1);
	}

	template<typename SampleType>
	CompressorVisualizer<SampleType>::CompressorVisualizer(GUI_Universals* universalsToUse, dsp::Compressor<SampleType>* compToVis)
		:OpenGlComponentWrapper<CompressorVisualizerInternal<SampleType>>(universalsToUse, compToVis)
	{
	}




	template class CompressorVisualizerInternal<float>;
	template class CompressorVisualizerInternal<double>;

	template class CompressorVisualizer<float>;
	template class CompressorVisualizer<double>;



} // namespace bdsp
