#pragma once



#define BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA 0.5f
#define BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA 0.25f
#define BDSP_OPEN_GL_LINE_FEATHER_RATIO 3.5f
#define BDSP_OPEN_GL_LINE_MINIMUM_FEATHER_SIZE 2.0f

#define BDSP_OPEN_GL_ROUNDED_RECTANGLE_TRIANGLE_NUMBER 32

#define BDSP_OSCILLOSCOPE_MAX_WINDOW_MS 1000
namespace bdsp
{

	class OpenGLColor : public GUI_Universals::Listener
	{
	public:

		OpenGLColor(GUI_Universals* universalsToUse, juce::Component* comp)
			:GUI_Universals::Listener(universalsToUse)
		{
			component = comp;
		}

		~OpenGLColor()
		{
		}

		void setColors(const NamedColorsIdentifier& newEnabledColor, const NamedColorsIdentifier& newDisabledColor)
		{
			enabledColor = newEnabledColor;
			disabledColor = newDisabledColor;
			GUI_UniversalsChanged();
		}

		operator juce::Colour()
		{
			return component->isEnabled() ? juce::Colour::fromFloatRGBA(r, g, b, a) : juce::Colour::fromFloatRGBA(dR, dG, dB, dA);
		}

		juce::Colour getColor(bool isEnabled)
		{
			return isEnabled ? juce::Colour::fromFloatRGBA(r, g, b, a) : juce::Colour::fromFloatRGBA(dR, dG, dB, dA);
		}

		float getRed()
		{
			return component->isEnabled() ? r : dR;
		}
		float getGreen()
		{
			return component->isEnabled() ? g : dG;
		}
		float getBlue()
		{
			return component->isEnabled() ? b : dB;
		}
		float getAlpha()
		{
			return component->isEnabled() ? a : dA;
		}

		void getComponents(float& red, float& green, float& blue, float& alpha)
		{
			if (component->isEnabled())
			{
				red = r;
				green = g;
				blue = b;
				alpha = a;
			}
			else
			{
				red = dR;
				green = dG;
				blue = dB;
				alpha = dA;
			}
		}

		void getInterpolatedComponents(float& red, float& green, float& blue, float& alpha, juce::Colour other, float otherAmt)
		{
			getComponents(red, green, blue, alpha);
			red = juce::jmap(otherAmt, red, other.getFloatRed());
			green = juce::jmap(otherAmt, green, other.getFloatGreen());
			blue = juce::jmap(otherAmt, blue, other.getFloatBlue());
			alpha = juce::jmap(otherAmt, alpha, other.getFloatAlpha());
		}

		NamedColorsIdentifier getColorID(bool isEnabled) const
		{
			return isEnabled ? enabledColor : disabledColor;
		}

		void GUI_UniversalsChanged() override
		{
			auto c = universalsPTR->colors.getColor(enabledColor);
			r = c.getFloatRed();
			g = c.getFloatGreen();
			b = c.getFloatBlue();
			a = c.getFloatAlpha();

			//================================================================================================================================================================================================

			c = universalsPTR->colors.getColor(disabledColor);
			dR = c.getFloatRed();
			dG = c.getFloatGreen();
			dB = c.getFloatBlue();
			dA = c.getFloatAlpha();

		}



	private:
		float r, g, b, a;
		float dR, dG, dB, dA;


		juce::Component* component;
		NamedColorsIdentifier enabledColor, disabledColor;
	};

	//================================================================================================================================================================================================


	struct VertexArray
	{

		VertexArray(int numAttr = 6)
			:num(numAttr)
		{
		}

		void init(int maxSize)
		{
			arr.resize(maxSize * num);
			n = maxSize * num;
		}
		void resize(int newSize)
		{
			n = newSize * num;
		}

		void resizeByVertex(int numVerticies)
		{
			n = numVerticies * num;
		}

		int size()
		{
			return n;
		}

		int getNumVerticies()
		{
			return n / num;
		}

		juce::Array<float>& getArray()
		{
			return arr;
		}


		const int getNumAttributes()
		{
			return num;
		}



		void set(int idx, std::initializer_list<float> v)
		{
			jassert(v.size() == num);

			auto it = v.begin();
			for (int i = 0; i < num; i++)
			{
				arr.set(num * idx + i, *it);
				it++;
			}

			if (idx * num >= n)
			{
				n += num;
			}
		}


		void set(int idx, ...)
		{
			std::va_list args;
			va_start(args, idx);

			for (int i = 0; i < num; i++)
			{
				arr.set(num * idx + i, va_arg(args, double));
			}

			if (idx * num >= n)
			{
				n += num;
			}
		}




		void set(int idx, float* v)
		{
			for (int i = 0; i < num; i++)
			{
				arr.set(num * idx + i, v[i]);
			}

			if (idx * num >= n)
			{
				n += num;
			}
		}

		float* data()
		{
			return arr.data();
		}



		float getAttribute(int vertexIdx, int attributeIdx)
		{
			return arr[num * vertexIdx + attributeIdx];
		}

		int n = 0;
		const int num;
		juce::Array<float> arr;
	};

	struct IndexArray
	{
		unsigned int operator[](int i)
		{
			return arr[i];
		}
		void init(int maxSize)
		{
			arr.resize(maxSize);
			n = maxSize;
		}
		void resize(int newSize)
		{
			n = newSize;
		}
		int size()
		{
			return n;
		}
		juce::Array<unsigned int>& getArray()
		{
			return arr;
		}

		void set(int idx, unsigned int v)
		{
			arr.set(idx, v);
		}

		void add(unsigned int v)
		{
			arr.add(v);
			++n;
		}

		template <typename TypeToCreateFrom>
		void addArray(const std::initializer_list<TypeToCreateFrom>& items)
		{
			arr.addArray(items);
			n += items.size();
		}


		unsigned int* data()
		{
			return arr.data();
		}

		void clear()
		{
			arr.clear();
			n = 0;
		}

		juce::Array<unsigned int> arr;
		int n = 0;
	};




	class OpenGLComponent : public Component, public juce::OpenGLRenderer
	{
	public:
		OpenGLComponent(GUI_Universals* universalsToUse, int numAttr = 6);
		virtual ~OpenGLComponent();




		void paint(juce::Graphics& g) override
		{

		}
		void setBackgroundColor(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& bkgdBehind);
		OpenGLColor getBackgroundColor();
		void resized() override;
		void newOpenGLContextCreated() override;

		void renderOpenGL() override;


		void openGLContextClosing() override;

		virtual void renderWithoutGenerating();

		virtual void createShaders();
		virtual void createUniforms();
		virtual void createVertexAttributes();
		virtual void removeVertexAttributes();

		virtual void generateVertexBuffer();

		VertexArray vertexBuffer;
		IndexArray indexBuffer;

		const char* vertexShader;
		const char* fragmentShader;
		bool shouldDrawBackground = true;

		std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;

		GLuint vbo; // Vertex buffer object.
		GLuint ibo; // Index buffer object.
	protected:







		juce::Rectangle<int> vpBounds;

		OpenGLColor background;





		const int numAttributes;






		juce::Array<juce::Point<float>> generateRoundedCornerPoints(float centerX, float centerY, float angleStart, float angleEnd, float radiusX, float radiusY, int numTriangles = BDSP_OPEN_GL_ROUNDED_RECTANGLE_TRIANGLE_NUMBER, float feather = 0); // feather ratio, positive feathers the outside of the curve, negative feathers inside


		bool buffersCreated = false;
	};


	//================================================================================================================================================================================================

	class OpenGLLineRenderer : public OpenGLComponent
	{
	public:

		OpenGLLineRenderer(GUI_Universals* universalsToUse, juce::Array<int> lineMaxPoints = juce::Array<int>(100));
		virtual ~OpenGLLineRenderer();
		void generateLineTriangles();

		void resized() override;

		void setThickness(int idx, float thickness);


		void setFeatherRatio(float newRatio);
		juce::OwnedArray<VertexArray> linePoints;

		void renderWithoutGenerating() override;


		void setHasAlpha(bool canHaveAlpha); // transparency on curves can cause artifacting from overlapping segments



		juce::Array<float> lineThicknessX, lineThicknessY; // converted to GL space based on component size already halved for faster computation
		juce::Array<float> lineFeatherX, lineFeatherY;
		juce::Array<float> lineScreenThickness;
		juce::Array<float> lineScreenFeather;

	protected:
		float featherRatio = BDSP_OPEN_GL_LINE_FEATHER_RATIO;




		juce::OwnedArray<VertexArray> lineVertexBuffer;
		juce::OwnedArray<IndexArray> lineIndexBuffer;



		const char* lineVertexShader;
		const char* lineFragmentShader;
		std::unique_ptr<juce::OpenGLShaderProgram> lineShaderProgram;
	private:

		juce::Array<GLuint> lineVbo; // Vertex buffer object.
		juce::Array<GLuint> lineIbo; // Index buffer object.

		void newOpenGLContextCreated() override;


		void openGLContextClosing() override;


		inline juce::Line<float> translateLine(const juce::Line<float>& l, const juce::Point<float>& p)
		{
			return  juce::Line<float>(l.getStart().translated(p.x, p.y), l.getEnd().translated(p.x, p.y));

		}

		bool hasAlpha = false;

	protected:
		int numLines = 1;

	};


	// standardizes things like rendering curves, area under curve
	// great for visualizers (filter,distortion, lfo, etc.)
	class OpenGLFunctionVisualizer : public OpenGLLineRenderer
	{
	public:
		OpenGLFunctionVisualizer(GUI_Universals* universalsToUse, bool shouldHaveZeroLine = true, bool shouldRenderAreaUnderCurve = true, int numOfSamplePoints = 100);
		virtual ~OpenGLFunctionVisualizer();

		void setBipolar(bool newState);

		void resized() override;

		void setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line = NamedColorsIdentifier(), float topCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA, float botCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA);
		void setScaling(float newXScalingFactor, float newYScalingFactor = 0);
		void generateZeroLine();

		void initArrays(int numSamplePoints);

	protected:


		// Inherited via OpenGLComponent
		void generateVertexBuffer() override;
		virtual inline float calculateAlpha(float x, float y); // calculates alpha of area under curve based on height of function
		virtual inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX);
		virtual inline void newFrameInit(); // called at beginning of new frame calculation to grab any necessary values


		virtual void calculateZeroLine();

		bool isBipolar = false;
		bool hasZeroLine;
		OpenGLColor lineColor, zeroLine;

		int samplePoints = 2;

		int zeroLineStart;
		bool renderAreaUnderCurve = true;

		float topCurveAlpha = 0.5f, botCurveAlpha = 0.0f;


		float zeroY = 0.0f;
		float scalingX = 1.0f, scalingY = 1.0f, baseScalingX = 1.0f, baseScalingY = 1.0f;


		juce::NormalisableRange<float> xAxisMapping;

	};


	class OpenGLSpectrumVisualizer : public OpenGLComponent
	{
	public:
		OpenGLSpectrumVisualizer(GUI_Universals* universalsToUse, int numSamples, bool isStereo = true, bool useLogFrequency = true);
		virtual ~OpenGLSpectrumVisualizer();

		void setColor(const NamedColorsIdentifier& c);

	protected:
		void generateVertexBuffer() override;
		virtual inline float calculateFrequencyResponse(int channel, float freq);
		virtual inline void newFrameInit(); // called at beginning of new frame calculation to grab any necessary values

		bool stereo = true;
		bool logFrequency = true;
		int samplePoints = 2;
		OpenGLColor color;

		juce::Array<std::pair<float, float>> freqVals;

	};


	//================================================================================================================================================================================================


	//================================================================================================================================================================================================

	// dobule template is useless b/c fft only works with floats
	class SpectrogramController
	{
	public:
		SpectrogramController(dsp::SampleSource<float>* sourceToTrack, dsp::DSP_Universals<float>* lookupsToUse, int order = 8);
		~SpectrogramController() = default;

		void prepare(const juce::dsp::ProcessSpec& spec);
		void newBufferCreated();

		int getSize();

		void setBufferToPushTo(CircularBuffer<juce::Array<float>>* newBuffer);
		void setFreqValsToPushTo(juce::Array<float>* newBuffer);

	private:
		juce::dsp::FFT* fft = nullptr;
		juce::dsp::WindowingFunction<float> window;
		juce::Array<float> input, response;
		int inputPtr = 0;

		CircularBuffer<juce::Array<float>>* bufferToPushTo = nullptr;
		juce::Array<float>* freqValsToPushTo = nullptr;
		dsp::SampleSourceWeakReference<float> source;
		int numChannels, numSamples;
	};

	class OpenGLFFTVisualizer : public OpenGLSpectrumVisualizer
	{
	public:
		OpenGLFFTVisualizer(GUI_Universals* universalsToUse, SpectrogramController* controller, bool isStereo = false, bool useLogFrequency = true, int numFramesToAvg = 5);
		virtual ~OpenGLFFTVisualizer();


	protected:
		inline float calculateFrequencyResponse(int channel, float freq) override;
		inline void newFrameInit() override;


		CircularBuffer<juce::Array<float>> vals;

	private:
		int bin = 0;
	};

	class OpenGLFFTLineVisualizer : public OpenGLFunctionVisualizer
	{
	public:
		OpenGLFFTLineVisualizer(GUI_Universals* universalsToUse, SpectrogramController* controller, bool isStereo = false, bool useLogFrequency = true, int numFramesToAvg = 3);
		virtual ~OpenGLFFTLineVisualizer();


	protected:
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
		inline void newFrameInit() override;


		CircularBuffer<juce::Array<float>> vals;

	private:
	};

	class OpenGLSpectrogram : public OpenGLComponent
	{
	public:
		OpenGLSpectrogram(GUI_Universals* universalsToUse, SpectrogramController* specController, float sampleRate, float windowSizeInMilliseconds = 250);
		virtual ~OpenGLSpectrogram();

		void generateVertexBuffer() override;


		void pushBuckets(juce::Array<float> newBuckets);

		void setColor(const NamedColorsIdentifier& newColor);

	protected:
		SpectrogramController* controller;
		CircularBuffer<juce::Array<float>> bucketValues;
		int bucketsPerWindow;

		float windowSizeMS, sR;

		OpenGLColor c;

		float w, h;

	};

	//================================================================================================================================================================================================

	//liftime managed by processor so no references to non existant  opengl comps as listners
	class OpenGLControlValuesListener : public LinkableControl::SampleListener
	{
	public:
		OpenGLControlValuesListener(float windowSizeInMilliseconds, LinkableControl* controlToFollow);
		void pushNewSample(float smp) override;
		CircularBuffer<float> functionSamples;
	};

	// like function viewer except it accounts for changes in frame timing and standardizes pushing new samples
	// specific to values that get updated at control sample rate
	class OpenGLControlValuesOverTime : public OpenGLFunctionVisualizer
	{
	public:
		OpenGLControlValuesOverTime(LinkableControl* controlToFollow, GUI_Universals* universalsToUse, bool shouldHaveZeroLine = true, bool shouldRenderAreaUnderCurve = true);
		virtual ~OpenGLControlValuesOverTime();


	protected:

		void generateVertexBuffer() override;


		bool forceFullRange = false; // if true will stretch [0,1] from unipolar control to [-1,1]
		bool alphaFalloff = true;



	protected:
		LinkableControl* control = nullptr;
		OpenGLControlValuesListener* listener = nullptr;

	};

	//================================================================================================================================================================================================

	class OpenGLGrid : public OpenGLComponent, public GUI_Universals::Listener
	{
	public:

		OpenGLGrid(GUI_Universals* universalsToUse, int maxRows, int maxCols);
		~OpenGLGrid();

		void resized() override;


		void setXSpacing(float newSpacing);
		void setYSpacing(float newSpacing);
		void setThickness(float newThickness);

		void setColor(const NamedColorsIdentifier& newColor);

		void generateVertexBuffer() override;

	private:
		NamedColorsIdentifier color;
		float  xThickness, yThickness, xSpacing, ySpacing;

		float thickness;

		float r, g, b;

		void GUI_UniversalsChanged() override;

	};


	//================================================================================================================================================================================================

	class OpenGLRing : public OpenGLComponent, public GUI_Universals::Listener
	{
	public:

		OpenGLRing(GUI_Universals* universalsToUse, int numSamples = 16);
		virtual ~OpenGLRing();

		void generateVertexBuffer() override;

		void resized() override;

		void setRadius(float newRadius);
		void setThickness(float newThickness);
		void setFeatherRatio(float newFeatherRatio);
		void setCenter(juce::Point<float> newCenter, bool isGLCoord = true);


		void setColor(const NamedColorsIdentifier& newColor);


	protected:


		float r, g, b, a;



		float screenRadius = 1, screenThickness = 1;

		float featherRatio = 4;

		int samplePoints = 32;

		juce::Point<float> center;

		juce::Array<juce::Point<float>> angles;
		float thicknessX = 0.01, thicknessY = 0.01;
		float radX = 0.01, radY = 0.01;

		float featherOutX, radOutX, radInX, featherInX;
		float featherOutY, radOutY, radInY, featherInY;

		NamedColorsIdentifier color;


		// Inherited via Listener
		void GUI_UniversalsChanged() override;

	};


	class OpenGLCirclePlotter : public OpenGLComponent
	{
	public:
		OpenGLCirclePlotter(GUI_Universals* universalsToUse, int maxCircles = 250);
		virtual ~OpenGLCirclePlotter();



		void resized() override;

		void setRadius(float newRadius);
		void setFeatherRatio(float newRatio);

		float getScreenRadius();

		void generateCircleVerticies();
		float pointW = 0.1, pointH = 0.1;
		float featherW = 0.1, featherH = 0.1;
		VertexArray circleData;
	protected:




		void createShaders() override;
		void createUniforms() override;
		void createVertexAttributes() override;
		void removeVertexAttributes() override;



		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> featherRatioUniform;






		float screenRadius = 1;
		float featherRatio = BDSP_OPEN_GL_LINE_FEATHER_RATIO;



	};


	//================================================================================================================================================================================================


	//holds gl component as child for better repaint propogation and and resizing granularity
	template <class InternalClass>
	class OpenGlComponentWrapper : public Component, public juce::ComponentListener
	{
	public:

		template <class ... Types>
		OpenGlComponentWrapper(GUI_Universals* universalsToUse, Types...args)
			:Component(universalsToUse)
		{
			vis = std::make_unique<InternalClass>(universalsToUse, args...);
			vis->setScaling(1.0f, 1.0f);

			addAndMakeVisible(vis.get());

			extendedComponent->addComponentListener(this);
			//setOpaque(true);
		}

		~OpenGlComponentWrapper()
		{
			if (prevParent != nullptr)
			{
				prevParent->removeComponentListener(this);
			}
			extendedComponent->removeComponentListener(this);
		}

		void resized() override
		{
			//vis->setBoundsRelative(0.1, 0.1, 0.8, 0.8);
			vis->setBounds(getLocalBounds().reduced(universals->roundedRectangleCurve));
		}
		void paint(juce::Graphics& g) override
		{
			g.setColour(vis->getBackgroundColor());
			g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
		}

		void setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line = NamedColorsIdentifier(), float topCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA, float botCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA)
		{
			vis->setColor(c, line, topCurveOpacity, botCurveOpacity);
		}

		void setBackgroundColor(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& bkgdBehind)
		{
			vis->setBackgroundColor(bkgd, bkgdBehind);
		}
		InternalClass* getVis()
		{
			return vis.get();
		}


		// listen to itself to add itself as a listener to new parent component
		void componentParentHierarchyChanged(juce::Component& component) override
		{
			if (&component == extendedComponent)
			{
				if (prevParent != nullptr)
				{
					prevParent->removeComponentListener(this);
				}

				prevParent = getParentComponent();

				if (prevParent != nullptr)
				{
					prevParent->addComponentListener(this);
				}
			}

		}


		// when parent component changes visibilty follow suit
		void componentVisibilityChanged(juce::Component& component) override
		{
			if (&component != extendedComponent)
			{
				setVisible(component.isVisible());
				vis->setVisible(component.isVisible());
			}
		}


	protected:
		std::unique_ptr<InternalClass> vis;
		juce::Component* prevParent = nullptr;
	};

	class OpenGLCompositeComponent : public OpenGLComponent
	{
	public:

		OpenGLCompositeComponent(GUI_Universals* universalsToUse);
		~OpenGLCompositeComponent();

		void resized() override;

		void newOpenGLContextCreated() override;

		void renderOpenGL() override;


		void openGLContextClosing() override;

		virtual void onShaderCreation();



		void initSubClasses();



		juce::OwnedArray<OpenGLComponent> subClasses;
	protected:
		virtual void generateVertexBuffer() override;

	};




} // namespace bdsp
