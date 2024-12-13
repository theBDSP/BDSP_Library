#include "bdsp_LinkableControl.h"
//=====================================================================================================================
namespace bdsp
{
	void LinkableControlComponent::setControlObject(LinkableControl* newControlObject)
	{
		control = newControlObject;
	}



	juce::String LinkableControlComponent::getDisplayName()
	{
		return displayName;
	}




	void LinkableControlComponent::setColor(const NamedColorsIdentifier& color, const NamedColorsIdentifier& caretColor)
	{
		c = color;
		caret = caretColor;
		colorSet();

		for (auto l : colorListeners)
		{
			l->colorChanged(this);
		}
	}

	NamedColorsIdentifier LinkableControlComponent::getColorID()
	{
		return c;
	}


	const juce::Identifier& LinkableControlComponent::getID()
	{
		return ID;
	}



	//=====================================================================================================================

	LinkableControlComponent::DragComp::DragComp(GUI_Universals* universalsToUse, LinkableControlComponent* p)
		:Component(universalsToUse)
	{
		parent = p;
		;



		setFontIndex(2);
	}


	void LinkableControlComponent::DragComp::mouseEnter(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::DraggingHandCursor);
	}

	void LinkableControlComponent::DragComp::mouseExit(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}

	void LinkableControlComponent::DragComp::mouseDrag(const juce::MouseEvent& e)
	{
		auto scale = 1.75;
		auto bounds = getLocalBounds().withSize(getWidth() * scale, getHeight() * scale);
		jassert(bounds.getAspectRatio() == 1);

		juce::GlyphArrangement ga;
		ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), parent->displayName, 0, 0);
		juce::Path p;
		ga.createPath(p);
		auto shrink = 0.99;// / juce::MathConstants<float>::sqrt2;
		scaleToFit(p, bounds.withSizeKeepingCentre(bounds.getWidth() * shrink, bounds.getHeight() * shrink));

		juce::Image img(juce::Image::PixelFormat::ARGB, bounds.getWidth(), bounds.getHeight(), true);
		juce::Graphics g(img);


		g.setColour(getColor(parent->c));
		g.fillPath(p);

		//g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		//g.strokePath(p, juce::PathStrokeType(p.getBounds().getWidth() / 30.0));


		auto off = bounds.getWidth();
		auto offset = juce::Point<int>(-off, -bounds.getHeight() / 2);
		startDragging(parent->ID.toString(), this, juce::ScaledImage(img), true, &offset);
	}

	void LinkableControlComponent::DragComp::dragOperationStarted(const juce::DragAndDropTarget::SourceDetails&)
	{
		repaint();
	}

	void LinkableControlComponent::DragComp::dragOperationEnded(const juce::DragAndDropTarget::SourceDetails&)
	{
		repaint();
	}

	void LinkableControlComponent::DragComp::paint(juce::Graphics& g)
	{


		g.setColour(getColor(parent->c));

		if (isDragAndDropActive())
		{
			scaleToFit(universals->commonPaths.dragHandleFilled, getLocalBounds());
			g.fillPath(universals->commonPaths.dragHandleFilled);
		}
		else
		{
			scaleToFit(universals->commonPaths.dragHandleEmpty, getLocalBounds());
			g.fillPath(universals->commonPaths.dragHandleEmpty);
		}
	}


	//================================================================================================================================================================================================


	void LinkableControl::pushToListeners()
	{
		for (auto l : listeners)
		{
			l->pushNewSample(value);
		}
	}

	void LinkableControl::addSampleListener(SampleListener* listenerToAdd)
	{
		listeners.addIfNotAlreadyThere(listenerToAdd);
	}

	void LinkableControl::removeSampleListener(SampleListener* listenerToRemove)
	{
		listeners.removeAllInstancesOf(listenerToRemove);
	}

	juce::Array<LinkableControl::SampleListener*>& LinkableControl::getListeners()
	{
		return listeners;
	}


	//================================================================================================================================================================================================


	ControlTransferPopupMenu::ControlTransferPopupMenu(GUI_Universals* universalsToUse)
		:PopupMenu(nullptr, universalsToUse)
	{
		List.setRowsAndCols(BDSP_NUMBER_OF_LINKABLE_CONTROLS / 4, 4);

		List.onSelect = [=](int i)
		{
			auto other = universals->controlComps->controls[i];
			auto id = sourceComponent->getID().toString().upToLastOccurrenceOf("ID", false, true);
			auto otherID = other->getID().toString().upToLastOccurrenceOf("ID", false, true);


			universals->undoManager->beginNewTransaction();
			if (sourceComponent->name.compare(sourceComponent->defaultName) != 0 && other->name.compare(other->defaultName) == 0) // if source has unique name and target does not
			{
				other->setName(sourceComponent->name);
			}
			sourceComponent->setName(sourceComponent->defaultName);

			for (int j = sourceComponent->links.size() - 1; j >= 0; j--) // gotta go backwards bc links get removes from array on reassignment
			{
				auto p = sourceComponent->links[j];
				auto newID = p->getParameterID().replace(id, otherID);
				auto otherParam = universals->APVTS->getParameter(newID);
				otherParam->setValueNotifyingHost(p->getValue());
				p->setValueNotifyingHost(p->getDefaultValue());
			}

			auto macroCast = dynamic_cast<MacroComponent*>(sourceComponent);

			if (macroCast != nullptr)
			{
				macroCast->getSlider()->setValue(0, juce::sendNotification);
			}

		};

		//onShow = [=]()
		//{
		//	for (int i = 0; i < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++i)
		//	{
		//		List.List[i]->setEnabled(sourceComponent->getIndex() != i);
		//	}
		//};
	}
	void ControlTransferPopupMenu::createPopup(LinkableControlComponent* source)
	{
		sourceComponent = source;
		addTitle("Transfer All Modulation From " + sourceComponent->getDisplayName() + " To:", 0.75);

		List.clearItems();
		for (int i = 0; i < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++i)
		{
			List.addItem(universals->controlComps->controls[i]->getDisplayName(), i);
			List.List.getLast()->setEnabled(i != sourceComponent->getIndex());
		}

		List.highlightItem(sourceComponent->getIndex() == 0 ? 1 : 0);


		auto sourceBounds = sourceComponent->transferComp.getBoundsRelativeToDesktopManager();
		setBoundsContained(juce::Rectangle<int>(sourceBounds.getCentreX() - universals->sliderPopupWidth, sourceBounds.getBottom(), 2 * universals->sliderPopupWidth, List.getIdealHeight(universals->sliderPopupWidth * 2)), 0);
		List.setColorSchemeClassic(BDSP_COLOR_KNOB, NamedColorsIdentifier(), sourceComponent->getColorID(), NamedColorsIdentifier(BDSP_COLOR_PURE_BLACK));
		show();
	}




	//================================================================================================================================================================================================




	LinkableControlComponent::TransferComp::TransferComp(GUI_Universals* universalsToUse, LinkableControlComponent* p)
		:Button(universalsToUse)
	{
		parent = p;
		onClick = [=]()
		{
			universals->controlPopup->createPopup(parent);
		};

		setEnabled(false);


	}

	void LinkableControlComponent::TransferComp::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		g.setColour(getColor(parent->c).withMultipliedAlpha(isEnabled() ? 1.0f : universals->disabledAlpha));

		scaleToFit(universals->commonPaths.transferIcon, getLocalBounds());
		g.fillPath(universals->commonPaths.transferIcon);

	}
	void LinkableControlComponent::TransferComp::enablementChanged()
	{
		setHintBarText("Transfers all of " + parent->defaultName + "'s modulations to another modulator" + juce::String(isEnabled() ? "" : " (No Modulation to transfer)"));
		repaint();
	}
}	// namnepace bdsp
