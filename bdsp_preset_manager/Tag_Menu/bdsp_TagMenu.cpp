#include "bdsp_TagMenu.h"
namespace bdsp
{
	TagMenu::TagMenu(PresetManager* parent)
	{
		p = parent;
		//addAndMakeVisible(clear);

		rebuildMenu();



	}

	TagMenu::~TagMenu()
	{
		for (auto e : menuExpanders)
		{
			e->removeMouseListener(this);
		}
		for (auto t : menuExpanderTriangles)
		{
			t->removeMouseListener(this);
		}

	}


	juce::StringArray TagMenu::getCurrentTags()
	{
		return currentTags;
	}

	juce::StringArray TagMenu::getCurrentSuperTags()
	{
		return currentSuperTags;
	}

	void TagMenu::reFilter()
	{
		auto& temp = currentSuperTags;
		for (auto arr : SuperTags)
		{
			for (auto stb : *arr)
			{
				stb->setToggleState(false, juce::sendNotification);
				if (temp.contains(stb->getText()))
				{
					stb->triggerClick();
				}
			}
		}

		temp = currentTags;
		for (auto tb : tags)
		{
			tb->setToggleState(false, juce::sendNotification);
			if (temp.contains(tb->getText()))
			{
				tb->triggerClick();
			}

		}
	}


	void TagMenu::rebuildMenu()
	{
		int openIdx = -1;

		for (int i = 0; i < menuExpanders.size(); ++i)
		{
			if (menuExpanders[i]->getToggleState())
			{
				openIdx = i;
				break;
			}
		}

		for (const auto& s : currentSuperTags)
		{
			if (!p->Handler->superTagNames.contains(s))
			{
				currentSuperTags.remove(currentSuperTags.indexOf(s));
			}
		}

		for (const auto& s : currentTags)
		{
			if (!p->Handler->tagNames.contains(s))
			{
				currentTags.remove(currentTags.indexOf(s));
			}
		}
		vpViewedComps.clear();
		vps.clear();
		tags.clear();
		menuExpanders.clear();
		menuExpanderTriangles.clear();

		auto triRect = juce::Rectangle<float>(-5, 1, 3, 3);

		SuperTags.clear();
		for (int i = 0; i < p->Handler->superTagNames.size(); ++i)
		{
			vps.add(new Viewport(p->universals));
			addAndMakeVisible(vps.getLast());
//			vps.getLast()->setLookAndFeel(&p->LNF);

			vpViewedComps.add(new juce::Component());
			vps.getLast()->setViewedComponent(vpViewedComps.getLast());
			vps.getLast()->setScrollBarsShown(true, false);
			vpViewedComps.getLast()->addMouseListener(this, false);

			//================================================================================================================================================================================================
			menuExpanders.add(new TextButton(p->universals));
			menuExpanders.getLast()->setClickingTogglesState(true);
			menuExpanders.getLast()->setText("Tags");

			menuExpanders.getLast()->textUp = p->pc.presetText;
			menuExpanders.getLast()->textDown = BDSP_COLOR_COLOR;
			menuExpanders.getLast()->backgroundUp = NamedColorsIdentifier();
			menuExpanders.getLast()->backgroundDown = NamedColorsIdentifier();

			menuExpanders.getLast()->setHasOutline(false);

			menuExpanders.getLast()->setFontIndex(2);

			addAndMakeVisible(menuExpanders.getLast());


			auto expanded = createEquilateralTriangle(triRect);
			auto retracted = createEquilateralTriangle(triRect, PI);


			menuExpanderTriangles.add(new MultiShapeButton(p->universals, true));
			menuExpanderTriangles.getLast()->addShapes(Shape(retracted, p->pc.presetText, juce::Rectangle<float>(0, 0, 1, 1)), Shape(expanded, BDSP_COLOR_COLOR, juce::Rectangle<float>(0, 0, 1, 1)));
			addAndMakeVisible(menuExpanderTriangles.getLast());

			menuExpanders.getLast()->addMouseListener(this, false);
			menuExpanderTriangles.getLast()->addMouseListener(this, false);

			menuExpanders.getLast()->setAllowsPopupClick(false);
			menuExpanderTriangles.getLast()->setAllowsPopupClick(false);

			menuExpanderTriangles.getLast()->onStateChange = [=]()
			{
				menuExpanderTriangles[i]->repaint();
			};

			menuExpanders.getLast()->onStateChange = [=]()
			{
				if (menuExpanders[i]->getToggleState())
				{
					for (int j = 0; j < menuExpanders.size(); ++j)
					{
						if (j != i)
						{
							menuExpanderTriangles[i]->setToggleState(false, juce::dontSendNotification);
							menuExpanders[j]->setToggleState(false, juce::dontSendNotification);
						}

					}
					vps[i]->setVisible(true);
					setSize(getWidth(), vps[i]->getBottom());

				}
				else
				{
					vps[i]->setVisible(false);
					bool anyExpanded = false;
					for (auto me : menuExpanders)
					{
						if (me->getToggleState())
						{
							anyExpanded = true;
							break;
						}
					}
					setSize(getWidth(), anyExpanded ? vps[i]->getBottom() : closedHeight);
				}
				menuExpanderTriangles[i]->setToggleState(menuExpanders[i]->getToggleState(), juce::dontSendNotification);
			};

			//================================================================================================================================================================================================



			SuperTags.add(new juce::OwnedArray<TagButton>());
			auto arr = SuperTags.getUnchecked(i);
			for (int j = 0; j < p->Handler->superTagNames[i].size(); ++j)
			{

				arr->add(new TagButton(p->universals, p->Handler->tagNames[i], &p->pc));
				vps.getLast()->getViewedComponent()->addAndMakeVisible(arr->getLast(), i);

				arr->getLast()->setFontIndex(0);

				arr->getLast()->setRadioGroupId(1);
				arr->getLast()->onClick = [=]()
				{
					if (arr->getUnchecked(j)->getToggleState())
					{
						currentSuperTags.set(i, arr->getUnchecked(j)->getText());
					}
					else
					{
						currentSuperTags.removeString(arr->getUnchecked(j)->getText());
					}

					p->menu->tagFilter(currentTags, currentSuperTags);

					if (p->menu->isShowing())
					{
						p->menu->grabKeyboardFocus();
					}

				};

				if (currentSuperTags.contains(arr->getLast()->getText()))
				{
					arr->getLast()->setToggleState(true, juce::sendNotification);
				}
			}
		}


		//================================================================================================================================================================================================

		vps.add(new Viewport(p->universals));
		addAndMakeVisible(vps.getLast());
//		vps.getLast()->setLookAndFeel(&p->LNF);
		vps.getLast()->setScrollBarsShown(true, false);

		vpViewedComps.add(new juce::Component());
		vps.getLast()->setViewedComponent(vpViewedComps.getLast());


		vpViewedComps.getLast()->addMouseListener(this, false);
		//================================================================================================================================================================================================





		menuExpanders.add(new TextButton(p->universals));
		menuExpanders.getLast()->setClickingTogglesState(true);
		menuExpanders.getLast()->setText("Tags");

		menuExpanders.getLast()->textUp = p->pc.presetText;
		menuExpanders.getLast()->textDown = BDSP_COLOR_COLOR;
		menuExpanders.getLast()->backgroundUp = NamedColorsIdentifier();
		menuExpanders.getLast()->backgroundDown = NamedColorsIdentifier();

		menuExpanders.getLast()->setHasOutline(false);

		menuExpanders.getLast()->setFontIndex(2);

		addAndMakeVisible(menuExpanders.getLast());


		auto expanded = createEquilateralTriangle(triRect);
		auto retracted = createEquilateralTriangle(triRect, PI);


		menuExpanderTriangles.add(new MultiShapeButton(p->universals, true));
		menuExpanderTriangles.getLast()->addShapes(Shape(retracted, p->pc.presetText, juce::Rectangle<float>(0, 0, 1, 1)), Shape(expanded, BDSP_COLOR_COLOR, juce::Rectangle<float>(0, 0, 1, 1)));
		addAndMakeVisible(menuExpanderTriangles.getLast());



		menuExpanders.getLast()->addMouseListener(this, false);
		menuExpanderTriangles.getLast()->addMouseListener(this, false);


		menuExpanders.getLast()->setAllowsPopupClick(false);
		menuExpanderTriangles.getLast()->setAllowsPopupClick(false);


		menuExpanderTriangles.getLast()->onStateChange = [=]()
		{
			menuExpanderTriangles.getLast()->repaint();
		};

		menuExpanders.getLast()->addHoverPartner(menuExpanderTriangles.getLast());
		menuExpanders.getLast()->onStateChange = [=]()
		{
			if (menuExpanders.getLast()->getToggleState())
			{
				for (int j = 0; j < menuExpanders.size() - 1; ++j)
				{
					menuExpanders[j]->setToggleState(false, juce::dontSendNotification);
					menuExpanderTriangles[j]->setToggleState(false, juce::dontSendNotification);
				}
				vps.getLast()->setVisible(true);
				calculateNewSize(getBounds().withHeight(maxOpenHeight));
				//setSize(getWidth(), vps.getLast()->getY() + vps.getLast()->getViewHeight());
			}
			else
			{
				vps.getLast()->setVisible(false);

				bool anyExpanded = false;
				for (auto me : menuExpanders)
				{
					if (me->getToggleState())
					{
						anyExpanded = true;
						break;
					}
				}
				setSize(getWidth(), closedHeight);
				//calculateNewSize(getBounds().withHeight(anyExpanded ? vps.getLast()->getBottom() : closedHeight));
				//calculateNewSize(getBounds().withHeight(closedHeight));

			}
			menuExpanderTriangles.getLast()->setToggleState(menuExpanders.getLast()->getToggleState(), juce::sendNotification);
		};




		//================================================================================================================================================================================================



		for (int i = 0; i < p->Handler->tagNames.size(); ++i)
		{
			tags.add(new TagButton(p->universals, p->Handler->tagNames[i],&p->pc));

			tags.getLast()->setFontIndex(0);

			vps.getLast()->getViewedComponent()->addAndMakeVisible(tags.getLast(), p->Handler->superTagNames.size());

			tags.getLast()->onClick = [=]()
			{
				if (tags[i]->getToggleState())
				{
					currentTags.addIfNotAlreadyThere(tags[i]->getText());
				}
				else
				{
					currentTags.removeString(tags[i]->getText());
				}

				p->menu->tagFilter(currentTags, currentSuperTags);

				if (p->menu->isShowing())
				{
					p->menu->grabKeyboardFocus();
				}
			};

			if (currentTags.contains(tags.getLast()->getText()))
			{
				tags.getLast()->setToggleState(true, juce::sendNotification);
			}

		}


		calculateNewSize(getBounds());
		if (openIdx >= 0)
		{
			menuExpanders[openIdx]->setToggleState(true, juce::sendNotification);
		}
	}


	bool TagMenu::isFiltering()
	{
		return !(currentTags.isEmpty() && currentSuperTags.isEmpty());
	}

	void TagMenu::calculateNewSize(juce::Rectangle<int> newBounds)

	{
		if (!newBounds.isEmpty())
		{
			maxOpenHeight = newBounds.getHeight();

			auto scrollBarW = p->scrollBarW;

			TagButton* lastButton = nullptr; // last button in menu


			auto y = 0;

			float w = newBounds.getWidth() * 0.95f;
			auto h = resizeFontToFit(p->universals->Fonts[0], w * 0.95f, DBL_MAX, BDSP_MAX_TAG_NAME_LENGTH).getHeight() * 0.65f;
			h /= BDSP_FONT_HEIGHT_SCALING;
			h /= BDSP_FONT_HEIGHT_SCALING;
			expanderH = h * 1.25;


			closedHeight = expanderH;

			border = (newBounds.getWidth() - w) / 2;



			//================================================================================================================================================================================================


			auto centerRow = [=](juce::OwnedArray<TagMenu::TagButton>& arr, int startIdx, int endIdx)
			{

				int totalW = arr[endIdx]->getRight() + border;
				auto x = (newBounds.getWidth() - scrollBarW - totalW) / 2 + border;
				for (int i = startIdx; i <= endIdx; ++i)
				{
					arr[i]->setTopLeftPosition(x, arr[i]->getY());
					x = arr[i]->getRight() + border;
				}
			};


			//================================================================================================================================================================================================


			int rowStart, rowEnd;
			int x;
			for (int i = 0; i < SuperTags.size(); ++i)
			{
				auto arr = SuperTags.getUnchecked(i);
				y = 0;
				x = border;
				rowStart = 0;
				rowEnd = 0;

				for (int j = 0; j < arr->size(); ++j)
				{
					auto curr = arr->getUnchecked(j);
					lastButton = curr;
					w = curr->getIdealWidth(h);

					curr->setBounds(x, y, w, h);

					if (curr->getRight() > newBounds.getWidth() - border - scrollBarW)
					{
						y += h + border;
						rowEnd = j - 1;

						centerRow(*arr, rowStart, rowEnd);

						rowStart = j;

						curr->setTopLeftPosition(border, y);
					}
					x = curr->getRight() + border;
				}

				vps[i]->setBounds(juce::Rectangle<int>(0, expanderH + border, newBounds.getWidth(), 1).withHeight(juce::jmin(tags.getLast()->getBottom(), maxOpenHeight - expanderH - border)));
				vps[i]->getViewedComponent()->setBounds(0, 0, vps[i]->getWidth(), arr->getLast()->getBottom());
				vps[i]->setScrollBarThickness(scrollBarW);
				vps[i]->setScrollDistance(h + border);

			}

			if (!tags.isEmpty())
			{
				y = 0;
				x = border;
				rowStart = 0;
				rowEnd = 0;

				for (int i = 0; i < tags.size(); ++i)
				{
					lastButton = tags[i];
					w = tags[i]->getIdealWidth(h);

					tags[i]->setBounds(x, y, w, h);

					if (tags[i]->getRight() > newBounds.getWidth() - border - scrollBarW)
					{
						y += h + border;
						rowEnd = i - 1;

						centerRow(tags, rowStart, rowEnd);

						rowStart = i;

						tags[i]->setTopLeftPosition(border, y);
					}
					x = tags[i]->getRight() + border;
				}
				centerRow(tags, rowStart, tags.size() - 1);
				vps.getLast()->setBounds(juce::Rectangle<int>(0, expanderH + border, newBounds.getWidth(), juce::jmin(maxOpenHeight - expanderH - border, tags.getLast()->getBottom() + border)));
				vps.getLast()->getViewedComponent()->setBounds(0, 0, vps.getLast()->getWidth(), tags.getLast()->getBottom());
				vps.getLast()->setScrollBarThickness(scrollBarW);
				vps.getLast()->setScrollDistance(h + border);
			}

			float num = menuExpanders.size();
			int bottom = expanderH;
			for (int i = 0; i < num; ++i)
			{
				menuExpanders[i]->setBounds(newBounds.withZeroOrigin().getProportion(juce::Rectangle<float>(i / num, 0, 1.0 / num, 1)).withHeight(expanderH));
				auto textW = menuExpanders[i]->getTextWidth();
				menuExpanderTriangles[i]->setBounds(juce::Rectangle<int>(menuExpanders[i]->getBounds().getCentreX() - textW / 2 - menuExpanders[i]->getHeight(), menuExpanders[i]->getY(), menuExpanders[i]->getHeight(), menuExpanders[i]->getHeight()).getProportion(juce::Rectangle<float>(0.3, 0.3, 0.4, 0.4)));

				if (menuExpanders[i]->getToggleState())
				{
					bottom = vps[i]->getBottom();
				}
			}
			setBounds(newBounds.withHeight(bottom));
		}

	}


	void TagMenu::resized()
	{
	}

	void TagMenu::paint(juce::Graphics& g)
	{
		if (isExpanded())
		{
			g.setColour(p->universals->colors.getColor(p->pc.tagMenuBKGD));
			g.fillRoundedRectangle(getLocalBounds().toFloat().withTrimmedTop(expanderH), p->universals->roundedRectangleCurve);
		}




		/*g.setColour(p->universals->colors.getColor(p->pc.presetText));
		g.setFont(p->LNF.universals->Fonts[1].getFont().withHeight(expanderH * 0.9));*/


		//for (int i = 0; i < superTagTitles.size(); ++i)
		//{
		//	auto bot = SuperTags.getUnchecked(i)->getFirst()->getY() - borderColor;
		//	auto rect = juce::Rectangle<float>(0, bot - expanderH, getWidth(), expanderH);

		//	g.drawText(superTagTitles[i], rect, juce::Justification::centred);

		//	g.drawHorizontalLine(SuperTags.getUnchecked(i)->getLast()->getBottom() + borderColor / 2.0, borderColor, getWidth() - borderColor);
		//}

		//if (!tags.isEmpty())
		//{
		//	auto bot = tags.getFirst()->getY() - borderColor;
		//	auto rect = juce::Rectangle<float>(0, bot - expanderH, getWidth(), expanderH);

		//	g.drawText("Tags", rect, juce::Justification::centred);

		//	g.drawHorizontalLine(bot, borderColor, getWidth() - borderColor);
		//}


	}

	void TagMenu::mouseDown(const juce::MouseEvent& e)
	{
		p->menu->mouseDown(e);
		//p->menu->grabKeyboardFocus();
	}

	void TagMenu::highlightTags(const juce::StringArray& superTagsToHighlight, const juce::StringArray& tagsToHighlight)
	{
		highlightedSuperTags = superTagsToHighlight;
		highlightedTags = tagsToHighlight;

		for (auto arr : SuperTags)
		{
			for (auto stb : *arr)
			{

				stb->highlighted = highlightedSuperTags.contains(stb->getText());
				stb->repaint();
			}
		}

		for (auto tb : tags)
		{
			tb->highlighted = highlightedTags.contains(tb->getText());
			tb->repaint();
		}

	}

	void TagMenu::clearTagHighlights()
	{
		highlightedSuperTags.clear();
		highlightedTags.clear();

		for (auto arr : SuperTags)
		{
			for (auto stb : *arr)
			{

				stb->highlighted = true;
				stb->repaint();
			}
		}

		for (auto tb : tags)
		{
			tb->highlighted = true;
			tb->repaint();
		}
	}

	void TagMenu::clearTags()
	{
		for (auto arr : SuperTags)
		{
			for (auto t : *arr)
			{
				t->setToggleState(false, juce::sendNotification);
			}
		}


		for (auto t : tags)
		{
			t->setToggleState(false, juce::sendNotification);
		}
	}

	void TagMenu::initMenuOpen()
	{
		menuExpanders.getLast()->setToggleState(false, juce::dontSendNotification);
		menuExpanders.getLast()->triggerClick();
	}

	bool TagMenu::isExpanded()
	{
		bool out = false;
		for (auto e : menuExpanders)
		{
			if (e->getToggleState())
			{
				out = true;
				break;
			}
		}
		return out;
	}



	//================================================================================================================================================================================================



	TagMenu::TagButton::TagButton(GUI_Universals* universalsToUse, const juce::String& s, PresetManagerColors* pmc, bool isAdd)
		:TextButton(universalsToUse)
	{
		setText(s, s);


		f = universals->Fonts[getFontIndex()].getFont();
		colors = pmc;

		backgroundUp = colors->tagBKGD;
		backgroundDown = colors->tagBKGDSelected;

		textUp = colors->tagText;
		auto highlightFunc = [=](const juce::Colour& c)
		{

			return c.withAlpha(highlighted ? 1.0f : 0.8f);
		};

		backgroundUp.adjustmentFunction = highlightFunc;
		backgroundDown.adjustmentFunction = highlightFunc;
		textUp.adjustmentFunction = highlightFunc;
		textDown.adjustmentFunction = highlightFunc;




		setHasOutline(false);
		setClickingTogglesState(!isAdd);
		;
		add = isAdd;

		if (add)
		{
			te = std::make_unique<TextEditor>(universals);
//			te->setLookAndFeel(LNF);
			addChildComponent(te.get());
			te->setJustification(juce::Justification::centred);
			te->setInputRestrictions(BDSP_MAX_TAG_NAME_LENGTH);

			onClick = [=]()
			{
				te->setColour(juce::TextEditor::textColourId, universals->colors.getColor(textUp));
				te->setColour(juce::TextEditor::backgroundColourId, universals->colors.getColor(backgroundUp));
				te->setVisible(!te->isVisible());
				te->grabKeyboardFocus();
			};

		}
	}

	inline TagMenu::TagButton::TagButton(const TagButton& other)
		:TagButton(other.universals, other.onText,other.colors, other.add)
	{

	}

	inline TagMenu::TagButton::TagButton(TagButton&& other) noexcept
		:TagButton(other.universals, other.onText,other.colors, other.add)
	{

	}



	inline float TagMenu::TagButton::getIdealWidth(int h, const juce::String& s)
	{
		return  f.withHeight(h * 0.9).getStringWidthFloat((s.isEmpty() ? getText() : s) + "  ") * 1.05;
	}

	inline void TagMenu::TagButton::resized()
	{
		f = f.withHeight(getHeight() * 0.9);
		if (add)
		{
			te->setBounds(getLocalBounds());
			te->applyFontToAllText(f);
		}
	}

	inline void TagMenu::TagButton::paintOverChildren(juce::Graphics& g)
	{
		//if (highlighted)
		//{
		//	g.setColour(LNF->universals->colors.getColor(LNF->pc->tagBKGDSelected));
		//	g.drawRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve, universals->roundedRectangleCurve / 3);

		//	//g.fillRect(getLocalBounds().withTop(getHeight() - universals->roundedRectangleCurve / 3));
		//}
	}




} // namespace bdsp
