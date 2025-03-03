#pragma once
namespace bdsp
{
	//liftime managed by processor so no references to non existant  opengl comps as listners
	class OpenGLControlValuesListener : public LinkableControl::SampleListener
	{
	public:
		OpenGLControlValuesListener(float windowSizeInMilliseconds, LinkableControl* controlToFollow)
			:LinkableControl::SampleListener(controlToFollow)
		{
			functionSamples.resize(windowSizeInMilliseconds * BDSP_CONTROL_SAMPLE_RATE / 1000.0f);
		}
		void pushNewSample(float smp) override
		{
			functionSamples.set(smp);
		}
		CircularBuffer<float> functionSamples;
	};

	// like function viewer except it accounts for changes in frame timing and standardizes pushing new samples
	// specific to values that get updated at control sample rate
	class OpenGLControlValuesOverTime : public OpenGLFunctionVisualizer
	{
	public:
		OpenGLControlValuesOverTime(LinkableControl* controlToFollow, GUI_Universals* universalsToUse, bool shouldHaveZeroLine = true, bool shouldRenderAreaUnderCurve = true)
			:OpenGLFunctionVisualizer(universalsToUse, shouldHaveZeroLine, shouldRenderAreaUnderCurve)
		{
			control = controlToFollow;

			for (auto l : control->getListeners())
			{
				auto cast = dynamic_cast<OpenGLControlValuesListener*>(l);
				if (l != nullptr)
				{
					listener = cast;
				}
			}

			jassert(listener != nullptr);

			initArrays(listener->functionSamples.getSize());

		}
		virtual ~OpenGLControlValuesOverTime()
		{
			universals->contextHolder->unregisterOpenGlRenderer(this);
		}


	protected:

		void generateVertexBuffer() override
		{
			float r, g, b, a;

			lineColor.getComponents(r, g, b, a);

			newFrameInit();

			setBipolar(control->getPolarity());


			//generateZeroLine();

			auto p = lineVertexBuffer.getFirst();

			for (int i = 0; i < listener->functionSamples.getSize(); ++i)
			{
				float prop = float(i) / (listener->functionSamples.getSize() - 1);
				float x = scalingX * (2 * prop - 1);
				float y;
				float smp = listener->functionSamples.get(i);


				if (forceFullRange)
				{
					y = isBipolar ? scalingY * smp : -zeroY * (2.0f * smp - 1.0f); // zero Y ensures line is visible on env follower with no input
				}
				else
				{
					y = scalingY * smp;
				}

				float alpha = alphaFalloff ? prop : 1.0f;

				if (renderAreaUnderCurve)
				{

					vertexBuffer.set(i,
						{
							x, y,
							r,g,b, alpha * calculateAlpha(x, y)
						} // AUC Top-i
					);
					vertexBuffer.set(samplePoints + i,
						{
							x, zeroY,
							r,g,b, alpha * calculateAlpha(x, zeroY)
						} // line AUC-i
					);
				}

				p->set(i,
					{
						x, y,
						//{r, g, b, 1.0f}
					alpha * r, alpha * g, alpha * b, 1.0f
					}
				);

			}

		}


		bool forceFullRange = false; // if true will stretch [0,1] from unipolar control to [-1,1]
		bool alphaFalloff = true;



	protected:
		LinkableControl* control = nullptr;
		OpenGLControlValuesListener* listener = nullptr;

	};
} //namespace bdsp