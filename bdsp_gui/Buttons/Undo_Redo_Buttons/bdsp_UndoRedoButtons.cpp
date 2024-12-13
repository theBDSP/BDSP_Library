#include "bdsp_UndoRedoButtons.h"
namespace bdsp
{

	UndoRedoButtons::UndoRedoButtons(GUI_Universals* universalsToUse, const NamedColorsIdentifier& buttonColor)
		:GUI_Universals::Listener(universalsToUse)
	{
		;
		c = buttonColor;
		undo = std::make_unique<MultiShapeButton>(universalsPTR);
		redo = std::make_unique<MultiShapeButton>(universalsPTR);

		addAndMakeVisible(undo.get());
		addAndMakeVisible(redo.get());

		universalsPTR->undoManager->addChangeListener(this);


		undo->onClick = [=]()
		{
			universalsPTR->undoManager->undo();
		};
	
		
		redo->onClick = [=]()
		{
			universalsPTR->undoManager->redo();
		};
	


		GUI_UniversalsChanged();
		changeListenerCallback(universalsPTR->undoManager);
	}
	UndoRedoButtons::~UndoRedoButtons()
	{
		universalsPTR->undoManager->removeChangeListener(this);
	}
	void UndoRedoButtons::resized()
	{
		undo->setBoundsRelative(0, 0, 0.5, 1);
		redo->setBoundsRelative(0.5, 0, 0.5, 1);
	}

	void UndoRedoButtons::GUI_UniversalsChanged()
	{
		static const unsigned char undoData[] = { 110,109,221,226,61,196,210,162,100,196,98,245,50,62,196,188,162,100,196,53,130,62,196,104,138,100,196,46,197,62,196,174,91,100,196,108,25,136,62,196,152,30,100,196,98,2,46,62,196,64,87,100,196,118,187,61,196,176,94,100,196,4,89,61,196,36,46,100,196,98,
82,225,60,196,29,243,99,196,181,156,60,196,206,114,99,196,33,174,60,196,125,238,98,196,98,140,191,60,196,45,106,98,196,252,34,61,196,233,255,97,196,225,165,61,196,224,229,97,196,98,197,40,62,196,214,203,97,196,72,173,62,196,235,7,98,196,3,240,62,196,
127,123,98,196,108,94,57,63,196,33,81,98,196,98,169,228,62,196,106,190,97,196,136,59,62,196,207,113,97,196,96,149,61,196,220,146,97,196,98,56,239,60,196,233,179,97,196,60,112,60,196,106,59,98,196,31,90,60,196,97,227,98,196,98,3,68,60,196,87,139,99,196,
146,155,60,196,54,47,100,196,131,51,61,196,36,122,100,196,98,127,89,61,196,224,140,100,196,119,129,61,196,236,152,100,196,225,169,61,196,193,158,100,196,98,211,188,61,196,124,161,100,196,221,207,61,196,215,162,100,196,221,226,61,196,210,162,100,196,99,
109,167,63,63,196,23,154,100,196,108,65,152,63,196,220,83,99,196,108,253,81,62,196,109,172,99,196,108,167,63,63,196,23,154,100,196,99,101,0,0 };

		juce::Path path;
		path.loadPathFromData(undoData, sizeof(undoData));

		undo->clearShapes();
		undo->addShapes(Shape(path, c, juce::Rectangle<float>(0, 0, 1, 1)));


		static const unsigned char redoData[] = { 110,109,220,231,57,196,210,162,100,196,98,196,151,57,196,194,162,100,196,132,72,57,196,103,138,100,196,139,5,57,196,174,91,100,196,108,161,66,57,196,152,30,100,196,98,183,156,57,196,64,87,100,196,68,15,58,196,176,94,100,196,182,113,58,196,35,46,100,196,
98,104,233,58,196,28,243,99,196,5,46,59,196,206,114,99,196,153,28,59,196,125,238,98,196,98,45,11,59,196,45,106,98,196,190,167,58,196,232,255,97,196,217,36,58,196,223,229,97,196,98,244,161,57,196,214,203,97,196,113,29,57,196,235,7,98,196,183,218,56,196,
126,123,98,196,108,92,145,56,196,32,81,98,196,98,17,230,56,196,105,190,97,196,50,143,57,196,206,113,97,196,90,53,58,196,219,146,97,196,98,130,219,58,196,232,179,97,196,126,90,59,196,105,59,98,196,154,112,59,196,95,227,98,196,98,183,134,59,196,85,139,
99,196,40,47,59,196,53,47,100,196,55,151,58,196,35,122,100,196,98,59,113,58,196,222,140,100,196,67,73,58,196,235,152,100,196,217,32,58,196,191,158,100,196,98,231,13,58,196,123,161,100,196,221,250,57,196,214,162,100,196,221,231,57,196,209,162,100,196,
99,109,18,139,56,196,23,154,100,196,108,120,50,56,196,220,83,99,196,108,188,120,57,196,110,172,99,196,99,101,0,0 };

		path.clear();
		path.loadPathFromData(redoData, sizeof(redoData));
		redo->clearShapes();
		redo->addShapes(Shape(path, c, juce::Rectangle<float>(0, 0, 1, 1)));


	}

	void UndoRedoButtons::changeListenerCallback(juce::ChangeBroadcaster* source)
	{
		undo->setEnabled(universalsPTR->undoManager->canUndo());
		redo->setEnabled(universalsPTR->undoManager->canRedo());


		undo->setHintBarText( juce::String(undo->isEnabled()? "Undo": "Undo (nothing to undo)"));
		redo->setHintBarText(juce::String(redo->isEnabled()? "Redo": "Redo (nothing to redo)"));


	}
} // namespace bdsp
