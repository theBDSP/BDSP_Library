#pragma once 


namespace bdsp
{

	class Caret : public juce::CaretComponent
	{
	public:

		Caret(juce::Component* keyFocusOwner)
			:juce::CaretComponent(keyFocusOwner)
		{

		}

		/** Destructor. */
		~Caret() override
		{

		}

		void setCaretPosition(const juce::Rectangle<int>& characterArea) override
		{
			juce::CaretComponent::setCaretPosition(characterArea);
			floatBounds = characterArea.toFloat().withWidth(characterArea.getHeight() * 0.05);
			setBounds(shrinkRectangleToInt(floatBounds.withWidth(ceil(floatBounds.getWidth())).withX(floor(floatBounds.getX()))));
		}

		void paint(juce::Graphics& g) override
		{
			juce::Path p;
			p.addRectangle(floatBounds.withPosition(0, 0));
			g.setColour(findColour(juce::CaretComponent::caretColourId, true));
			g.fillPath(p);
		}

	private:
		juce::Rectangle<float> floatBounds;
	};


} // namespace bdsp