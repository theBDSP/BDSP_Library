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


		void setDividerColor(const NamedColorsIdentifier& color);

		SequencerControllerObject* getController()
		{
			return controller;
		}

		NamedColorsIdentifier cutoutColor;
		NamedColorsIdentifier dividerColor;

		float numbersProportion = 0.4f;
	private:

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
		void paintOverChildren(juce::Graphics& g) override
		{
			OpenGlComponentWrapper<SequencerVisualizerInternal>::paintOverChildren(g);


			auto numbersRect = getLocalBounds().withTop(vis->getY() + vis->getHeight() * (1 - vis->numbersProportion));

			juce::Path p;
			auto font = resizeFontToFit(vis->getFont(), 0.8 * (float)getWidth() / BDSP_SEQUENCER_STEPS, vis->numbersProportion * 0.8 * getHeight(), juce::String(BDSP_SEQUENCER_STEPS));
			float scalingX = vis->getScaling().x;
			float scaledLeft = 1.0 - (scalingX + 1.0) / 2.0;
			float scaledRight = (scalingX + 1.0) / 2.0;

			for (int i = 0; i < BDSP_SEQUENCER_STEPS; ++i)
			{
				auto rect = vis->getBounds().toFloat().getProportion(juce::Rectangle<float>(juce::jmap((float)i / BDSP_SEQUENCER_STEPS,scaledLeft,scaledRight), 1 - vis->numbersProportion, (1.0f-2*scaledLeft) / BDSP_SEQUENCER_STEPS, vis->numbersProportion));

				p.addPath(createTextPath(font, juce::String(i + 1), rect));
			}

			juce::Path boundsRect;
			boundsRect.addRectangle(numbersRect);

			g.setColour(getColor(vis->cutoutColor));
			auto cutoutPath = ClipperLib::performBoolean(boundsRect, p, ClipperLib::ctDifference);
			g.fillPath(cutoutPath);


			juce::Path dividerSquares;
			for (int i = 0; i < BDSP_SEQUENCER_STEPS / 2; ++i)
			{
				auto left = juce::jmap((2.0f * i + 1.0f) / BDSP_SEQUENCER_STEPS,scaledLeft , scaledRight);
				auto right = juce::jmap((2.0f * i + 2.0f) / BDSP_SEQUENCER_STEPS, scaledLeft, scaledRight);

				dividerSquares.addRectangle(juce::Rectangle<float>(vis->getBounds().toFloat().getRelativePoint(left, 1.0f - vis->numbersProportion), vis->getBounds().toFloat().getRelativePoint(right, 1.0f)));
				dividerSquares.closeSubPath();
			}

			g.setColour(getColor(vis->dividerColor));
			g.fillPath(ClipperLib::performBoolean(cutoutPath, dividerSquares, ClipperLib::ctIntersection));


		}

	};






} // namespace bdsp