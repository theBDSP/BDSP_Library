#pragma once



namespace bdsp
{
	class SignalPathManipulator : public Component, public ChoiceComponentCore
	{
	public:
		SignalPathManipulator(GUI_Universals* universalsTouse, juce::AudioParameterChoice* param, bool isVertical = true, double nodeAspectRatio = 2.0);
		virtual ~SignalPathManipulator();

		void setEndpoints(const juce::Array<Shape>& inputShapes, const juce::Array<Shape>& outputShapes);
		void setEndpoints(const juce::StringArray& inputNames, const juce::StringArray& outputNames, const juce::Array<NamedColorsIdentifier>& inputColors, const juce::Array<NamedColorsIdentifier>& outputColors,  juce::Array<juce::Path>& inputPaths,  juce::Array<juce::Path>& outputPaths);
		void setNodes(const juce::Array<Shape>& nodeShapes);
		void setNodes(const juce::StringArray& names, const juce::Array<NamedColorsIdentifier>& colors,  juce::Array<juce::Path>& paths);
		void setNodeStages(int n);

		void resized() override;
		void paint(juce::Graphics& g) override;
		void paintNodeSlots(juce::Graphics& g);
		juce::Array<juce::Rectangle<float>> relativeNodeSlots, relativeInputNodes, relativeOutputNodes;
		juce::Array<juce::Rectangle<int>> nodeSlots, inputNodes, outputNodes;
		juce::Array<Shape> inputNodeShapes, outputNodeShapes;

	protected:

		virtual int calculateParameterIndex() = 0;
		virtual void interpretParameterIndex(int idx);

		class NodeComponent : public Component
		{
		public:
			NodeComponent(SignalPathManipulator* p, const Shape& nodeShape);
			virtual ~NodeComponent();

			void setSlotIndex(int i);
			int getSlotIndex() const;

			bool isBeingDragged() const;

			Shape* getShape();
		private:
			void paint(juce::Graphics& g) override;

			void sendParentNewLocation();

			void mouseDown(const juce::MouseEvent& e) override;
			void mouseDrag(const juce::MouseEvent& e) override;
			void mouseUp(const juce::MouseEvent& e) override;

			juce::ComponentDragger* dragger;
			SignalPathManipulator* parent;
			Shape s;

			int slotIndex = 0;
			bool beingDragged = false;

		};

		void nodeMoved(NodeComponent* compThatMoved, bool shouldUpdateParameter = true);
		void snapNode(NodeComponent* compToSnap);
		void updateParameter();

		void parameterChanged(int idx) override;



		//================================================================================================================================================================================================

		bool isSlotOccupied(int slotIndex);

		//================================================================================================================================================================================================



		juce::OwnedArray<NodeComponent> nodeComps;

		juce::ComponentDragger dragger;

		bool vertical = true;

		int stages = 1; // node slots per signal branch (aka how many things can be routed to each output)
		int branches = 1;
		const float border = 0.1;

		const juce::Rectangle<float> shapeRelativePathBounds = juce::Rectangle<float>(0.1, 0.1, 0.8, 0.8);

		double aspectRatio;

	};
} // namespace bdsp
