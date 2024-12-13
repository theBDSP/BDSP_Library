#pragma once 


namespace bdsp
{
	class DragHandle : public Component
	{
	public:
		DragHandle(GUI_Universals* universalsToUse, juce::Component* componentToDrag, Shape shapeToUse = Shape());
		virtual ~DragHandle() = default;
		void paint(juce::Graphics& g) override;
		void setLegalBounds(juce::Rectangle<int> newLegalBounds);
		void resized() override;

		void mouseDown(const juce::MouseEvent& e) override;
		void mouseDrag(const juce::MouseEvent& e) override;
		void mouseUp(const juce::MouseEvent& e) override;


		void setNewShape(Shape newShape);
		Shape& getShape();

		void setMovementSnapping(bool shouldSnap, bool verticalSnap = true);

		std::function<void(const juce::MouseEvent&)> onDragEnd;
	protected:
		juce::ComponentDragger dragger;
		juce::Component* dragee;
		Shape shape;

		bool snapMovement = true;
		bool snapVertically = true;


		juce::Rectangle<int> legalBounds;

	};
} // namespace bdsp