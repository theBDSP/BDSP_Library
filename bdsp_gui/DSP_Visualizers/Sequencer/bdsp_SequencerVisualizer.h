#pragma once

namespace bdsp
{


	class SequencerVisualizerInternal final : public OpenGLFunctionVisualizer, public GUI_Universals::Listener
	{
	public:
		SequencerVisualizerInternal(GUI_Universals* universalsToUse, SequencerComponent* parentComp);
		~SequencerVisualizerInternal();


		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
		inline void newFrameInit() override;


		void mouseEnter(const juce::MouseEvent& e) override;
		void mouseExit(const juce::MouseEvent& e) override;
		void mouseDown(const juce::MouseEvent& e) override;
		void mouseDrag(const juce::MouseEvent& e) override;

		void paintOverChildren(juce::Graphics& g) override
		{
			auto rect1 = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(endPointVal, 0, 1 - endPointVal, 1 - numbersProportion));
			auto rect2 = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(endPointVal, 1 - numbersProportion, 1 - endPointVal, numbersProportion)).reduced(1);

			auto c = getBackgroundColor().getColor(isEnabled()).withMultipliedAlpha(1 - universals->disabledAlpha);

			auto numbersRect = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, 1 - numbersProportion, 1, numbersProportion));
			g.setColour(c);
			g.fillRect(rect1);
			g.fillRect(rect2);

			juce::Path p;
			auto font = resizeFontToFit(getFont(), 0.8 * (float)getWidth() / BDSP_SEQUENCER_STEPS, numbersProportion * 0.8 * getHeight(), juce::String(BDSP_SEQUENCER_STEPS));

			for (int i = 0; i < BDSP_SEQUENCER_STEPS; ++i)
			{
				auto rect = numbersRect.getProportion(juce::Rectangle<float>((float)i / BDSP_SEQUENCER_STEPS, 0, 1.0f / BDSP_SEQUENCER_STEPS, 1));

				p.addPath(createTextPath(font, juce::String(i + 1), rect));
			}

			juce::Path boundsRect;
			boundsRect.addRectangle(numbersRect);

			g.setColour(getColor(cutoutColor));
			g.fillPath(ClipperLib::performBoolean(boundsRect, p, ClipperLib::ctDifference));

		}

		void setDividerColor(const NamedColorsIdentifier& color);

		SequencerControllerObject* getController()
		{
			return controller;
		}

		NamedColorsIdentifier cutoutColor;

		float numbersProportion = 0.5f;
	private:
		NamedColorsIdentifier dividerColor;
		void setValueFromMouse(const juce::MouseEvent& e);

		// Inherited via Listener
		void GUI_UniversalsChanged() override
		{
			repaint();
		}

		void renderWithoutGenerating() override;
		void generateVertexBuffer() override;

		bool shapeStartsVertical(SequencerShapes shape);

		int vertsPerStep;

		SequencerControllerObject* controller;
		float progress = 0, progressIncrement = 0, shape = 0;

		SequencerComponent* parentComponent = nullptr;

		juce::Point<float> lastMousePos;
	};



	class SequencerVisualizer : public OpenGlComponentWrapper<SequencerVisualizerInternal>
	{
	public:
		SequencerVisualizer(GUI_Universals* universalsToUse, SequencerComponent* parentComp);
		virtual ~SequencerVisualizer() = default;
		void resized() override;
		void paint(juce::Graphics& g) override;

	};






} // namespace bdsp