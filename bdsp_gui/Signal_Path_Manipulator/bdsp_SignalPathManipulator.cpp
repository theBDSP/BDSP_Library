#include "bdsp_SignalPathManipulator.h"

namespace bdsp
{
	SignalPathManipulator::SignalPathManipulator(GUI_Universals* universalsTouse, juce::AudioParameterChoice* param, bool isVertical, double nodeAspectRatio)
		:Component(universalsTouse),
		ChoiceComponentCore(param, this)
	{
		vertical = isVertical;
		aspectRatio = nodeAspectRatio;
		attach(param, universals->undoManager);
	}

	SignalPathManipulator::~SignalPathManipulator()
	{
	}

	void SignalPathManipulator::setEndpoints(const juce::Array<Shape>& inputShapes, const juce::Array<Shape>& outputShapes)
	{
		relativeNodeSlots.clear();
		relativeInputNodes.clear();

		inputNodes.clear();
		inputNodeShapes.clear();

		relativeOutputNodes.clear();
		outputNodes.clear();
		outputNodeShapes.clear();

		float length = (1.0 - (inputShapes.size() + 1) * border) / inputShapes.size();
		for (int i = 0; i < inputShapes.size(); ++i)
		{
			if (vertical)
			{
				relativeInputNodes.add(juce::Rectangle<float>((i + 1) * border + i * length, border, length, 0));
				//inputNodes.add(shrinkRectangleToInt(juce::Rectangle<float>(getWidth() * length, getWidth() * length).withPosition(relativeInputNodes.getLast())));
			}
			else
			{
				relativeInputNodes.add(juce::Rectangle<float>(border, (i + 1) * border + i * length, 0, length));
				//inputNodes.add(shrinkRectangleToInt(juce::Rectangle<float>(getHeight() * length, getHeight() * length).withPosition(relativeInputNodes.getLast())));
			}
			inputNodeShapes.add(inputShapes[i]);
		}

		length = (1.0 - (outputShapes.size() + 1) * border) / outputShapes.size();
		for (int i = 0; i < outputShapes.size(); ++i)
		{
			if (vertical)
			{
				relativeOutputNodes.add(juce::Rectangle<float>((i + 1) * border + i * length, 1 - border, length, 0));
				//outputNodes.add(shrinkRectangleToInt(juce::Rectangle<float>(getWidth() * length, getWidth() * length).withPosition(relativeOutputNodes.getLast())));
			}
			else
			{
				relativeOutputNodes.add(juce::Rectangle<float>(1 - border, (i + 1) * border + i * length, 0, length));
				//outputNodes.add(shrinkRectangleToInt(juce::Rectangle<float>(getHeight() * length, getHeight() * length).withPosition(relativeOutputNodes.getLast())));
			}
			outputNodeShapes.add(outputShapes[i]);
		}

		branches = juce::jmax(inputShapes.size(), outputShapes.size());

		resized();
	}

	void SignalPathManipulator::setEndpoints(const juce::StringArray& inputNames, const juce::StringArray& outputNames, const juce::Array<NamedColorsIdentifier>& inputColors, const juce::Array<NamedColorsIdentifier>& outputColors, juce::Array<juce::Path>& inputPaths, juce::Array<juce::Path>& outputPaths)
	{
		juce::Array<Shape> inputShapes, outputShapes;

		jassert(inputNames.size() == inputColors.size() && inputColors.size() == inputPaths.size());
		jassert(outputNames.size() == outputColors.size() && outputColors.size() == outputPaths.size());

		for (int i = 0; i < inputNames.size(); ++i)
		{
			juce::GlyphArrangement ga;
			juce::Path p;

			ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), inputNames[i], 0, 0);
			ga.createPath(p);

			scaleToFit(p, juce::Rectangle<float>(0, 0, 1, 1));
			scaleToFit(inputPaths.getReference(i), p.getBounds().withY(p.getBounds().getBottom()));

			p.addPath(inputPaths[i]);

			inputShapes.add(Shape(p, inputColors[i], shapeRelativePathBounds));
		}

		for (int i = 0; i < outputNames.size(); ++i)
		{
			juce::GlyphArrangement ga;
			juce::Path p;

			ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), outputNames[i], 0, 0);
			ga.createPath(p);

			scaleToFit(p, juce::Rectangle<float>(0, 0, 1, 1));
			scaleToFit(outputPaths.getReference(i), p.getBounds().withY(p.getBounds().getBottom()));
			p.addPath(outputPaths[i]);

			outputShapes.add(Shape(p, outputColors[i], shapeRelativePathBounds));
		}

		setEndpoints(inputShapes, outputShapes);
	}

	void SignalPathManipulator::setNodes(const juce::Array<Shape>& nodeShapes)
	{
		nodeComps.clear();

		for (int i = 0; i < nodeShapes.size(); ++i)
		{
			nodeComps.add(new NodeComponent(this, nodeShapes[i]));
			addAndMakeVisible(nodeComps.getLast());
			nodeComps.getLast()->setSlotIndex(i);
		}
		resized();
	}
	void SignalPathManipulator::setNodes(const juce::StringArray& names, const juce::Array<NamedColorsIdentifier>& colors,  juce::Array<juce::Path>& paths)
	{
		jassert(names.size() == colors.size() && colors.size() == paths.size());

		juce::Array<Shape> shapes;
		for (int i = 0; i < names.size(); ++i)
		{
			juce::GlyphArrangement ga;
			juce::Path p;

			ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), names[i], 0, 0);
			ga.createPath(p);

			scaleToFit(p, juce::Rectangle<float>(0, 0, 1, 1));
			scaleToFit(paths.getReference(i), p.getBounds().withY(p.getBounds().getBottom()));

			p.addPath(paths[i]);

			shapes.add(Shape(p, colors[i], shapeRelativePathBounds));
		}
		setNodes(shapes);
	}

	void SignalPathManipulator::setNodeStages(int n)
	{
		relativeNodeSlots.clear();
		nodeSlots.clear();
		stages = n;

		int inputs = inputNodes.isEmpty() ? 0 : 1;
		int outputs = outputNodes.isEmpty() ? 0 : 1;

		int total = inputs + outputs + stages;
		float length = (1.0 - (total + 1) * border) / total;
		float branchLength = (1.0 - (branches + 1) * border) / branches;

		for (int j = 0; j < branches; ++j)
		{
			for (int i = inputs; i < stages + inputs; ++i)
			{
				if (vertical)
				{
					relativeNodeSlots.add(juce::Rectangle<float>((j + 1) * border + j * branchLength, (i + 1) * border + i * length, branchLength, 0));
					//		nodeSlots.add(shrinkRectangleToInt(juce::Rectangle<float>(getWidth() * length, getWidth() * length).withPosition(relativeNodeSlots.getLast())));
				}
				else
				{
					relativeNodeSlots.add(juce::Rectangle<float>((i + 1) * border + i * length, (j + 1) * border + j * branchLength, 0, branchLength));
					//nodeSlots.add(shrinkRectangleToInt(juce::Rectangle<float>(getHeight() * length, getHeight() * length).withPosition(relativeNodeSlots.getLast())));
				}
			}
		}
		resized();
	}

	void SignalPathManipulator::resized()
	{
		for (int i = 0; i < relativeInputNodes.size(); ++i)
		{
			if (vertical)
			{
				inputNodes.set(i, getLocalBounds().getProportion(relativeInputNodes[i]).withSize(getWidth() * relativeInputNodes[i].getWidth() * aspectRatio, getWidth() * relativeInputNodes[i].getWidth()));
			}
			else
			{
				inputNodes.set(i, getLocalBounds().getProportion(relativeInputNodes[i]).withSize(getHeight() * relativeInputNodes[i].getHeight(), getHeight() * relativeInputNodes[i].getHeight() * aspectRatio));
			}
		}

		for (int i = 0; i < relativeOutputNodes.size(); ++i)
		{
			if (vertical)
			{
				auto length = getWidth() * relativeOutputNodes[i].getWidth();
				outputNodes.set(i, getLocalBounds().getProportion(relativeOutputNodes[i]).withSize(length, length * aspectRatio).translated(0, -length * aspectRatio));
			}
			else
			{
				auto length = getHeight() * relativeOutputNodes[i].getHeight();
				outputNodes.set(i, getLocalBounds().getProportion(relativeOutputNodes[i]).withSize(length * aspectRatio, length).translated(-length * aspectRatio, 0));
			}
		}

		for (int i = 0; i < relativeNodeSlots.size(); ++i)
		{
			if (vertical)
			{
				nodeSlots.set(i, getLocalBounds().getProportion(relativeNodeSlots[i]).withSize(getWidth() * relativeNodeSlots[i].getWidth(), getWidth() * relativeNodeSlots[i].getWidth() * aspectRatio));
			}
			else
			{
				nodeSlots.set(i, getLocalBounds().getProportion(relativeNodeSlots[i]).withSize(getHeight() * relativeNodeSlots[i].getHeight() * aspectRatio, getHeight() * relativeNodeSlots[i].getHeight()));
			}
		}

		for (int i = 0; i < nodeComps.size(); ++i)
		{
			if (!nodeComps[i]->isBeingDragged())
			{
				nodeComps[i]->setBounds(nodeSlots[nodeComps[i]->getSlotIndex()]);
			}
		}

	}

	void SignalPathManipulator::paint(juce::Graphics& g)
	{
		g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);

		paintNodeSlots(g);

	}

	void SignalPathManipulator::paintNodeSlots(juce::Graphics& g)
	{
		for (int i = 0; i < nodeSlots.size(); ++i)
		{
			g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
			g.fillRoundedRectangle(nodeSlots[i].toFloat(), universals->roundedRectangleCurve);

			g.setColour(getColor(BDSP_COLOR_LIGHT));
			g.drawRoundedRectangle(nodeSlots[i].toFloat(), universals->roundedRectangleCurve, 1);
		}

		for (int i = 0; i < inputNodes.size(); ++i)
		{
			g.setColour(getColor(inputNodeShapes[i].fillOn));
			g.fillRoundedRectangle(inputNodes[i].toFloat(), universals->roundedRectangleCurve);

			g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
			scaleToFit(inputNodeShapes.getReference(i).path, inputNodes[i].getProportion(inputNodeShapes[i].relativeBounds));
			g.fillPath(inputNodeShapes.getReference(i).path);
		}
		for (int i = 0; i < outputNodes.size(); ++i)
		{
			g.setColour(getColor(outputNodeShapes[i].fillOn));
			g.fillRoundedRectangle(outputNodes[i].toFloat(), universals->roundedRectangleCurve);

			g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
			juce::Path p = outputNodeShapes[i].path;
			scaleToFit(p, outputNodes[i].getProportion(outputNodeShapes[i].relativeBounds));
			g.fillPath(p);
		}
	}

	void SignalPathManipulator::interpretParameterIndex(int idx)
	{
	}

	void SignalPathManipulator::nodeMoved(NodeComponent* compThatMoved, bool shouldUpdateParameter)
	{

		auto center = compThatMoved->getBounds().toFloat().getCentre();

		float minDistance = nodeSlots.getFirst().toFloat().getCentre().getDistanceSquaredFrom(center);
		int minIdx = 0;
		for (int i = 1; i < nodeSlots.size(); ++i)
		{
			float dist = nodeSlots[i].toFloat().getCentre().getDistanceSquaredFrom(center);
			if (dist < minDistance)
			{
				minDistance = dist;
				minIdx = i;
			}
		}
		compThatMoved->setSlotIndex(minIdx);



		int prevOccupant = -1;
		for (int i = 0; i < nodeComps.size(); ++i)
		{
			if (nodeComps[i] != compThatMoved)
			{
				if (nodeComps[i]->getSlotIndex() == minIdx)
				{
					prevOccupant = i;
					break;
				}
			}
		}

		if (prevOccupant != -1) // collision, have to move comp that used to be here
		{
			int branch = minIdx / branches;
			//int stage = minIdx % branches;

			int idx = -1;
			for (int i = 0; i < stages; ++i) // same branch
			{
				int curr = stages * branch + i;
				if (!isSlotOccupied(curr))
				{
					idx = curr;
					break;
				}
			}

			if (idx == -1)
			{
				for (int j = 0; j < branches; ++j)
				{
					if (j != branch)
					{
						for (int i = 0; i < stages; ++i) // find first open slot on this branch
						{
							int curr = stages * j + i;
							if (!isSlotOccupied(curr))
							{
								idx = curr;
								break;
							}
						}
					}
				}
			}

			nodeComps[prevOccupant]->setSlotIndex(idx);
		}



		if (shouldUpdateParameter)
		{
			updateParameter();
		}
		else // updating parameter should eventually call resized to snap nodes in place
		{
			resized();
		}
	}

	void SignalPathManipulator::snapNode(NodeComponent* compToSnap)
	{
		resized();
	}

	void SignalPathManipulator::updateParameter()
	{
		selectItem(calculateParameterIndex());
	}

	void SignalPathManipulator::parameterChanged(int idx)
	{
		interpretParameterIndex(idx);
	}

	bool SignalPathManipulator::isSlotOccupied(int slotIndex)
	{
		bool out = false;
		for (auto c : nodeComps)
		{
			if (c->getSlotIndex() == slotIndex)
			{
				out = true;
				break;
			}
		}
		return out;
	}



	//================================================================================================================================================================================================


	SignalPathManipulator::NodeComponent::NodeComponent(SignalPathManipulator* p, const Shape& nodeShape)
		:Component(p->universals)
	{
		parent = p;
		s = nodeShape;
		dragger = &parent->dragger;
	}
	SignalPathManipulator::NodeComponent::~NodeComponent()
	{
	}

	void SignalPathManipulator::NodeComponent::setSlotIndex(int i)
	{
		slotIndex = i;
	}
	int SignalPathManipulator::NodeComponent::getSlotIndex() const
	{
		return slotIndex;
	}
	bool SignalPathManipulator::NodeComponent::isBeingDragged() const
	{
		return beingDragged;
	}
	Shape* SignalPathManipulator::NodeComponent::getShape()
	{
		return &s;
	}
	void SignalPathManipulator::NodeComponent::paint(juce::Graphics& g)
	{
		g.setColour(getColor(s.fillOn));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);

		g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		scaleToFit(s.path, getLocalBounds().getProportion(s.relativeBounds));
		g.fillPath(s.path);
	}
	void SignalPathManipulator::NodeComponent::sendParentNewLocation()
	{
		parent->nodeMoved(this);
	}
	void SignalPathManipulator::NodeComponent::mouseDown(const juce::MouseEvent& e)
	{
		dragger->startDraggingComponent(this, e);
		beingDragged = true;
	}
	void SignalPathManipulator::NodeComponent::mouseDrag(const juce::MouseEvent& e)
	{
		auto parentSpace = parent->getLocalPoint(this, e.getPosition());
		auto bounded = parent->getLocalBounds().getConstrainedPoint(parentSpace);
		auto newPosition = getLocalPoint(parent, bounded);
		juce::MouseEvent boundedE = e.withNewPosition(newPosition);
		dragger->dragComponent(this, boundedE, nullptr);
		sendParentNewLocation();
	}
	void SignalPathManipulator::NodeComponent::mouseUp(const juce::MouseEvent& e)
	{
		beingDragged = false;
		parent->snapNode(this);
	}

	//================================================================================================================================================================================================



} // namespace bdsp
