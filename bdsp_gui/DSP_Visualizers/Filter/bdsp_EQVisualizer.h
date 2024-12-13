#pragma once



namespace bdsp
{

	class EQVisualizerInternal : public OpenGLCompositeComponent
	{
	public:
		EQVisualizerInternal(GUI_Universals* universalsToUse, dsp::ParametricEQ<float>* EQToUse, int numSamples);
		void setSliders(int band, BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* gainSlider);
		void setGlobalSliders(BaseSlider* gainSlider, BaseSlider* mixSlider);
		void generateVertexBuffer() override;

		void setScaling(float x, float y);

		void setColor(const NamedColorsIdentifier& color, const NamedColorsIdentifier& line = NamedColorsIdentifier(), float top = BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA, float bot = BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA);

		void resized() override;


		float borderX = 0, borderY = 0;
		std::function<void(int)> onHandleSelected;

		class EQFuncitonVisualizer final : public OpenGLFunctionVisualizer
		{
		public:

			EQFuncitonVisualizer(EQVisualizerInternal* parent, dsp::ParametricEQ<float>* EQToUse, int numSamples);
			~EQFuncitonVisualizer();


			void resized() override;
			float db_scale = 36.0f;
		protected:



			void newFrameInit() override;

			dsp::ParametricEQ<float>* eq;


			float mixVal = 1.0f;
			float gainVal = 1.0f;





			// Inherited via OpenGLFunctionVisualizer
			inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
			void calculateZeroLine() override;

			EQVisualizerInternal* p = nullptr;

		public:
			juce::Array<BaseSlider*> freqSliders, qSliders, gainSliders;
			BaseSlider* mixSlider = nullptr;
			BaseSlider* globalGain = nullptr;


		};

		class EQVisualizerHandle : public OpenGLCirclePlotter, public juce::Slider::Listener
		{
		public:

			EQVisualizerHandle(EQVisualizerInternal* parent);

			void setSliders(BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* gainSlider);

			void setToDefault();

			void resized() override;

			void generateVertexBuffer() override;



			void setColor(const NamedColorsIdentifier& norm, const NamedColorsIdentifier& down, const OpenGLColor& bkgd);


			juce::Point<float> getGLCenter();

		private:
			juce::Point<float> compPointToGLPoint(juce::Point<float> compPoint);
			juce::Point<float> GLPointToCompPoint(juce::Point<float> glPoint);

			juce::Rectangle<float> compBoundsToGLBounds(juce::Rectangle<float> compBounds);
			juce::Rectangle<float> GLBoundsToCompBounds(juce::Rectangle<float> glBounds);

			juce::Rectangle<float> getHandleBounds();

			void setParamsFromGLCenter();


			BaseSlider* freq = nullptr, * q = nullptr, * gain = nullptr;

			OpenGLColor normColor, downColor;


			juce::Point<float> glCenter;

			EQVisualizerInternal* p = nullptr;

			void selected();

			class MouseComp : public Component, public juce::ComponentDragger
			{
			public:

				MouseComp(EQVisualizerHandle* p);

				void mouseEnter(const juce::MouseEvent& e) override;
				void mouseExit(const juce::MouseEvent& e) override;
				void mouseDown(const juce::MouseEvent& e) override;
				void mouseDoubleClick(const juce::MouseEvent& e) override;
				void mouseUp(const juce::MouseEvent& e) override;
				void mouseDrag(const juce::MouseEvent& e) override;
				void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override;

				bool isDown = false, isOver = false, isDragging = false;

			private:
				EQVisualizerHandle* parent;


			};
			std::unique_ptr<MouseComp> mouseComp;

			// Inherited via Listener
			void sliderValueChanged(juce::Slider* slider) override;
		};

		void handleSelected(EQVisualizerHandle* handleSelected);

		int selectedHandle = 0;

		juce::Array<EQVisualizerHandle*> handlePointers;
		OpenGLRing* ringPointer;

		juce::ComponentBoundsConstrainer constrainer;


	};


	class EQVisualizer : public OpenGlComponentWrapper<EQVisualizerInternal>
	{
	public:
		EQVisualizer(GUI_Universals* universalsToUse, dsp::ParametricEQ<float>* EQToUse, int numSamples)
			:OpenGlComponentWrapper<EQVisualizerInternal>(universalsToUse, EQToUse, numSamples)
		{

		}

		void selectHandle(int idx)
		{
			vis->selectedHandle = idx;
		}



	};

} // namespace bdsp
