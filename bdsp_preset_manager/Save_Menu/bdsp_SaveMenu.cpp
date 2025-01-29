#include "bdsp_SaveMenu.h"
namespace bdsp
{
	SaveMenu::SaveMenu(PresetManager* parent)
        :DesktopComponent(parent->universals, true),
        GUI_Universals::Listener(parent->universals),
        fav(parent->universals, true),
        cancel(parent->universals),
        save(parent->universals),
        nameBar(parent->universals),
        vp(parent->universals)
	{

		p = parent;

		//p->universals->desktopManager.addComponent(this);

		p->alert->addComponentListener(this);

		setLossOfFocusClosesWindow(false);

		//================================================================================================================================================================================================




		centerRow = [=](juce::OwnedArray<TagMenu::TagButton>& arr, int startIdx, int endIdx)
		{

			auto totalW = 0;
			for (int i = startIdx; i <= endIdx; ++i)
			{
				totalW += arr[i]->getWidth() + border / 3;
			}
			totalW -= border / 3;

			int x = (vp.getWidth() - vp.getScrollBarThickness() - totalW) / 2.0;
			for (int i = startIdx; i <= endIdx; ++i)
			{
				arr[i]->setTopLeftPosition(x, arr[i]->getY());
				x = arr[i]->getRight() + border / 3;
			}
		};


		//================================================================================================================================================================================================

		addAndMakeVisible(vp);

		vp.setScrollBarsShown(true, false);
		//		vp.setLookAndFeel(&p->LNF);

		vp.addMouseListener(this, true);

		for (int i = 0; i < p->Handler->superTagTitles.size() + 1; ++i)
		{
			tagSections.add(new juce::Component());
			tagSectionuniversals.addAndMakeVisible(tagSections.getLast());
		}

		vp.setViewedComponent(&tagSectionuniversals);
		rebuildTags();



		//================================================================================================================================================================================================

		addAndMakeVisible(nameBar);



		nameBar.setInputRestrictions(BDSP_MAX_PRESET_NAME_LENGTH, BDSP_PRESET_NAME_ALLOWED_CHARACTERS);
		nameBar.setJustification(juce::Justification::centredLeft);
		nameBar.background = p->pc.saveMenuTagBKGD;
		onShow = [=]()
		{
			parent->alert->resize();
			setBounds(parent->alert->getBounds());
			resized();
			nameBar.grabKeyboardFocus();
		};

		//================================================================================================================================================================================================


		Shape s(universals->commonPaths.star, p->pc.tagBKGDSelected, juce::Rectangle<float>(0, 0, 1, 1));
		s.fillOff = p->pc.emptyFav;

		fav.addShapes(s);

		addAndMakeVisible(fav);
		//================================================================================================================================================================================================






		cancel.setText("Cancel");

		cancel.textUp = p->pc.saveMenuTagText;
		cancel.textDown = p->pc.saveMenuTagText;

		cancel.backgroundUp = p->pc.saveMenuTagBKGD;
		cancel.backgroundDown = p->pc.saveMenuTagBKGDSelected;

		cancel.setHasOutline(false);

		cancel.onClick = [=]()
		{
			//clear();
			hide();
		};
		addAndMakeVisible(cancel);


		//================================================================================================================================================================================================


		save.setText("Save");


		save.textUp = cancel.textUp;
		save.textDown = cancel.textDown;

		save.backgroundUp = cancel.backgroundUp;
		save.backgroundDown = cancel.backgroundDown;
		save.setHasOutline(false);


		save.onClick = [=]()
		{
			auto text = nameBar.getText().trimStart().trimEnd();
			std::function<void(int)> invalidFunc = [=](int i)
			{
				show();
			};

			if (text.compareIgnoreCase("init") == 0)
			{
				AlertWindowItem invalidNameItem("", "", "Ok", "Invalid Preset Name", "Preset name cannot be 'init'", invalidFunc);

				p->alert->addItemToQueue(invalidNameItem);
			}
			else if (text.isEmpty())
			{
				AlertWindowItem emptyNameItem("", "", "Ok", "Invalid Preset Name", "Preset name cannot be empty", invalidFunc);

				p->alert->addItemToQueue(emptyNameItem);
			}
			else
			{

				const auto& file = p->Handler->pluginFolder->getPresetFolder().getChildFile(text + ".bdspreset");

				std::function<void(int)> overwriteFunc = [=](int i)
				{
					if (i == 2)
					{
						juce::StringArray tags, superTags;
						for (auto t : tagButtons)
						{
							if (t->getToggleState())
							{
								tags.add(t->getText());
								pendingTagButtons.removeAllInstancesOf(t);
							}
						}

						for (int k = 0; k < superTagButtons.size(); k++)
						{
							auto arr = superTagButtons.getUnchecked(k);
							for (int j = 0; j < arr->size(); ++j)
							{
								auto curr = arr->getUnchecked(j);
								if (curr->getToggleState())
								{
									superTags.add(curr->getText());
									pendingSuperTagButtons.removeAllInstancesOf(curr);
									break;
								}
							}
						}
						p->Handler->savePreset(file, fav.getToggleState(), tags, superTags);
						clear();
						hide();

						p->menu->reBuildMenu(false);
						p->menu->previewItem(p->menu->findItem(file), false);
						p->universals->undoManager->clearUndoHistory();

						p->Handler->alteredState.setValue(false);

					}
					else
					{
						show();
					}
					p->alert->hide();
				};
				if (file.exists())
				{
					AlertWindowItem overwriteItem("No", "", "Yes", "Overwrite Preset?", "This preset already exists. Would you like to overwrite it?", overwriteFunc);
					p->alert->addItemToQueue(overwriteItem);
				}
				else
				{
					overwriteFunc(2);
				}
			}
		};
		addAndMakeVisible(save);

		onHide = [=]()
		{
			createHandlerSaveMenuState();
		};

		//================================================================================================================================================================================================





	}

	SaveMenu::~SaveMenu()
	{
		if (p->alert.get() != nullptr)
		{
			p->alert->removeComponentListener(this);
		}

		vp.removeMouseListener(this);


		createHandlerSaveMenuState();



	}

	void SaveMenu::clear()
	{
		nameBar.clear();
		fav.setToggleState(false, juce::dontSendNotification);

		for (auto tb : pendingTagButtons)
		{
			tagButtons.removeObject(tb);
		}
		pendingTagButtons.clear();

		for (auto tb : tagButtons)
		{
			tb->setToggleState(false, juce::dontSendNotification);
		}
		tagButtons.getLast()->te->onEscapeKey();

		for (int i = 0; i < superTagButtons.size(); ++i)
		{
			for (auto st : pendingSuperTagButtons)
			{
				superTagButtons[i]->removeObject(st);
			}
			auto arr = superTagButtons.getUnchecked(i);
			for (int j = 0; j < arr->size(); ++j)
			{
				arr->getUnchecked(j)->setToggleState(false, juce::dontSendNotification);
			}
			arr->getLast()->te->onEscapeKey();
		}
		pendingSuperTagButtons.clear();

	}

	void SaveMenu::resized()
	{
		auto leftEdge = getWidth() * 0.2;
		auto h = getHeight() * 0.15;
		titleH = h * 0.75;
		border = h * 0.25;




		nameBar.setBounds(juce::Rectangle<int>().leftTopRightBottom(leftEdge, 2 * border + titleH, getWidth() - 2 * border, 2 * titleH + 2 * border).reduced(0, border / 2));
		nameBar.applyFontToAllText(p->universals->Fonts[p->pc.saveMenuNameBarFontIndex].getFont().withHeight(nameBar.getHeight() * 0.9));







		//	vp.setBounds(juce::Rectangle<int>().leftTopRightBottom(leftEdge, nameBar.getBottom() + borderColor, getWidth() - borderColor, cancel.getY() - borderColor));

		vp.setScrollBarThickness(p->scrollBarW);

		//================================================================================================================================================================================================

		float y;
		h = titleH / 1.25;
		int col = 0; // column || tag section index

		int rowStart = 0, rowEnd = 0;
		for (int i = 0; i < superTagButtons.size(); ++i)
		{
			auto arr = superTagButtons.getUnchecked(i);
			y = 0;

			auto text = arr->getUnchecked(i)->te == nullptr ? arr->getUnchecked(0)->getText() : arr->getUnchecked(0)->te->getText();

			auto w = arr->getUnchecked(0)->getIdealWidth(h, text);
			arr->getUnchecked(0)->setBounds(border, y, w, h);

			for (int j = 1; j < arr->size(); ++j)
			{
				auto curr = arr->getUnchecked(j);
				text = curr->te == nullptr ? curr->getText() : curr->te->getText();

				w = curr->getIdealWidth(h, text);
				curr->setBounds(arr->getUnchecked(j - 1)->getRight() + border / 3, y, w, h);
				if (curr->getRight() > vp.getWidth() - vp.getScrollBarThickness())
				{
					rowEnd = j - 1;
					centerRow(*arr, rowStart, rowEnd);
					y += h + border / 3;
					curr->setTopLeftPosition(0, y);
					rowStart = j;
				}

			}
			centerRow(*arr, rowStart, arr->size() - 1);
			tagSections[col]->setSize(vp.getWidth(), arr->getLast()->getBottom() + border);
			col++;
			rowStart = 0;
			rowEnd = 0;
		}


		//================================================================================================================================================================================================

		if (!tagButtons.isEmpty())
		{
			y = 0;
			auto text = tagButtons[0]->te == nullptr ? tagButtons[0]->getText() : tagButtons[0]->te->getText();
			auto w = tagButtons[0]->getIdealWidth(h, text);

			tagButtons[0]->setBounds(0, y, w, h);
			for (int i = 1; i < tagButtons.size(); ++i)
			{

				text = tagButtons[i]->te == nullptr ? tagButtons[i]->getText() : tagButtons[i]->te->getText();
				w = tagButtons[i]->getIdealWidth(h, text);

				tagButtons[i]->setBounds(tagButtons[i - 1]->getRight() + border / 3, y, w, h);

				if (tagButtons[i]->getRight() > vp.getWidth() - vp.getScrollBarThickness())
				{
					rowEnd = i - 1;
					centerRow(tagButtons, rowStart, rowEnd);
					rowStart = i;

					y += h + border / 3;
					tagButtons[i]->setTopLeftPosition(0, y);
				}
			}
			centerRow(tagButtons, rowStart, tagButtons.size() - 1);
			tagSections[col]->setSize(vp.getWidth(), tagButtons.getLast()->getBottom() + border);

		}

		//================================================================================================================================================================================================


		y = 0;
		for (int i = 0; i < tagSections.size(); ++i)
		{
			tagSections[i]->setTopLeftPosition(0, y);
			y = tagSections[i]->getBottom() + titleH;
		}

		tagSectionuniversals.setBounds(juce::Rectangle<int>(vp.getWidth(), tagSections.getLast()->getBottom()));

		vp.setBounds(juce::Rectangle<int>().leftTopRightBottom(leftEdge, nameBar.getBottom() + 2 * border, getWidth() - 2 * border, getHeight() - 2 * titleH - 2 * border));

		vp.setScrollDistance(h + border / 3);


		//================================================================================================================================================================================================

		auto f = p->universals->Fonts[cancel.getFontIndex()].getFont().withHeight(h * 0.9);
		auto buttonW = getWidth() * 0.175;

		cancel.setBounds(2 * border, getHeight() - 1.2 * titleH, buttonW, 0.9 * titleH);
		save.setBounds(getWidth() - 2 * border - buttonW, getHeight() - 1.2 * titleH, buttonW, 0.9 * titleH);

		fav.setBounds(juce::Rectangle<int>(cancel.getHeight(), cancel.getHeight()).withCentre(juce::Point<int>(getWidth() / 2, cancel.getBounds().getCentreY())));
		//================================================================================================================================================================================================

		//setBounds(getBounds().withHeight(save.getBottom() + borderColor));

	}


	void SaveMenu::matchItem(PresetMenuItem* MI)
	{
		clear();
		fav.setToggleState(MI->isFavorite(), juce::dontSendNotification);

		nameBar.setText(MI->label.getText());

		auto tags = MI->getTags();
		for (auto tb : tagButtons)
		{
			if (tags.contains(tb->getText()))
			{
				tb->setToggleState(true, juce::dontSendNotification);
			}
		}

		for (int i = 0; i < superTagButtons.size(); ++i)
		{
			auto superTag = MI->getSuperTag(i);
			auto arr = superTagButtons.getUnchecked(i);
			for (int j = 0; j < arr->size(); ++j)
			{
				if (arr->getUnchecked(j)->getText().compare(superTag) == 0)
				{
					arr->getUnchecked(j)->setToggleState(true, juce::dontSendNotification);
					break;
				}
			}
		}
		createHandlerSaveMenuState();
	}

	void SaveMenu::matchPreviousState(SaveMenuState* prevState)
	{


		clear();
		fav.setToggleState(prevState->favortie, juce::dontSendNotification);

		nameBar.setText(prevState->presetName);

		for (auto t : prevState->activeTags)
		{

			for (auto tb : tagButtons)
			{
				if (t.compare(tb->getText()) == 0)
				{
					tb->setToggleState(true, juce::dontSendNotification);
				}
			}
		}

		for (auto t : prevState->activePendingTags)
		{
			tagButtons.add(new TagMenu::TagButton(p->universals, t, &p->pc));
			tagButtons.getLast()->setToggleState(true, juce::dontSendNotification);

			tagSections.getFirst()->addAndMakeVisible(tagButtons.getLast());

			pendingTagButtons.add(tagButtons.getLast());
		}
		for (auto t : prevState->inactivePendingTags)
		{
			tagButtons.add(new TagMenu::TagButton(p->universals, t, &p->pc));
			tagButtons.getLast()->setToggleState(false, juce::dontSendNotification);

			tagSections.getFirst()->addAndMakeVisible(tagButtons.getLast());

			pendingTagButtons.add(tagButtons.getLast());
		}

		TagButtonComparator comp;
		tagButtons.sort(comp, true);

	}

	void SaveMenu::createHandlerSaveMenuState(bool isUndoable)
	{

		SaveMenuState newState;
		newState.presetName = nameBar.getText();
		newState.favortie = fav.getToggleState();



		for (auto tb : tagButtons)
		{
			if (p->Handler->tagNames.contains(tb->getText()))
			{
				if (tb->getToggleState())
				{
					newState.activeTags.add(tb->getText());
				}
			}
			else if (tb != tagButtons.getLast())
			{
				if (tb->getToggleState())
				{
					newState.activePendingTags.add(tb->getText());
				}
				else
				{
					newState.inactivePendingTags.add(tb->getText());
				}
			}
		}

		if (isUndoable)
		{
			p->Handler->replaceSaveMenuState(newState);
		}
		else
		{
			p->Handler->currentSaveMenuState = newState;
		}

	}

	void SaveMenu::rebuildTags()
	{
		std::function<void(TagMenu::TagButton*)> setButtonColors = [=](TagMenu::TagButton* b)
		{
			b->backgroundUp = p->pc.saveMenuTagBKGD;
			b->backgroundDown = p->pc.saveMenuTagBKGDSelected;

			b->textUp = p->pc.saveMenuTagText;

			b->setHasOutline(false);
		};

		tagButtons.clear();
		for (auto& tn : p->Handler->tagNames)
		{
			tagButtons.add(new TagMenu::TagButton(p->universals, tn, &p->pc));
			tagSections.getFirst()->addAndMakeVisible(tagButtons.getLast());

			setButtonColors(tagButtons.getLast());
		}

		tagButtons.add(new TagMenu::TagButton(p->universals, " + ", &p->pc, true));
		setButtonColors(tagButtons.getLast());

		tagSections.getFirst()->addAndMakeVisible(tagButtons.getLast());

		//tagButtons.getLast()->backgroundUp = p->pc.emptyFav;
		tagButtons.getLast()->te->onEscapeKey = [=]()
		{
			tagButtons.getLast()->te->clear();
			tagButtons.getLast()->te->setVisible(false);
			tagButtons.getLast()->setBounds(tagButtons.getLast()->getBounds().withWidth(tagButtons.getLast()->getIdealWidth(tagButtons.getLast()->getHeight())));
		};


		tagButtons.getLast()->te->onTextChange = [=]()
		{
			auto last = tagButtons.getLast();
			last->setSize(last->getIdealWidth(last->getHeight(), last->te->getText()), last->getHeight());

			resized();
			vp.setViewPosition(0, vp.getViewedComponent()->getBottom() - vp.getHeight());

		};


		tagButtons.getLast()->te->onReturnKey = [=]()
		{
			auto last = tagButtons.getLast();
			auto trimmed = last->te->getText().trim();
			bool conflict = trimmed == "+";

			AlertWindowItem item;
			item.SelectFunction = [=](int i)
			{
				p->alert->hide();
				last->te->onEscapeKey();
				show();
			};

			item.RightName = "Ok";
			item.HeaderText = "Invalid Tag Name";
			if (conflict)
			{
				item.BodyText = "Tag name cannot be '+' with no other characters";

				p->alert->addItemToQueue(item);
				//p->alert->addMessageToQueue("Ok", "Invalid Tag Name", "Tag name cannot be '+' with no other characters");
			}
			else
			{
				for (int i = 0; i < tagButtons.size() - 1; ++i)
				{
					if (tagButtons[i]->getText().compare(trimmed) == 0)
					{
						item.BodyText = "This tag already exists, please choose another name or use the existing tag";
						p->alert->addItemToQueue(item);

						//p->alert->addMessageToQueue("Ok", "Invalid Tag Name", "This tag already exists, please choose another name or use the existing tag");
						conflict = true;
						break;
					}
				}
			}

			if (!conflict)
			{
				if (trimmed.isEmpty())
				{
					last->te->onEscapeKey();
				}
				else
				{
					last->te->setVisible(false);

					tagButtons.add(new TagMenu::TagButton(p->universals, trimmed, &p->pc));
					setButtonColors(last);

					tagButtons.getLast()->setToggleState(true, juce::sendNotification);
					pendingTagButtons.add(tagButtons.getLast());

					tagSections.getFirst()->addAndMakeVisible(tagButtons.getLast());
					tagButtons.getUnchecked(tagButtons.size() - 2)->te->clear();

					TagButtonComparator comp;
					tagButtons.sort(comp, true);
					resized();

					int vpY = 0;
					for (auto b : tagButtons)
					{
						if (b->getText().compare(trimmed) == 0)
						{
							vpY = b->getY();
							break;
						}
					}
					vp.setViewPosition(0, vpY);

				}
			}
		};

		tagButtons.getLast()->te->onFocusLost = [=]()
		{
			tagButtons.getLast()->te->keyPressed(juce::KeyPress(juce::KeyPress::returnKey));
		};
		//================================================================================================================================================================================================


		resized();
	}

	void SaveMenu::paint(juce::Graphics& g)
	{
		juce::PathStrokeType stroke(universals->roundedRectangleCurve / 4);
		juce::Path bkgd, bkgdTop, bkgdBot;
		bkgd.addRoundedRectangle(getLocalBounds().toFloat().reduced(stroke.getStrokeThickness()), p->universals->roundedRectangleCurve);
		bkgdTop.addRoundedRectangle(0, 0, getWidth(), titleH, universals->roundedRectangleCurve, universals->roundedRectangleCurve, true, true, false, false);
		bkgdBot.addRoundedRectangle(0, getHeight() - 1.5 * titleH, getWidth(), 1.5 * titleH, universals->roundedRectangleCurve, universals->roundedRectangleCurve, false, false, true, true);


		g.setColour(getColor(p->pc.saveMenuBKGD));
		g.fillPath(bkgd);

		g.setColour(getColor(p->pc.saveMenuBKGDTop));
		g.fillPath(bkgdTop);
		drawDivider(g, juce::Line<float>(stroke.getStrokeThickness(), titleH, getWidth() - stroke.getStrokeThickness(), titleH), p->universals->colors.getColor(BDSP_COLOR_LIGHT), p->universals->dividerSize);

		g.setColour(getColor(p->pc.saveMenuBKGDBot));
		g.fillPath(bkgdBot);
		drawDivider(g, juce::Line<float>(stroke.getStrokeThickness(), getHeight() - 1.5 * titleH, getWidth() - stroke.getStrokeThickness(), getHeight() - 1.5 * titleH).reversed(), p->universals->colors.getColor(BDSP_COLOR_LIGHT), p->universals->dividerSize);

		g.setColour(getColor(p->pc.outline));
		g.strokePath(bkgd, stroke);

		//================================================================================================================================================================================================


		static const unsigned char savePathData[] = { 110,109,185,121,65,196,207,125,230,68,108,185,121,65,196,110,1,232,68,108,208,212,66,196,109,1,232,68,108,66,27,65,196,202,127,233,68,108,179,97,63,196,109,1,232,68,108,202,188,64,196,108,1,232,68,108,202,188,64,196,205,125,230,68,99,109,105,225,68,196,
39,154,233,68,108,105,225,68,196,159,248,233,68,108,26,85,61,196,160,248,233,68,108,26,85,61,196,40,154,233,68,99,101,0,0 };

		juce::Path savePath;
		savePath.loadPathFromData(savePathData, sizeof(savePathData));


		juce::Path textPath;

		auto titleRect = getLocalBounds().withHeight(titleH);
		textPath = createTextPath(p->universals->Fonts[p->titleBar.getFontIndex()].getFont().withHeight(titleH * 0.9), "Save Preset", titleRect);

		auto textPathBounds = textPath.getBounds();
		scaleToFit(savePath, juce::Rectangle<float>(textPathBounds.getHeight(), textPathBounds.getHeight()).withCentre(juce::Point<float>(textPathBounds.getX() - titleH / 2 - textPathBounds.getHeight() / 2, textPathBounds.getCentreY())));

		textPath.addPath(savePath);

		scaleToFit(textPath, titleRect.withSizeKeepingCentre(getWidth(), textPathBounds.getHeight()));

		g.setColour(getColor(p->pc.saveMenuTitle));


		g.fillPath(textPath);


		//================================================================================================================================================================================================

		//g.drawHorizontalLine((nameBar.getBottom() + vp.getY()) / 2, borderColor, getWidth() - borderColor);

		auto y = nameBar.getBottom() + border;
		drawDivider(g, juce::Line<float>(2 * border, y, getWidth() - 2 * border, y), p->universals->colors.getColor(BDSP_COLOR_LIGHT), p->universals->dividerSize);

		g.setColour(getColor(p->pc.saveMenuLabel));
		g.setFont(p->universals->Fonts[p->titleBar.getFontIndex()].getFont().withHeight(titleH * 0.9));
		//g.setFont(g.getCurrentFont().withHeight(titleH * 0.9));

		auto rect = juce::Rectangle<float>(2 * border, nameBar.getY(), vp.getX() - border, nameBar.getHeight());

		drawText(g, g.getCurrentFont(), "Name", rect, false, juce::Justification::centredLeft);


		//ga.addJustifiedText(g.getCurrentFont(), "Name", rect.getX(), rect.getCentreY(), rect.getWidth(), juce::Justification::centredLeft);

		//ga.draw(g);	


		/*	ga.clear();
			ga.addJustifiedText(g.getCurrentFont(), "Fav", rect.getX(), rect.getCentreY(), rect.getWidth(), juce::Justification::centredLeft);

			ga.draw(g);*/

		auto viewY = vp.getViewPositionY();

		for (int i = 0; i < tagSections.size(); ++i)
		{
			juce::String text;

			if (i < tagSections.size() - 1) //SuperTag
			{
				text = p->Handler->superTagTitles[i];
			}
			else
			{
				text = "Tags";
			}

			auto top = tagSections[i]->getY();
			auto bot = tagSections[i]->getBottom();

			if (bot - viewY > titleH)
			{

				rect = juce::Rectangle<float>(2 * border, juce::jlimit((double)vp.getY(), vp.getBottom() - titleH, (double)top - viewY), vp.getX() - border, titleH);

				drawText(g, g.getCurrentFont(), text, rect, false, juce::Justification::centredLeft);

				g.setColour(getColor(p->pc.saveMenuTitle));

				if (i != tagSections.size() - 1)
				{
					//g.drawHorizontalLine(bot - viewY + vp.getY(), borderColor, getWidth() - borderColor);
					drawDivider(g, juce::Line<float>(2 * border, bot - viewY + vp.getY() + border / 2, getWidth() - 2 * border, bot - viewY + vp.getY() + border / 2), p->universals->colors.getColor(BDSP_COLOR_LIGHT), p->universals->dividerSize);

				}


			}
			//juce::GlyphArrangement ga;
			//ga.addJustifiedText(g.getCurrentFont(), textUp, rect.getX(), rect.getCentreY(), rect.getWidth(), juce::Justification::centredLeft);

			//ga.draw(g);


			//g.drawHorizontalLine(ga.getBoundingBox(0, -1, true).getBottom() - g.getCurrentFont().getDescent() + borderColor / 3, 2 * borderColor, getWidth() - 2 * borderColor);
		}

		//g.setColour(getColor(p->pc.saveMenuTitle));
		////g.drawHorizontalLine(cancel.getY() - borderColor / 2, borderColor, getWidth() - borderColor);
		//drawDivider(g, juce::Line<float>(2 * borderColor, cancel.getY() - borderColor / 2, getWidth() - 2 * borderColor, cancel.getY() - borderColor / 2), p->universals->colors.getColor(BDSP_COLOR_LIGHT), p->universals->dividerSize);




	}

	void SaveMenu::paintOverChildren(juce::Graphics& g)
	{
		if (darken)
		{
			g.setColour(juce::Colours::black.withAlpha(BDSP_OVERLAY_ALPHA));
			g.fillRoundedRectangle(getLocalBounds().toFloat().reduced((border - p->universals->roundedRectangleCurve) / 2), p->universals->roundedRectangleCurve);
		}
	}

	void SaveMenu::mouseDown(const juce::MouseEvent& e)
	{
		unfocusAllComponents();
	}




	void SaveMenu::centerSection(juce::OwnedArray<TagMenu::TagButton>& arr, int col)
	{
		float y = 0;

		int rowStart = 0, rowEnd = 0;
		float rowW = arr.getFirst()->getWidth() + border;


		for (int j = 1; j < arr.size(); ++j)
		{
			auto curr = arr.getUnchecked(j);
			rowW += curr->getWidth() + border / 3;
			if (rowW > vp.getWidth())
			{
				rowEnd = j - 1;
				centerRow(arr, rowStart, rowEnd);
				y += curr->getHeight() + border / 3;
				curr->setTopLeftPosition(0, y);
				rowStart = j;
				rowW = curr->getWidth() + border;
			}
			else if (curr->getY() > arr[j - 1]->getY())
			{
				rowStart = j;
				y += curr->getHeight() + border / 3;
				rowW = curr->getWidth() + border;
			}

		}
		centerRow(arr, rowStart, arr.size() - 1);
		tagSections[col]->setBounds(vp.getLocalBounds().withBottom(arr.getLast()->getBottom() + border / 2));
		rowStart = 0;
		rowEnd = 0;
	}

	void SaveMenu::componentVisibilityChanged(juce::Component& c)
	{
		if (&c == p->alert)
		{
			darken = c.isVisible();
			repaint();
		}
	}

	void SaveMenu::GUI_UniversalsChanged()
	{
		nameBar.setColour(juce::TextEditor::textColourId, getColor(p->pc.saveMenuTagText));
		nameBar.setColour(juce::TextEditor::highlightedTextColourId, getColor(p->pc.saveMenuTagText));


		nameBar.setColour(juce::TextEditor::highlightColourId, getColor(p->pc.saveMenuTagBKGDSelected).withAlpha(0.25f));
		nameBar.setColour(juce::CaretComponent::caretColourId, getColor(p->pc.saveMenuTagBKGDSelected));
	}





} // namespace bdsp
