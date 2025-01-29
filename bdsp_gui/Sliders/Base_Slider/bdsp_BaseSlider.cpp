#include "bdsp_BaseSlider.h"


namespace bdsp
{
	BaseSlider::BaseSlider(GUI_Universals* universalsToUse, const juce::String& baseName)
		:ComponentCore(this, universalsToUse),
		GUI_Universals::Listener(universalsToUse)
	{
		addListener(this);
		getValueFunction = [=]()
		{
			return getValue();
		};

		snapToValueFunction = [=](double val, juce::Slider::DragMode mode)
		{
			double out = getNormalisableRange().snapToLegalValue(val);

			if (!juce::approximatelyEqual(val, out))
			{


				auto interval = getInterval();
				if (interval == 0)
				{
					out = val;
				}
				else
				{
					auto rel = val - getMinimum(); // in case minimum is not an integer
					auto mod = fmod(rel, interval);

					if (mod < interval * 0.5)
					{
						out = val - mod;
					}
					else
					{
						out = val + interval - mod;
					}
				}
			}

			return juce::jlimit(getMinimum(), getMaximum(), out);
		};

		;

		setNamesAndIDs(baseName);
		setKnobPlacement(Below);


		setVelocityModeParameters(getVelocitySensitivity(), getVelocityThreshold(), getVelocityOffset(), false, juce::ModifierKeys::noModifiers);// need to unbind ctrl key from juce level of switching to velocity (fine control) mode

		std::function<void(bool)> Func = [=](bool state)
		{
			if (mainLabel != nullptr)
			{

				if (mainLabel->getCurrentTextEditor() == nullptr)
				{
					updateLabel();
					if (!mainLabel->getHasNoTitle())
					{
						mainLabel->setVisible(state);
						mainLabel->getTitle().setVisible(!state);
					}

				}
				mainLabel->desiredHoverState = state;
			}
		};

		setHoverFunc(Func);

		popup = std::make_unique<SliderPopupMenu>(this, universalsToUse);



	}

	BaseSlider::BaseSlider(const BaseSlider& other)
		: BaseSlider(other.universals, other.getName())
	{
		setValue(other.getValue());
	}
	BaseSlider::~BaseSlider()
	{

		if (bdspParam != nullptr)
		{
			bdspParam->removeLambdaListener(this);
		}
	}



	void BaseSlider::setType(Type newType)
	{
		type = newType;

		if (newType == CenterZero)
		{
			catchValues.clear();
			catchValues.add(0.5);
		}
	}

	BaseSlider::Type BaseSlider::getType()
	{
		return type;
	}

	void BaseSlider::setKnobPlacement(KnobPlacement p)
	{
		place = p;

		if (p == Above || p == Below)
		{
			setSliderStyle(juce::Slider::LinearBar);
		}
		else
		{
			setSliderStyle(juce::Slider::LinearBarVertical);
		}
	}

	BaseSlider::KnobPlacement BaseSlider::getKnobPlacement()
	{
		return place;
	}

	void BaseSlider::setUnboundedMouseDrag(bool state)
	{
		unboundedMouseDrag = state;
	}

	juce::Rectangle<int> BaseSlider::getBoundsToIncludeLabel()
	{
		juce::Rectangle<int> out;
		if (mainLabel == nullptr)
		{
			out = getBounds();
		}
		else
		{
			out = getBounds().getUnion(mainLabel->getBounds());
		}
		return out;
	}



	void BaseSlider::setNamesAndIDs(const juce::String& baseName)
	{
		auto noSpaces = baseName.removeCharacters(" ");
		setName(baseName);
		setComponentID(noSpaces + "ID");
		if (mainLabel != nullptr)
		{
			mainLabel->setName(baseName + " Label");
			mainLabel->setComponentID(noSpaces + "LabelID");
		}
	}

	void BaseSlider::showTextEditor()
	{
		if (mainLabel == nullptr)
		{
			showTextBox();
		}
		else
		{
			mainLabel->setHovering(true);
			mainLabel->setVisible(true);
			mainLabel->showEditor();
		}
	}

	void BaseSlider::attach(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterID)
	{
		param = stateToUse.getParameter(parameterID);
		bdspParam = dynamic_cast<Parameter*>(param);

		jassert(param != nullptr);

		attachment = std::make_unique<SliderAttachment>(stateToUse, parameterID, *this);
		if (onAttach.operator bool())
		{
			onAttach();
		}
		attachLabel();

		setDoubleClickReturnValue(false, getDefaultValue(), juce::ModifierKeys::noModifiers);

		if (bdspParam != nullptr)
		{
			bdspParam->addLambdaListener(this);
		}
	}
	juce::RangedAudioParameter* BaseSlider::getParameter()
	{
		return param;
	}
	void BaseSlider::attachLabel()
	{
		if (mainLabel != nullptr)
		{
			mainLabel->setMaxText(getTextFromValue(getRange().getEnd()));
		}
		updateLabel();
	}
	void BaseSlider::updateLabel()
	{

		if (mainLabel != nullptr)
		{
			if (textFromValueFunction.operator bool())
			{
				mainLabel->setText(getTextFromValue(getValue()), juce::dontSendNotification);
			}

		}

	}


	bool BaseSlider::hasLabel()
	{
		if (mainLabel == nullptr)
		{
			return false;
		}
		else
		{
			return mainLabel->isVisible() || mainLabel->getTitle().isVisible();
		}
	}

	double BaseSlider::getDefaultValue()
	{
		return getNormalisableRange().convertFrom0to1(param->getDefaultValue());
	}
	double BaseSlider::getActualValue()
	{
		//JUCE_ASSERT_MESSAGE_THREAD
		return getValueFunction();
	}



	double BaseSlider::snapValue(double inputVal, juce::Slider::DragMode mode)
	{
		//if (mode == absoluteDrag)
		//{


		//	return proportionOfLengthToValue(v);

		//}
		return snapToValueFunction(inputVal, mode);
	}

	void BaseSlider::addCatchValues(const juce::Array<double>& newValues, bool isNormalized)
	{
		if (isNormalized)
		{
			catchValues.addArray(newValues);
		}
		else
		{
			auto range = getNormalisableRange();
			for (auto d : newValues)
			{
				catchValues.add(valueToProportionOfLength(d));
			}
		}
		catchValues.sort();
	}






	void BaseSlider::enablementChanged()
	{
		juce::Slider::repaint();
	}

	void BaseSlider::visibilityChanged()
	{
		if (mainLabel != nullptr)
		{
			mainLabel->getTitle().setVisible(isVisible());
		}
	}



	void BaseSlider::mouseDown(const juce::MouseEvent& e)
	{
		if (isEnabled())
		{


			if (e.mods.isPopupMenu())
			{
				auto desktopBounds = getBoundsRelativeToDesktopManager();
				auto h = popup->List.getIdealHeight(universals->sliderPopupWidth);
				auto popupBounds = juce::Rectangle<int>(desktopBounds.getRight(), desktopBounds.getCentreY() - h / 2, universals->sliderPopupWidth, h);

				if (!universals->desktopManager.getBounds().contains(popupBounds)) // popup would go off screen
				{
					if (popupBounds.getRight() > universals->desktopManager.getWidth()) // need to go on left
					{
						popupBounds = popupBounds.withRightX(desktopBounds.getX());
					}
				}
				popup->setBoundsContained(popupBounds, 0);




				popup->show();
				return;
			}


			//setVelocityBasedMode(e.mods.withoutMouseButtons().getRawFlags() ==universals->bindings[SliderKeyBindings::SingleClickFineControl]);



			//================================================================================================================================================================================================


			if (e.mods.withoutMouseButtons().getRawFlags() == universals->bindings[SliderKeyBindings::SingleClickReset] || e.mods.isMiddleButtonDown())
			{
				universals->undoManager->beginNewTransaction(); // sometimes doens't get called for some reason
				setValue(getDefaultValue());
			}
			else
			{
				if (!unboundedMouseDrag)
				{
					e.source.enableUnboundedMouseMovement(true);
				}
				lastDragPosition = e.position;
				dragDelta = 0;
				catchDragDelta = 0;
				dragStartProp = valueToProportionOfLength(getValue());
				currentCatchRange = -1;
				juce::Slider::mouseDown(e);

			}
		}
	}


	void BaseSlider::mouseUp(const juce::MouseEvent& e)
	{
		//setVelocityBasedMode(false);
		juce::Slider::mouseUp(e);

		if (!unboundedMouseDrag)
		{
			e.source.enableUnboundedMouseMovement(false);
		}
	}


	void BaseSlider::mouseDrag(const juce::MouseEvent& e)
	{
		if (isEnabled())
		{

			if (!popup->isVisible() && e.mods.withoutMouseButtons().getRawFlags() != universals->bindings[SliderKeyBindings::SingleClickReset] && !e.mods.isMiddleButtonDown())
			{


				auto style = getSliderStyle();
				auto speedMult = e.mods.withoutMouseButtons().getRawFlags() == universals->bindings[SliderKeyBindings::SingleClickFineControl] ? 0.01f : 1.0f;
				auto mousePos = (isHorizontal() || style == RotaryHorizontalDrag) ? e.position.x : e.position.y;
				catchEscape = 0.075 * speedMult;


				double newPos = 0;

				if (style == RotaryHorizontalDrag
					|| style == RotaryVerticalDrag
					|| style == IncDecButtons
					|| ((style == LinearHorizontal || style == LinearVertical || style == LinearBar || style == LinearBarVertical)
						&& !getSliderSnapsToMousePosition()))
				{
					auto mouseDiff = (style == RotaryHorizontalDrag
						|| style == LinearHorizontal
						|| style == LinearBar)
						? e.position.x - lastDragPosition.x
						: lastDragPosition.y - e.position.y;

					mouseDiff *= speedMult;

					dragDelta += mouseDiff * (1.0 / getMouseDragSensitivity());
					catchDragDelta += mouseDiff * (1.0 / getMouseDragSensitivity());

					newPos = dragCatchCheck();
					//newPos = dragDeltaToProportionDelta(newPos, dragDelta);


				}
				else if (style == RotaryHorizontalVerticalDrag)
				{
					auto mouseDiff = (e.position.x - lastDragPosition.x)
						+ (lastDragPosition.y - e.position.y);

					mouseDiff *= speedMult;

					dragDelta += mouseDiff * (1.0 / getMouseDragSensitivity());
					catchDragDelta += mouseDiff * (1.0 / getMouseDragSensitivity());
					newPos = dragCatchCheck();

					//newPos = dragDeltaToProportionDelta(newPos, dragDelta);
				}
				else
				{
					auto layout = getLookAndFeel().getSliderLayout(*this);
					int regionStart = 0;
					int regionSize = 1;
					if (isHorizontal())
					{
						regionStart = layout.sliderBounds.getX();
						regionSize = layout.sliderBounds.getWidth();
					}
					else if (isVertical())
					{
						regionStart = layout.sliderBounds.getY();
						regionSize = layout.sliderBounds.getHeight();
					}
					newPos = (mousePos - regionStart) / regionSize;

					if (isVertical())
						newPos = 1.0 - newPos;
				}

				newPos = (isRotary() && !getRotaryParameters().stopAtEnd) ? newPos - std::floor(newPos)
					: juce::jlimit(0.0, 1.0, newPos);
				lastDragPosition = e.position;
				setValue(snapValue(proportionOfLengthToValue(newPos), absoluteDrag),
					notifyOnRelease ? juce::dontSendNotification : juce::sendNotificationSync);
			}
		}
	}

	void BaseSlider::mouseDoubleClick(const juce::MouseEvent& e)
	{
		if (isEnabled())
		{
			if (e.mods.withoutMouseButtons().getRawFlags() == universals->bindings[SliderKeyBindings::DoubleClickTextEdit] && isTextBoxEditable())
			{
				showTextEditor();
			}
			else if (e.mods.withoutMouseButtons().getRawFlags() == juce::ModifierKeys::noModifiers)
			{
				setValue(getDefaultValue());
			}
		}
	}

	void BaseSlider::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{
		if (isEnabled())
		{

			auto prevValue = getValue();
			juce::MouseWheelDetails newMW = mw;

			if (e.mods.withoutMouseButtons().getRawFlags() == universals->bindings[SliderKeyBindings::SingleClickFineControl])
			{
				newMW.deltaX /= BDSP_FINE_MOUSE_WHEEL_REDUCTION_FACTOR;
				newMW.deltaY /= BDSP_FINE_MOUSE_WHEEL_REDUCTION_FACTOR;
			}

			if (e.eventTime != lastMouseWheelTime)
			{
				lastMouseWheelTime = e.eventTime;



				auto wheelAmount = (std::abs(newMW.deltaX) > std::abs(newMW.deltaY) ? -newMW.deltaX : newMW.deltaY) * (newMW.isReversed ? -1.0f : 1.0f);

				auto proportionDelta = wheelAmount * 0.15;
				wheelProp += proportionDelta;
				wheelProp = (isRotary() && !getRotaryParameters().stopAtEnd) ? wheelProp - std::floor(wheelProp) : juce::jlimit(0.0, 1.0, wheelProp);




				setValue(snapValue(proportionOfLengthToValue(wheelProp), notDragging), juce::sendNotificationSync);






				for (int i = 0; i < catchValues.size(); i++)
				{
					auto v = proportionOfLengthToValue(catchValues[i]);
					if (isBetweenExclusive(v, prevValue, getValue()))
					{
						setValue(v);
						break;
					}
				}

			}
		}
	}


	void BaseSlider::setChangeNotificationOnlyOnRelease(bool onlyNotifyOnRelease)
	{
		juce::Slider::setChangeNotificationOnlyOnRelease(onlyNotifyOnRelease);
		notifyOnRelease = onlyNotifyOnRelease;
	}

	void BaseSlider::modifierKeysChanged(const juce::ModifierKeys& mod)
	{
		if (mod.testFlags(universals->bindings[SliderKeyBindings::SingleClickFineControl]))
		{
			catchDragDelta = 0;
		}
	}

	void BaseSlider::setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text, const NamedColorsIdentifier& textHighlight, const NamedColorsIdentifier& textEdit, const NamedColorsIdentifier& caret, const NamedColorsIdentifier& highlight)
	{
		knobColor = knob;
		valueColor = value;
		valueTrackColor = valueTrack;
		valueTrackInsideColor = valueTrackInside;
		textColor = text;
		textHighlightColor = textHighlight;
		textEditColor = textEdit;
		caretColor = caret;
		highlightColor = highlight;

		GUI_UniversalsChanged();
		repaint();

		popup->List.setColorSchemeClassic(knobColor, NamedColorsIdentifier(), NamedColorsIdentifier(BDSP_COLOR_PURE_BLACK), valueColor);

	}

	void BaseSlider::setSliderColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& caret, float altOpactiy)
	{
		//setSliderColors(knobColor, c, valueTrackColor, valueTrackInsideColor, c, c, c, caret, c.withMultipliedAlpha(altOpactiy));
		if (altOpactiy < 0)
		{
			altOpactiy = universals->lowOpacity;
		}
		valueColor = c;
		textColor = c;
		textHighlightColor = c;
		textEditColor = c;
		caretColor = caret;
		highlightColor = c.withMultipliedAlpha(altOpactiy);
		GUI_UniversalsChanged();
		repaint();
		popup->List.setColorSchemeClassic(knobColor, NamedColorsIdentifier(), NamedColorsIdentifier(BDSP_COLOR_PURE_BLACK), valueColor);
	}



	void BaseSlider::setFontIndex(int idx)
	{
		ComponentCore::setFontIndex(idx);
		if (mainLabel != nullptr)
		{
			mainLabel->setFontIndex(idx);
			mainLabel->getTitle().setFontIndex(idx);
		}
	}

	void BaseSlider::GUI_UniversalsChanged()
	{
		setColour(backgroundColourId, getColor(knobColor));
		setColour(trackColourId, getColor(valueColor));
		setColour(thumbColourId, getColor(valueTrackColor));
		setColour(juce::Label::textColourId, getColor(textColor));
		setColour(juce::TextEditor::textColourId, getColor(textColor));
		setColour(juce::TextEditor::highlightedTextColourId, getColor(textHighlightColor));
		setColour(juce::Label::textWhenEditingColourId, getColor(textEditColor));
		setColour(juce::CaretComponent::caretColourId, getColor(caretColor));
		setColour(juce::TextEditor::highlightColourId, getColor(highlightColor));

		if (mainLabel != nullptr)
		{
			copyAllExplicitColoursTo(*mainLabel);
			mainLabel->copyAllExplicitColoursTo(mainLabel->getTitle());
		}

	}



	double BaseSlider::dragCatchCheck()
	{
		auto newPos = dragStartProp + dragDelta;
		auto oldPos = valueToProportionOfLength(getValue());
		for (int i = 0; i < catchValues.size(); i++)
		{
			auto c = catchValues[i];
			if (currentCatchRange == i)
			{
				if (abs(catchDragDelta) > catchEscape)
				{
					newPos = c + signum(catchDragDelta) * juce::jmax(getInterval(), abs(catchDragDelta) - catchEscape);
					currentCatchRange = -1;

					dragStartProp = newPos;
					dragDelta = 0;
					break;
				}
				else
				{
					newPos = c;
					break;
				}
			}
			else if (isBetweenInclusive(c, oldPos, newPos)) //entered a catch range
			{

				catchDragDelta = 0;
				currentCatchRange = i;
				newPos = c;


				break;
			}

		}
		return newPos;

	}

	void BaseSlider::parameterLambdasUpdated()
	{
		if (universals->APVTS != nullptr)
		{
			attach(*universals->APVTS, param->getParameterID()); // rebuilds the attachment with the updated lambdas and range
		}
	}

	void BaseSlider::sliderValueChanged(juce::Slider* slider)
	{
		if (slider == this)
		{
			wheelProp = valueToProportionOfLength(getValue());
		}
	}





	//===================================================================================================================================================================

	BaseSlider::Label::Label(BaseSlider* s)
		:bdsp::Label(s->universals, ""),
        title(s->universals, ""),
        parentSlider(*s)
	{
		addListener(this);
		//title.addListener(this);

		parentSlider.addListener(this);

		parentSlider.addAndMakeVisible(title);
		parentSlider.addChildComponent(this);

		setJustificationType(juce::Justification::centred);
		title.setJustificationType(juce::Justification::centred);

		setFontIndex(0);
		title.setFontIndex(0);


		setInterceptsMouseClicks(true, true);
		title.setInterceptsMouseClicks(true, true);

		title.addHoverPartner(s);
		title.addMouseListener(s, true);

		//onTextChange = [=]()
		//{
		//	setVisible(true);
		//	parentSlider.setHovering(false);
		//};


		const auto& borderSize = juce::BorderSize<int>(0);
		setBorderSize(borderSize);
		title.setBorderSize(borderSize);

		title.setEditable(false, false, false);

		title.setMouseCursor(getMouseCursor());



	}

	BaseSlider::Label::~Label()
	{

	}

	void BaseSlider::Label::setAsMainLabel()
	{
		parentSlider.mainLabel = this;
		linkToComponent(&parentSlider);
	}


	void BaseSlider::Label::mouseDoubleClick(const juce::MouseEvent& e)
	{
		parentSlider.mouseDoubleClick(e);
	}

	void BaseSlider::Label::labelTextChanged(juce::Label* labelThatHasChanged)
	{

		auto val = parentSlider.getValueFromText(getText());

		//parentSlider.setValue(parentSlider.proportionOfLengthToValue(val));
		parentSlider.setValue(val);

		if (parentSlider.textFromValueFunction.operator bool())
		{
			setText(parentSlider.textFromValueFunction(val), juce::dontSendNotification); // ensures that textUp is formatted correctly for influence
		}

		parentSlider.updateLabel();
		//repaint();

	}

	void BaseSlider::Label::editorShown(juce::Label* labelThatHasChanged, juce::TextEditor& te)
	{
		labelThatHasChanged->copyAllExplicitColoursTo(te);
		te.applyFontToAllText(parentSlider.universals->Fonts[getFontIndex()].getFont());
	}

	void BaseSlider::Label::editorHidden(juce::Label* labelThatHasChanged, juce::TextEditor& te)
	{
		setHovering(desiredHoverState);
	}

	void BaseSlider::Label::sliderValueChanged(juce::Slider* slider)
	{
		parentSlider.updateLabel();
	}

	juce::TextEditor* BaseSlider::Label::createEditorComponent()
	{
		auto* ed = new CenteredTextEditor(this);
		return ed;
	}

	void BaseSlider::Label::setHasNoTitle(bool state)
	{
		if (state)
		{
			hasNoTitle = true;
		}
		else
		{
			hasNoTitle = false;
		}
		setVisible(false);
		visibilityChanged();
	}

	bool BaseSlider::Label::getHasNoTitle()
	{
		return hasNoTitle;
	}

	void BaseSlider::Label::visibilityChanged()
	{
		if (hasNoTitle)
		{
			title.setVisible(false);
		}
		else
		{
			title.setVisible(!isVisible());
		}
	}

	void BaseSlider::Label::parentHierarchyChanged()
	{
		auto parent = getParentComponent();
		if (parent != nullptr)
		{
			parent->addChildComponent(title);
		}
		else
		{
			parent = title.getParentComponent();
			if (parent != nullptr)
			{
				parent->removeChildComponent(&title);
			}
		}
	}

	void BaseSlider::Label::lookAndFeelChanged()
	{
		title.setLookAndFeel(&getLookAndFeel());
	}

	void BaseSlider::Label::resized()
	{
		title.setBounds(getBounds());


		auto ed = getCurrentTextEditor();
		if (ed != nullptr)
		{
			ed->setBounds(getLocalBounds());
		}
	}

	void BaseSlider::Label::setTitle(const juce::String& s)
	{
		title.setText(s, juce::sendNotification);
	}

	bdsp::Label& BaseSlider::Label::getTitle()
	{
		return title;
	}

	void BaseSlider::Label::colourChanged()
	{
		copyAllExplicitColoursTo(title);
	}

	void BaseSlider::Label::paint(juce::Graphics& g)
	{


		if (parentSlider.pathFromValueFunction.operator bool())
		{
			scaleToFit(parentSlider.labelPath, getLocalBounds());
			g.setColour(findColour(juce::Label::textColourId));
			g.fillPath(parentSlider.labelPath);
		}
		else
		{
			bdsp::Label::paint(g);
		}
	}






}// namnepace bdsp



