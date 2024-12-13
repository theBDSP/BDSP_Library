#include "bdsp_AlertWindow.h"
namespace bdsp
{
	AlertWindow::AlertWindow(std::function<void(bool)>& ShowHideFunction, GUI_Universals* universalsToUse)
		:DesktopComponent(universalsToUse, true, false),
		Header(this, universalsToUse),
		Body(this, universalsToUse)
	{

		setInterceptsMouseClicks(true, true);
		setLossOfFocusClosesWindow(false);
		//universalsToUse->desktopManager.addComponent(this);


		resetLookAndFeels(universalsToUse);


		Header.setJustificationType(juce::Justification::centred);
		Body.setJustificationType(juce::Justification::centred);

		//Header.addMouseListener(this,true);
		//Body.addMouseListener(this,true);



		setOpaque(false);
		addAndMakeVisible(Header);
		addAndMakeVisible(Body);

		std::function<void()> showFunc = [=]()
		{
			ShowHideFunction(true);
		};

		std::function<void()> hideFunc = [=]()
		{
			ShowHideFunction(false);
		};
		onShow = showFunc;
		onHide = hideFunc;

		setColors(BDSP_COLOR_MID, BDSP_COLOR_DARK, BDSP_COLOR_WHITE, BDSP_COLOR_WHITE, BDSP_COLOR_WHITE, BDSP_COLOR_COLOR, BDSP_COLOR_BLACK, BDSP_COLOR_BLACK);
	}


	AlertWindow::~AlertWindow()
	{
		clearLookAndFeels();
	}

	void AlertWindow::reset()
	{
		leftButton.reset();
		centerButton.reset();
		rightButton.reset();

		Header.setText("", juce::dontSendNotification);
		Body.setText("", juce::dontSendNotification);
		onSelect = std::function<void(int)>();
	}

	void AlertWindow::addItemToQueue(const AlertWindowItem& item)
	{
		auto prevFirst = ItemQueue.getFirst();
		ItemQueue.addUsingDefaultSort(item);
		if (ItemQueue.getFirst() == item) // if this item is now the top priority item make and display it
		{
			makeNew(item);
		}

		if (ItemQueue.getFirst() != prevFirst && prevFirst.DismissOnHide) // if the new item replaced an existing one and that item has a dismiss function call it
		{
			removeFromQueue(prevFirst, false);
		}
	}

	void AlertWindow::addMessageToQueue(const juce::String& buttonText, const juce::String& headerText, const juce::String& bodyText, AlertItemPriority priority)
	{
		std::function<void(int)> Func = [this](int idx)
		{
			hide();
		};
		addItemToQueue(AlertWindowItem(juce::String(), juce::String(), buttonText, headerText, bodyText, Func, priority));
	}

	AlertWindowItem AlertWindow::getVisibleItem()
	{
		AlertWindowItem out;
		if (isVisible())
		{
			out = ItemQueue.getFirst();
		}

		return out;
	}




	void AlertWindow::show()
	{

		//addToDesktop(juce::ComponentPeer::StyleFlags::windowIsTemporary);
		//setVisible(true);
		////juce::Rectangle<int> bounds;

		//bounds = topComp->getScreenBounds();


		resize();
		DesktopComponent::show();


	}


	void AlertWindow::hide()
	{
		//removeFromDesktop();
		//setVisible(false);
		//onShowHide(false);

		DesktopComponent::hide();
		removeFromQueue(ItemQueue.getFirst());
	}

	void AlertWindow::resize()
	{
		setBounds(manager->mainArea.getProportion(juce::Rectangle<float>((1 - relWidth) / 2.0f, (1 - relHeight) / 2.0f, relWidth, relHeight)).withCentre(manager->getBounds().getCentre()));

		auto reduced = getLocalBounds().reduced(universals->roundedRectangleCurve);
		Header.setBounds(reduced.getProportion(juce::Rectangle<float>(0, 0, 1, HeaderRatio)));
		Body.setBounds(reduced.getProportion(juce::Rectangle<float>(0, HeaderRatio, 1, BodyRatio)));

		auto y = HeaderRatio + BodyRatio;
		auto w = 1 / 4.0f;
		//auto inset = w / 10;
		auto h = (1 - y) * 0.6f;
		auto inset = (1 - y - h) / 2;

		if (leftButton != nullptr)
		{
			leftButton->setBounds(reduced.getProportion(juce::Rectangle<float>(inset, y + inset, w, 1 - y - 2 * inset)));
		}

		if (centerButton != nullptr)
		{
			centerButton->setBounds(reduced.getProportion(juce::Rectangle<float>(0.5 - w / 2.0, y + inset, w, 1 - y - 2 * inset)));
		}

		if (rightButton != nullptr)
		{
			rightButton->setBounds(reduced.getProportion(juce::Rectangle<float>(1 - w - inset, y + inset, w, 1 - y - 2 * inset)));
		}

		dropShadow.setRadius(universals->roundedRectangleCurve);
	}

	void AlertWindow::setRelativeSize(float relativeWidth, float relativeHeight)
	{
		relWidth = relativeWidth;
		relHeight = relativeHeight;
		resize();
	}

	void AlertWindow::paint(juce::Graphics& g)
	{
		auto contentW = 0.95f;

		auto reduced = getLocalBounds().toFloat().reduced(universals->roundedRectangleCurve);

		juce::Path bkgdPath;
		bkgdPath.addRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);

		dropShadow.render(g, bkgdPath);

		g.setColour(getColor(BKGD));
		g.fillRoundedRectangle(reduced, universals->roundedRectangleCurve);

		//================================================================================================================================================================================================
		auto dividerColor = getColor(BDSP_COLOR_KNOB);
		g.setColour(getColor(HeaderBKGD));
		juce::Path header;
		header.addRoundedRectangle(reduced.getX(), reduced.getY(), reduced.getWidth(), Header.getBottom() - reduced.getY(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, true, true, false, false);
		g.fillPath(header);
		bdsp::drawDivider(g, juce::Line<float>(reduced.getX(), Header.getBottom(), reduced.getRight(), Header.getBottom()), dividerColor, universals->dividerSize);


		//================================================================================================================================================================================================
		auto y = reduced.getRelativePoint(0.0f, HeaderRatio + BodyRatio).y;
		g.setColour(getColor(HeaderBKGD));
		juce::Path footer;
		footer.addRoundedRectangle(reduced.getX(), y, reduced.getWidth(), reduced.getBottom() - y, universals->roundedRectangleCurve, universals->roundedRectangleCurve, false, false, true, true);
		g.fillPath(footer);
		bdsp::drawDivider(g, juce::Line<float>(reduced.getX(), y, reduced.getRight(), y).reversed(), dividerColor, universals->dividerSize);


		//================================================================================================================================================================================================




		auto headerRect = Header.getBounds().withSizeKeepingCentre(Header.getWidth() * contentW, Header.getHeight());

		g.setColour(getColor(HeaderText));
		g.setFont(resizeFontToFit(universals->Fonts[headerFontIndex].getFont(), headerRect.getWidth() * contentW, headerRect.getHeight() * 0.9f, Header.getText()));
		drawText(g, g.getCurrentFont(), Header.getText(), headerRect);




		g.setColour(getColor(BodyText));
		g.setFont(universals->Fonts[bodyFontIndex].getFont().withHeight(g.getCurrentFont().getHeight() * 0.9f));
		//g.drawMultiLineText(Body.getText(), (1 - contentW*contentW) / 2.0 * reduced->getWidth(), Body.getY() + g.getCurrentFont().getHeight() * 1.5, reduced->getWidth() * contentW*contentW, juce::Justification::centred);
		g.drawFittedText(Body.getText(), Body.getBounds().getProportion(juce::Rectangle<float>(0.05f, 0.05f, 0.9f, 0.9f)), juce::Justification::centredTop, Body.getHeight() / g.getCurrentFont().getHeight());


		g.setColour(getColor(BDSP_COLOR_LIGHT));
		g.drawRoundedRectangle(reduced, universals->roundedRectangleCurve, universals->roundedRectangleCurve / 4);

	}

	void AlertWindow::setColors(const NamedColorsIdentifier& newBKGD, const NamedColorsIdentifier& newHeaderBKGD, const NamedColorsIdentifier& newHeaderText, const NamedColorsIdentifier& newBodyText, const NamedColorsIdentifier& newButtonBKGDUp, const NamedColorsIdentifier& newButtonBKGDDown, const NamedColorsIdentifier& newButtonTextUp, const NamedColorsIdentifier& newButtonTextDown)
	{

		BKGD = newBKGD;
		HeaderBKGD = newHeaderBKGD;
		HeaderText = newHeaderText;
		BodyText = newBodyText;
		ButtonBKGDUp = newButtonBKGDUp;
		ButtonBKGDDown = newButtonBKGDDown;
		ButtonTextUp = newButtonTextUp;
		ButtonTextDown = newButtonTextDown;


		if (leftButton != nullptr)
		{
			leftButton->backgroundUp = ButtonBKGDUp;
			leftButton->backgroundDown = ButtonBKGDDown;
			leftButton->textUp = ButtonTextUp;
			leftButton->textDown = ButtonTextDown;
			leftButton->repaint();
		}

		if (centerButton != nullptr)
		{
			centerButton->backgroundUp = ButtonBKGDUp;
			centerButton->backgroundDown = ButtonBKGDDown;
			centerButton->textUp = ButtonTextUp;
			centerButton->textDown = ButtonTextDown;
			centerButton->repaint();
		}

		if (rightButton != nullptr)
		{
			rightButton->backgroundUp = ButtonBKGDUp;
			rightButton->backgroundDown = ButtonBKGDDown;
			rightButton->textUp = ButtonTextUp;
			rightButton->textDown = ButtonTextDown;
			rightButton->repaint();
		}

		dropShadow.setColor(getColor(BDSP_COLOR_PURE_BLACK));
		repaint();

	}

	void AlertWindow::setFonts(int headerIndex, int bodyIndex)
	{
		headerFontIndex = headerIndex;
		bodyFontIndex = bodyIndex;
		repaint();
	}

	bool AlertWindow::keyPressed(const juce::KeyPress& key)
	{
		auto code = key.getKeyCode();

		auto idxs = getAllValidIndecies();
		if (code == key.leftKey)
		{
			int idx = idxs.indexOf(highlightedButton);
			int newIdx = idxs[(idxs.size() + idx - 1) % idxs.size()];
			setHighlighted(newIdx);
		}
		else if (code == key.rightKey)
		{
			int idx = idxs.indexOf(highlightedButton);
			int newIdx = idxs[(idx + 1) % idxs.size()];
			setHighlighted(newIdx);
		}
		else if (code == key.returnKey)
		{
			onSelect(highlightedButton);
		}
		return true;
	}

	void AlertWindow::setHighlighted(int idx)
	{
		jassert(idx > -1 && idx < 3);

		if (leftButton != nullptr)
		{
			leftButton->forceHighlight = (idx == 0);
			leftButton->repaint();
		}

		if (centerButton != nullptr)
		{
			centerButton->forceHighlight = (idx == 1);
			centerButton->repaint();
		}

		if (rightButton != nullptr)
		{
			rightButton->forceHighlight = (idx == 2);
			rightButton->repaint();
		}
		highlightedButton = idx;
	}

	juce::Array<int> AlertWindow::getAllValidIndecies()
	{
		juce::Array<int> out;
		if (leftButton != nullptr)
		{
			out.add(0);
		}

		if (centerButton != nullptr)
		{
			out.add(1);
		}

		if (rightButton != nullptr)
		{
			out.add(2);
		}

		return out;
	}

	void AlertWindow::lookAndFeelChanged()
	{
		Header.setLookAndFeel(&getLookAndFeel());
		Body.setLookAndFeel(&getLookAndFeel());
	}



	bool AlertWindow::itemQueueIsEmpty()
	{
		return ItemQueue.isEmpty();
	}

	//=====================================================================================================================

	void AlertWindow::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
	{
		resize();
	}

	void AlertWindow::makeNew(const AlertWindowItem& item)
	{
		reset();
		if (!item.LeftName.isEmpty())
		{
			leftButton = std::make_unique<IndexedButton>(this, 0, item.LeftName);
		}
		if (!item.CenterName.isEmpty())
		{
			centerButton = std::make_unique<IndexedButton>(this, 1, item.CenterName);
		}
		if (!item.RightName.isEmpty())
		{
			rightButton = std::make_unique<IndexedButton>(this, 2, item.RightName);
		}
		setHighlighted(item.LeftName.isEmpty() ? (item.CenterName.isEmpty() ? 2 : 1) : 0); //furtherst left button

		Header.setText(item.HeaderText, juce::dontSendNotification);
		Body.setText(item.BodyText, juce::dontSendNotification);
		onSelect = item.SelectFunction;

		show();
	}




	void AlertWindow::removeFromQueue(const AlertWindowItem& item, bool showNext)
	{
		for (int i = 0; i < ItemQueue.size(); i++)
		{
			if (ItemQueue[i] == item)
			{
				ItemQueue.remove(i);
				break;
			}
		}

		if (!ItemQueue.isEmpty() && showNext)
		{
			makeNew(ItemQueue.getFirst());
		}
	}


	//========================================================================================================================================================================================================================



	AlertWindow::IndexedButton::IndexedButton(AlertWindow* p, int idx, const juce::String& text)
		:TextButton(p->universals)
	{
		setHasOutline(false);

		parent = p;
		index = idx;
		setText(text);
		setLookAndFeel(&parent->getLookAndFeel());
		parent->addAndMakeVisible(this);

		backgroundUp = p->ButtonBKGDUp;
		backgroundDown = p->ButtonBKGDDown;
		textUp = p->ButtonTextUp;
		textDown = p->ButtonTextDown;

		onClick = [this]()
		{
			if (parent->onSelect.operator bool())
			{
				parent->onSelect(index);
			}
		};
	}

	AlertWindow::IndexedButton::~IndexedButton()
	{
		setLookAndFeel(nullptr);
	}

	void AlertWindow::IndexedButton::mouseEnter(const juce::MouseEvent& e)
	{
		parent->setHighlighted(index);
	}

	bool AlertWindow::IndexedButton::keyPressed(const juce::KeyPress& key)
	{
		return false;
	}

	//void AlertWindow::IndexedButton::paintButton(juce::Graphics& g, bool highlighted, bool down)
	//{
	//	juce::Path p;
	//	p.addRoundedRectangle(getLocalBounds().toFloat(), parent->universals->roundedRectangleCurve);

	//	g.setFont(dynamic_cast<AlertLNF*>(&parent->getLookAndFeel())->getFont(getFontIndex()).withHeight(getHeight() * 0.8));
	//	g.setColour(getHoverColor(parent->getColor(parent->ButtonBKGDDown), parent->getColor(parent->ButtonBKGDUp), forceHighlight, highlighted));
	//	g.fillPath(p);


	//	g.setColour(parent->getColor(forceHighlight ? parent->ButtonTextDown : parent->ButtonTextUp));


	//	drawText(g, g.getCurrentFont(), getButtonText(), getLocalBounds());
	//}


}// namnepace bdsp
//==============================================================================
