#pragma once
#include "bdsp_BitCrushShaders.h"
namespace bdsp
{
	/**
	 * Gives an abstract representation of the state a bitcrusher.
	 * Displays an oval of a certain color, sample rate affets its horizontal resolution, bit depth affects its vertical resolution.
	 * Decreasing the wet mix will add in a grayscale filter.
	 */
	class BitCrushVisualizer : public OpenGLComponent
	{
	public:
		BitCrushVisualizer(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider);
		virtual ~BitCrushVisualizer();


		void createVertexAttributes() override;
		void removeVertexAttributes() override;

		void createShaders() override;
		void createUniforms() override;
		void deleteUniforms() override;


		void setColor(const NamedColorsIdentifier& newColor);

		void setScaling(float x, float y);


	protected:




		void generateVertexBuffer() override;




		OpenGLColor color;


		BaseSlider* bitDepth, * sampleRate, * mix;


		float rateVal, depthVal;
		float mixVal;

		float xScaling = 0.9f, yScaling = 0.9f;

		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> colorUniform, bitDepthUniform, rateUniform, mixUniform;

	};





} // namespace bdsp