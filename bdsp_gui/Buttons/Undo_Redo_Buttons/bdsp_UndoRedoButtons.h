#pragma once


namespace bdsp
{



	class UndoRedoButtons : public juce::Component, public GUI_Universals::Listener, public juce::ChangeListener
	{
	public:
		UndoRedoButtons(GUI_Universals* universalsToUse, const NamedColorsIdentifier& buttonColor);
		~UndoRedoButtons() override;
		void resized() override;
	protected:
		std::unique_ptr<MultiShapeButton> undo, redo;
		NamedColorsIdentifier c;
		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;

		// Inherited via ChangeListener
		virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;

		;
	};
} // namespace bdsp