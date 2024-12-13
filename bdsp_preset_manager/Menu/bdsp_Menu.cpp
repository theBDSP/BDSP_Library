#include "bdsp_Menu.h"
namespace bdsp
{
	PresetMenu::PresetMenu(PresetManager* p)
		:favColumnHeader(p->universals, true),
		vp(this),
		clearSearch(p->universals),
		GUI_Universals::Listener(p->universals),
		search(p->universals)
	{
		parentManager = p;


		tags = p->tagMenu.get();

		tags->addComponentListener(this);


		setWantsKeyboardFocus(true);

		setInterceptsMouseClicks(true, true);

		addAndMakeVisible(vp);
//		vp.setLookAndFeel(&parentManager->LNF);
		vp.setViewedComponent(&list, false);
		vp.setScrollBarsShown(true, false);

		addAndMakeVisible(search);

		search.setInputRestrictions(juce::jmax(BDSP_MAX_PRESET_NAME_LENGTH, BDSP_MAX_TAG_NAME_LENGTH));
		search.setPopupMenuEnabled(false);
//		search.setLookAndFeel(&parentManager->LNF);
		search.setJustification(juce::Justification::centred);


		search.onTextChange = [=]()
		{
			numItemsVisible = 0;
			auto text = search.getText();
			for (auto MI : items)
			{
				MI->setVisible(MI->testTags(selectedTags) && MI->testSuperTags(selectedSuperTags) && MI->label.getText().containsIgnoreCase(text));
				numItemsVisible += MI->isVisible() ? 1 : 0;
			}
			reSort(currentSorting.col, currentSorting.rev);

			clearSearch.setVisible(!text.isEmpty());
			search.setBounds(juce::Rectangle<int>().leftTopRightBottom(search.getX(), search.getY(), clearSearch.isVisible() ? clearSearch.getX() : clearSearch.getRight(), search.getBottom()));
			updateItemCountText();
		};




		//================================================================================================================================================================================================

		Shape X(universalsPTR->commonPaths.closeButton, p->pc.presetText, juce::Rectangle<float>(0.2, 0.2, 0.6, 0.6));

		clearSearch.addShapes(X);


		addChildComponent(clearSearch);

		clearSearch.onClick = [=]()
		{
			search.clear();
			search.onTextChange();
		};

		//================================================================================================================================================================================================





		Shape s(universalsPTR->commonPaths.star, parentManager->pc.tagBKGDSelected, juce::Rectangle<float>(0, 0, 1, 1));
		s.fillOff = parentManager->pc.emptyFav;
		favColumnHeader.addShapes(s);

		favColumnHeader.onClick = [=]()
		{
			reSort(currentSorting.col, currentSorting.rev);
			if (favColumnHeader.getToggleState())
			{
				goToItem(0);
			}
		};

		addAndMakeVisible(favColumnHeader);

		for (int i = 0; i < parentManager->Handler->superTagNames.size() + 2; i++)
		{
			columnHeaders.add(new TextButton(parentManager->universals));

			columnHeaders.getLast()->backgroundDown = NamedColorsIdentifier();
			columnHeaders.getLast()->backgroundUp = NamedColorsIdentifier();
			columnHeaders.getLast()->textUp = NamedColorsIdentifier();
			columnHeaders.getLast()->textDown = NamedColorsIdentifier();
			columnHeaders.getLast()->setHasOutline(false);

			columnHeaders.getLast()->setText((i == parentManager->Handler->superTagNames.size() + 1) ? "Date" : (i == 0 ? "Name" : parentManager->Handler->superTagTitles[i - 1]));

			addAndMakeVisible(columnHeaders.getLast());

			columnHeaders.getLast()->onClick = [=]()
			{
				bool rev = false;
				if (currentSorting.col == i)
				{
					rev = !currentSorting.rev;
				}
				reSort(i, rev);
				repaint();
			};
		}
		reBuildMenu();

		Popup = std::make_unique<PopupMenu>(this, p->universals);
		Popup->List.onSelect = [=](int i)
		{
			switch (i)
			{
			case PresetItemActions::OSDirectory:
				highlightQueue.isEmpty() ? p->pFolder->getPresetFolder().revealToUser() : highlightQueue.getLast()->getFile().revealToUser();
				break;

			case PresetItemActions::Refresh:
				p->Handler->rebuildMasterFile();
				reBuildMenu();
				break;

			case PresetItemActions::Delete:
				deleteItems(highlightQueue);
				break;

			case PresetItemActions::Rename:
				rename(highlightQueue.getLast());
				break;

			case PresetItemActions::Overwrite:

				p->overwritePreset(highlightQueue.getLast());
				break;

			case PresetItemActions::SetAsDefault:
				setDefaultPreset(highlightQueue.getLast());
				break;

			case PresetItemActions::ReInstall:
				reInstallFactoryPresets();
				break;
			}
		};



		GUI_UniversalsChanged();

		resetHighlights();
	}

	PresetMenu::~PresetMenu()
	{
		vp.setLookAndFeel(nullptr);
		search.setLookAndFeel(nullptr);

		tags->removeComponentListener(this);
	}

	void PresetMenu::resized()
	{
		auto searchRect = getLocalBounds().getProportion(juce::Rectangle<float>(0.05, 0.04, 0.9, 0.04));
		search.setBounds(searchRect.withTrimmedRight(clearSearch.isVisible() ? searchRect.getHeight() : 0));
		search.applyFontToAllText(parentManager->universals->Fonts[parentManager->pc.searchBarFontIndex].getFont().withHeight(search.getHeight() * 0.9));
		search.setBorder(juce::BorderSize<int>(parentManager->universals->roundedRectangleCurve / 4));

		clearSearch.setBounds(searchRect.withWidth(search.getHeight()).withRightX(searchRect.getRight()));

		tags->calculateNewSize(juce::Rectangle<int>(0, search.getBottom() + search.getY(), getWidth(), tagMenuH).reduced(0, universalsPTR->roundedRectangleCurve));

		goToItem(pendingGoToItem);


	}

	void PresetMenu::goToItem(PresetMenuItem* MI)
	{
		if (MI != nullptr)
		{
			vp.setViewPosition(MI->getPosition());
		}
		pendingGoToItem = nullptr;
	}


	void PresetMenu::goToItem(int idx)
	{
		goToItem(items[idx]);
	}

	void PresetMenu::keepItemInSafeArea(PresetMenuItem* MI)
	{

		auto prevPosition = vp.getViewPosition();

		while (MI->getY() > vp.getViewPositionY() + safeRect.getBottom())
		{
			vp.setViewPosition(0, vp.getViewPositionY() + MI->getHeight());
			if (vp.getViewPosition() == prevPosition)
			{
				break;
			}
			prevPosition = vp.getViewPosition();
		}
		while (MI->getBottom() < vp.getViewPositionY() + safeRect.getY())
		{
			vp.setViewPosition(0, vp.getViewPositionY() - MI->getHeight());

			if (vp.getViewPosition() == prevPosition)
			{
				break;
			}
			prevPosition = vp.getViewPosition();
		}
	}

	void PresetMenu::reBuildMenu(bool performOnLoadFunction)
	{
		const auto& loadedPresetFile = parentManager->loadedPreset == nullptr ? juce::File() : parentManager->loadedPreset->getFile();
		juce::Array<juce::File> highlightFiles;
		for (auto MI : highlightQueue)
		{
			highlightFiles.add(MI->getFile());
		}

		highlightQueue.clear();

		items.clear();
		numItemsVisible = 0;
		const auto& files = parentManager->pFolder->getPresetFolder().findChildFiles(juce::File::findFiles, true, "*.bdspreset");
		numItems = files.size();
		for (int i = 0; i < files.size(); i++)
		{
			items.add(new PresetMenuItem(this, i, files[i]));
			auto currentItem = items.getLast();
			list.addAndMakeVisible(currentItem);

			int idx = highlightFiles.indexOf(files[i]);

			if (idx != -1)
			{
				highlightQueue.set(idx, currentItem); // re-adds item to highlight queue 
			}

		}
		numItemsVisible = items.size();
		updateItemCountText();

		//tagFilter(tags->getCurrentTags(), tags->getCurrentSuperTags());
		reSort(currentSorting.col, currentSorting.rev);


		parentManager->loadPreset(findItem(loadedPresetFile), performOnLoadFunction);
		tags->rebuildMenu();
		tags->reFilter();
		updateHighlights();
	}

	void PresetMenu::reSort(int columnIdx, bool reversed)
	{
		ColumnSorter sorter(columnIdx, reversed, favColumnHeader.getToggleState());
		items.sort(sorter, true);
		updateIndecies();
		currentSorting = { columnIdx,reversed, favColumnHeader.getToggleState() };

		goToItem(highlightQueue.getLast());
	}



	void PresetMenu::tagFilter(const juce::StringArray& currentTags, const juce::StringArray& currentSuperTags)
	{
		selectedTags = currentTags;
		selectedSuperTags = currentSuperTags;

		search.onTextChange();
		//reSort(currentSorting.col, currentSorting.rev);

		if (highlightQueue.size() == 1)
		{
			if (highlightQueue.getLast()->isVisible())
			{
				tags->highlightTags(highlightQueue.getLast()->getSuperTags(), highlightQueue.getLast()->getTags());
			}
			else
			{
				tags->clearTagHighlights();
			}
		}

		for (auto MI : items)
		{
			if (!MI->isVisible() && highlightQueue.contains(MI))
			{
				highlightQueue.removeFirstMatchingValue(MI);
			}
		}

		updateHighlights();
	}

	void PresetMenu::updateItem(int index, const juce::File& newFile)
	{
		items[index]->updateFile(newFile);
		reSort(currentSorting.col, currentSorting.rev);
	}




	PresetMenuItem* PresetMenu::findItem(const juce::File& file)
	{
		PresetMenuItem* out = nullptr;
		for (auto mi : items)
		{
			if (mi->getFile() == file)
			{
				out = mi;
				break;
			}
		}
		return out;
	}

	PresetMenuItem* PresetMenu::findItem(const juce::String& name)
	{
		return findItem(parentManager->pFolder->getPresetFolder().getChildFile(name + ".bdspreset"));
	}

	int PresetMenu::findItemIndex(const juce::File& file)
	{
		auto mi = findItem(file);
		return mi == nullptr ? -1 : mi->index;
	}

	void PresetMenu::setItemFontIndex(int index)
	{
		parentManager->pc.menuItemFontIndex = index;


		for (auto MI : items)
		{
			MI->label.setFontIndex(index);
			for (auto l : *MI->getSuperTagLabels())
			{
				l->setFontIndex(index);
			}
		}
	}





	void PresetMenu::deleteItems(juce::Array<PresetMenuItem*>& itemsToDelete)
	{

		std::function<void(int)> func = [=](int i)
		{
			if (i == 2)
			{
				bool success = false;
				for (auto MI : itemsToDelete)
				{
					success = deleteItemInternal(MI) || success;
				}

				if (success)
				{
					parentManager->Handler->rebuildMasterFile();
					parentManager->alert->hide();
				}
			}
			else
			{
				parentManager->alert->hide();
			}
		};


		auto header = juce::String("About to Delete ");
		header.append((itemsToDelete.size() == 1 ? "a Preset" : "Presets"), -1);

		auto body = juce::String("Please confirm that you want to delete ");
		body.append((itemsToDelete.size() == 1 ? "this preset" : "these presets"), -1);

		parentManager->alert->addItemToQueue(AlertWindowItem("Cancel", "", "Confirm", header, body, func));



	}

	bool PresetMenu::deleteItemInternal(PresetMenuItem* MI)
	{
		MI->getFile().setReadOnly(false);
		if (MI->getFile().deleteFile())
		{

			if (parentManager->loadedPreset == MI)
			{
				parentManager->loadedPreset = nullptr;
				parentManager->Handler->presetName.setValue("init");
				parentManager->Handler->alteredState.setValue(true);
			}
			highlightQueue.removeAllInstancesOf(MI);
			items.removeObject(MI);
			updateIndecies();
			updateHighlights();


			return true;
		}
		else
		{
			parentManager->alert->addMessageToQueue("Ok", "Could Not Delete " + MI->getName(), BDSP_APP_NAME + juce::String(" was unable to delete the preset ") + MI->getName());
			return false;
		}
	}

	void PresetMenu::GUI_UniversalsChanged()
	{
		search.setColour(juce::TextEditor::backgroundColourId, juce::Colour());
		search.setColour(juce::TextEditor::outlineColourId, juce::Colour());
		search.setColour(juce::TextEditor::textColourId, parentManager->universals->colors.getColor(parentManager->pc.presetText));
		search.setColour(juce::TextEditor::highlightedTextColourId, parentManager->universals->colors.getColor(parentManager->pc.presetText));
		search.setColour(juce::TextEditor::highlightColourId, parentManager->universals->colors.getColor(parentManager->pc.presetText).withAlpha(parentManager->universals->lowOpacity));
		search.setColour(juce::CaretComponent::caretColourId, parentManager->universals->colors.getColor(parentManager->pc.browserBKGD));
		search.setInputRestrictions(juce::jmax(BDSP_MAX_PRESET_NAME_LENGTH, BDSP_MAX_TAG_NAME_LENGTH));

//		search.setLookAndFeel(&parentManager->LNF);
		search.setTextToShowWhenEmpty("Search", parentManager->universals->colors.getColor(parentManager->pc.browserBKGD).interpolatedWith(parentManager->universals->colors.getColor(parentManager->pc.presetText), 0.5));
		search.setJustification(juce::Justification::centred);



		//Popup->setColour(juce::Label::backgroundColourId, parentManager->universals->colors.getColor(BDSP_COLOR_LIGHT));
		//Popup->setColour(juce::Label::backgroundWhenEditingColourId, parentManager->universals->colors.getColor(BDSP_COLOR_DARK));
		//Popup->setColour(juce::Label::outlineColourId, parentManager->universals->colors.getColor(parentManager->pc.outline));
		//Popup->setColour(juce::Label::textColourId, parentManager->universals->colors.getColor(parentManager->pc.presetText));
	}

	void PresetMenu::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
	{
		auto itemH = getWidth() * itemHRatio;
		//auto itemB = itemH * 0.25;

		vp.setBounds(getLocalBounds().withTop(tags->getBottom() + 2 * search.getY()).withBottom(getHeight() - search.getY()));

		vp.setScrollBarThickness(parentManager->scrollBarW);

		vp.setScrollDistance(itemH);

		int i = 0;
		for (; i < items.size(); i++) // set bounds of all visible items
		{
			if (!items[i]->isVisible()) // reched end of visible items
			{
				break;
			}

			items[i]->setBounds(0, i * itemH, vp.getWidth() - 1.5 * vp.getScrollBarThickness(), itemH);
		}



		list.setBounds(0, 0, vp.getWidth(), (items.isEmpty() || i < 1) ? vp.getHeight() : items[i - 1]->getBottom());

		safeRect = vp.getLocalBounds().reduced(0, 2 * itemH);

		if (!highlightQueue.isEmpty())
		{
			goToItem(highlightQueue.getLast());
		}

		auto splits = getColumnSplits();
		favColumnHeader.setBounds(juce::Rectangle<int>().leftTopRightBottom(0, vp.getY() - itemH * 0.75, splits[0], vp.getY()).withTrimmedBottom(itemH / 10));

		for (int i = 0; i < columnHeaders.size(); i++)
		{
			columnHeaders[i]->setBounds(juce::Rectangle<int>().leftTopRightBottom(splits[i], favColumnHeader.getY(), splits[i + 1], vp.getY()));
		}

	}

	void PresetMenu::updateIndecies()
	{
		for (int i = 0; i < items.size(); i++)
		{
			items[i]->index = i;
		}
		resized();
		componentMovedOrResized(*tags, false, true);
	}



	void PresetMenu::paint(juce::Graphics& g)
	{
		g.setColour(parentManager->universals->colors.getColor(parentManager->pc.menuBKGD));
		g.fillRoundedRectangle(vp.getBounds().toFloat().withTrimmedRight(1.5 * vp.getScrollBarThickness()), parentManager->universals->roundedRectangleCurve);

		g.setColour(parentManager->universals->colors.getColor(parentManager->pc.searchBarBKGD));
		g.fillRoundedRectangle(juce::Rectangle<int>(search.getPosition(), clearSearch.getBounds().getBottomRight()).toFloat().expanded(parentManager->universals->roundedRectangleCurve / 2), parentManager->universals->roundedRectangleCurve);



		//drawDivider(g, juce::Line<float>(vp.getX(), vp.getY(), vp.getRight() - 1.5 * vp.getScrollBarThickness(), vp.getY()), parentManager->universals->colors.getColor(parentManager->LNF.pc->searchBarBKGD), parentManager->universals->dividerSize);
		//drawDivider(g, juce::Line<float>(vp.getX(), vp.getBottom(), vp.getRight() - 1.5 * vp.getScrollBarThickness(), vp.getBottom()), parentManager->universals->colors.getColor(parentManager->LNF.pc->searchBarBKGD), parentManager->universals->dividerSize);

	}

	void PresetMenu::paintOverChildren(juce::Graphics& g)
	{
		//g.setColour(parentManager->universals->colors.getColor(parentManager->pc.searchBarBKGD));
		//g.drawRoundedRectangle(juce::Rectangle<int>(search.getPosition(), clearSearch.getBounds().getBottomRight()).toFloat(), parentManager->universals->roundedRectangleCurve, parentManager->universals->roundedRectangleCurve / 5);

		//================================================================================================================================================================================================


		//g.setColour(parentManager->universals->colors.getColor(parentManager->LNF.pc->presetText));
		//g.drawHorizontalLine(vp.getY(), parentManager->universals->roundedRectangleCurve, getWidth() - parentManager->universals->roundedRectangleCurve);


		juce::Path tri;
		auto h = sqrt(3);
		tri.addTriangle(0, 0, -1, -h, 1, -h);

		if (currentSorting.rev)
		{
			tri.applyTransform(juce::AffineTransform().rotated(PI));
		}

		g.setFont(parentManager->universals->Fonts[1].getFont().withHeight(favColumnHeader.getHeight() * 0.9));
		for (int i = 0; i < columnHeaders.size(); i++)
		{
			/*	ga.clear();
				ga.addFittedText(g.getCurrentFont(), columnHeaders[i]->getText(), columnHeaders[i]->getX(), columnHeaders[i]->getY(), columnHeaders[i]->getWidth(), columnHeaders[i]->getHeight(), juce::Justification::centred, 1);

				header.clear();
				ga.createPath(header);

				g.fillPath(header);*/


			if (i == currentSorting.col)
			{
				g.setColour(parentManager->universals->colors.getColor(parentManager->pc.sortedColumnText));
				auto header = drawText(g, g.getCurrentFont(), columnHeaders[i]->getText(), columnHeaders[i]->getBounds());
				h = g.getCurrentFont().getHeight();
				scaleToFit(tri, juce::Rectangle<int>(header.getBounds().getX() - h, header.getBounds().getY(), h, header.getBounds().getHeight()).withSizeKeepingCentre(h / 2.0, h / 2.0));
				g.fillPath(tri);
			}
			else
			{
				g.setColour(parentManager->universals->colors.getColor(parentManager->pc.presetText));
				auto header = drawText(g, g.getCurrentFont(), columnHeaders[i]->getText(), columnHeaders[i]->getBounds());

			}
		}
	}

	void PresetMenu::mouseDown(const juce::MouseEvent& e)
	{

		//PresetMenuItem* MI = highlightQueue.isEmpty() ? nullptr : highlightQueue.getLast();
		if (e.mods.isPopupMenu())
		{
			popupClick(e.getEventRelativeTo(&universalsPTR->desktopManager).getMouseDownPosition());
		}
		else
		{
			resetHighlights();
			hideAllPopupMenus();
			if (parentManager->loadedPreset != nullptr)
			{
				toggleSingleHighlight(parentManager->loadedPreset); // reselect loaded preset
			}

		}
		search.giveAwayKeyboardFocus();
	}

	void PresetMenu::mouseDoubleClick(const juce::MouseEvent& e)
	{
		PresetMenuItem* MI = highlightQueue.isEmpty() ? nullptr : highlightQueue.getLast();
		if (MI != nullptr)
		{
			if (e.mods.isCommandDown())
			{
				rename(highlightQueue.getLast());
			}
			else
			{
				doubleClick(MI);
			}
		}
	}

	void PresetMenu::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{
		//bool multi = e.mods.isShiftDown();
		//if (!mw.isReversed)
		//{
		//	if (mw.deltaY < 0)
		//	{
		//		scroll(false, multi);
		//	}
		//	else if (mw.deltaY > 0)
		//	{
		//		scroll(true, multi);
		//	}
		//}
		//else
		//{
		//	if (mw.deltaY < 0)
		//	{
		//		scroll(true, multi);
		//	}
		//	else if (mw.deltaY > 0)
		//	{
		//		scroll(false, multi);
		//	}
		//}
	}

	bool PresetMenu::keyPressed(const juce::KeyPress& key)
	{
		auto code = key.getKeyCode();
		auto mods = key.getModifiers();


		auto lastHighlighted = highlightQueue.getLast();
		if (code == key.deleteKey)
		{
			deleteItems(highlightQueue);
		}
		else if (code == key.upKey)
		{
			scroll(true, mods.isShiftDown());
		}
		else if (code == key.downKey)
		{
			scroll(false, mods.isShiftDown());
		}
		else if (code == key.returnKey || code == key.rightKey)
		{
			if (mods.isCommandDown())
			{
				popupClick(lastHighlighted->getBoundsRelativeToDesktopManager().getCentre());
			}
			else
			{
				doubleClick(lastHighlighted);
			}

		}

		else if (code == key.F2Key)
		{
			rename(lastHighlighted);
		}




		return true;
	}

	void PresetMenu::scroll(bool up, bool multi)
	{

		auto size = items.size();
		if (size > 1)
		{
			PresetMenuItem* MI = parentManager->loadedPreset;
			int idx = 0;
			if (MI != nullptr && highlightQueue.size() <= 1)
			{
				idx = MI->index;
			}
			else if (highlightQueue.isEmpty())
			{
				idx = up ? 0 : -1;
			}
			else
			{
				idx = highlightQueue.getLast()->index;
			}

			MI = items[idx];
			for (int i = 1; i < size; i++)
			{
				int newIDX = (up ? idx + size - i : idx + i) % size;
				if (items[newIDX]->isVisible())
				{
					MI = items[newIDX];
					break;
				}
			}


			if (multi)
			{

				if (!MI->isHighlighted())
				{
					modClick(MI, true, false);
					keepItemInSafeArea(MI);
				}
			}
			else
			{
				previewItem(MI);
			}


		}
		else
		{
			previewItem(items.getFirst());
		}
	}



	void PresetMenu::singleClick(PresetMenuItem* MI)
	{
		previewItem(MI);
		Popup->hide();
	}

	void PresetMenu::modClick(PresetMenuItem* MI, bool ctrl, bool shift)
	{
		if (shift)
		{
			groupHighlight(MI, ctrl);
		}
		else
		{
			toggleSingleHighlight(MI);
		}
		Popup->hide();

	}

	void PresetMenu::popupClick(juce::Point<int> mouseDownPosition)
	{

		Popup->hide();

		createPopupMenu(mouseDownPosition, !highlightQueue.isEmpty());
	}

	void PresetMenu::doubleClick(PresetMenuItem* MI)
	{
		parentManager->loadPreset(MI);
		Popup->hide();

	}

	void PresetMenu::favClick(PresetMenuItem* MI, bool state)
	{
		parentManager->updateFavorite(MI, state);
		Popup->hide();

	}

	juce::Array<int> PresetMenu::getColumnSplits()
	{
		juce::Array<int> out;


		out.add(getHeight() * 0.1); // fav

		auto remainder = getWidth() - 1.5 * vp.getScrollBarThickness() - out.getFirst();

		auto w = remainder / (columnHeaders.size() + 1);

		out.add(out.getFirst() + 2 * w);

		for (int i = 1; i < columnHeaders.size(); i++)
		{
			out.add(out.getLast() + w);
		}

		//jassert(out.getLast() == getWidth());

		return out;
	}

	void PresetMenu::resetHighlights()
	{
		highlightQueue.clear();
		updateHighlights();
	}

	void PresetMenu::hideAllPopupMenus()
	{
		Popup->hide();
	}

	void PresetMenu::previewItem(PresetMenuItem* MI, bool perfromOnLoadFunction, bool isUndoable)
	{
		highlightQueue.clear();
		highlightQueue.add(MI);
		parentManager->loadPreset(MI, perfromOnLoadFunction, isUndoable);

		updateHighlights();

		keepItemInSafeArea(MI);
	}

	void PresetMenu::toggleSingleHighlight(PresetMenuItem* MI)
	{
		if (!MI->isHighlighted()) // needs to be highlighted
		{
			highlightQueue.addIfNotAlreadyThere(MI);
		}
		else
		{
			highlightQueue.removeAllInstancesOf(MI);
		}
		updateHighlights();
	}

	void PresetMenu::groupHighlight(PresetMenuItem* MI, bool Ctrl)
	{

		if (!Ctrl)
		{
			auto temp = highlightQueue.isEmpty() ? items.getFirst() : highlightQueue.getFirst();
			highlightQueue.clear();
			highlightQueue.add(temp);
		}
		if (!highlightQueue.isEmpty())
		{
			auto first = juce::jmin(highlightQueue.getLast()->index, MI->index);
			auto last = juce::jmax(highlightQueue.getLast()->index, MI->index);
			for (auto i = first; i <= last; i++)
			{
				highlightQueue.addIfNotAlreadyThere(items[i]);
			}

		}
		else
		{
			previewItem(MI);
		}

		hideAllPopupMenus();

		updateHighlights();
	}



	void PresetMenu::updateHighlights()
	{
		for (auto MI : items)
		{
			MI->setHighlighted(highlightQueue.contains(MI));
		}

		if (highlightQueue.size() == 1 && highlightQueue.getLast() != nullptr)
		{
			tags->highlightTags(highlightQueue.getLast()->getSuperTags(), highlightQueue.getLast()->getTags());
		}
		else
		{
			tags->clearTagHighlights();
		}
		updateItemCountText();
	}

	void PresetMenu::createPopupMenu(juce::Point<int>& clickPos, bool highlighted)
	{
		//		auto wRat = 1 / 1.5;
		//		auto hRat = 1 / 6.0;
		juce::StringArray names;
		juce::Array<int> returnValues;


		//=====================================================================================================================
		// add items common to both regular popups




		names.add("Open in File Explorer");
		returnValues.add(PresetItemActions::OSDirectory);

		names.add("Refresh");
		returnValues.add(PresetItemActions::Refresh);


		//=====================================================================================================================

		if (!highlightQueue.isEmpty())
		{
			names.insert(1, "Delete");
			returnValues.insert(1, PresetItemActions::Delete);

		}


		if (highlightQueue.size() == 1)
		{
			names.insert(2, "Rename");
			returnValues.insert(2, PresetItemActions::Rename);

			names.add("Overwrite Preset");
			returnValues.add(PresetItemActions::Overwrite);

			names.add("Set as Default Preset");
			returnValues.add(PresetItemActions::SetAsDefault);
		}


		names.add("Re-Install Factory Presets");
		returnValues.add(PresetItemActions::ReInstall);

		Popup->List.setItems(names, returnValues);

		Popup->List.setColorSchemeClassic(BDSP_COLOR_DARK, NamedColorsIdentifier(), parentManager->pc.presetText, NamedColorsIdentifier(BDSP_COLOR_LIGHT));


		auto w = parentManager->browser.getWidth() * parentManager->pc.menuPopupWidthRatio;
		auto h = Popup->List.getIdealHeight(w);
		int x = clickPos.x;
		int y = clickPos.y;
		if (clickPos.x + w > universalsPTR->desktopManager.getWidth())
		{
			x -= w;
		}
		if (clickPos.y + h > universalsPTR->desktopManager.getHeight())
		{
			y -= h;
		}
		Popup->setBoundsContained(expandRectangleToInt(juce::Rectangle<float>(x, y, w, h)), 0);
		Popup->show();

	}

	void PresetMenu::rename(PresetMenuItem* MI)
	{

		MI->label.showEditor();
		MI->label.setEditable(false, false, true);
		MI->label.getCurrentTextEditor()->setInputRestrictions(BDSP_MAX_PRESET_NAME_LENGTH, BDSP_PRESET_NAME_ALLOWED_CHARACTERS);

		MI->label.onEditorHide = [=]()
		{
			auto newFile = parentManager->pFolder->getPresetFolder().getChildFile(MI->label.getText() + ".bdspreset");
			if (newFile.exists() && newFile != MI->getFile())
			{
				std::function<void(int)> func = [=](int)
				{
					MI->label.showEditor();
					parentManager->alert->hide();
				};
				AlertWindowItem item("", "", "Ok", "Invalid Preset Name", "This preset name already exists, please choose another one", func);
				parentManager->alert->addItemToQueue(item);

			}
			else if (newFile.getFileName().compare(newFile.getFileExtension()) == 0) // empty name
			{
				MI->label.setText(MI->getFile().getFileNameWithoutExtension(), juce::sendNotification);
				parentManager->alert->addMessageToQueue("Ok", "Invalid Preset Name", "The new preset name you enter cannot be empty");
			}
			else if (newFile != MI->getFile())
			{
				if (parentManager->Handler->renamePreset(MI->getFile(), newFile))
				{
					MI->setFile(newFile);
					reSort(currentSorting.col, currentSorting.rev);
					if (parentManager->loadedPreset == MI)
					{
						parentManager->Handler->presetName.setValue(newFile.getFileNameWithoutExtension());
					}
					universalsPTR->undoManager->clearUndoHistory();
				}
				else
				{
					MI->label.setText(MI->getFile().getFileNameWithoutExtension(), juce::sendNotification);
					parentManager->alert->addMessageToQueue("Ok", "Could Not Rename " + MI->getName(), BDSP_APP_NAME + juce::String(" was unable to rename the preset ") + MI->getName());
				}
			}
		};
	}

	juce::Array<PresetMenuItem*>& PresetMenu::getHighlightQueue()
	{
		return highlightQueue;
	}

	void PresetMenu::setDefaultPreset(PresetMenuItem* MI)
	{
		parentManager->Handler->setStartupPreset(MI->getFile());
	}

	void PresetMenu::reInstallFactoryPresets()
	{

		std::function<void(int)> func = [=](int i)
		{
			if (i != 0)
			{
				parentManager->Handler->reInstallFactoryPresets(i == 2);
			}

			parentManager->alert->hide();
		};


		auto header = juce::String("Overwrite Changes to Factory Presets?");

		auto body = juce::String("Would you like to overwrite any changes made to factory presets, or only reinstall those that are missing?");

		parentManager->alert->addItemToQueue(AlertWindowItem("Cancel", "Keep Changes", "Overwrite", header, body, func));

	}

	void PresetMenu::initToProperItem()
	{
		PresetMenuItem* MI = nullptr;
		if (parentManager->loadedPreset != nullptr)
		{
			MI = parentManager->loadedPreset;
		}
		else if (!highlightQueue.isEmpty())
		{
			MI = highlightQueue.getLast();
		}

		//================================================================================================================================================================================================


		if (getBounds().isEmpty())
		{
			pendingGoToItem = MI;
		}
		else
		{
			goToItem(MI);
		}
	}

	void PresetMenu::updateItemCountText()
	{

		parentManager->numItems = juce::String(numItemsVisible) + " of ";
		parentManager->totalItems = displayCount(numItems, "preset");
		if (!highlightQueue.isEmpty())
		{
			parentManager->totalItems += " (" + juce::String(highlightQueue.size()) + " Selected)";
		}
		parentManager->browser.repaint();
	}





	//================================================================================================================================================================================================

	PresetMenu::passthroughVP::passthroughVP(PresetMenu* parent)
		:Viewport(parent->universalsPTR)
	{
		p = parent;

	}




	bool PresetMenu::passthroughVP::keyPressed(const juce::KeyPress& key)
	{
		return p->keyPressed(key);
	}

	void PresetMenu::passthroughVP::paint(juce::Graphics& g)
	{

		/*	auto inset = p->items.getFirst()->getHeight() * 0.125;
			g.setColour(getColor(p->parentManager->pc.searchBarBKGD));

			for (int i = 0; i < p->items.size() - 1; i++)
			{
				auto y = (p->items[i]->getBottom() + p->items[i + 1]->getY()) / 2.0;
				y -= getViewPositionY();
				if (juce::isPositiveAndBelow(y, getHeight()))
				{
					g.drawHorizontalLine(y, inset, getWidth() - 1.5 * getScrollBarThickness() - inset);
				}
				else
				{
					break;
				}
			}*/

	}

} // namespace bdsp
