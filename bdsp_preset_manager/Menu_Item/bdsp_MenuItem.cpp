#include "bdsp_MenuItem.h"
namespace bdsp
{
	PresetMenuItem::PresetMenuItem(PresetMenu* parent, int idx, const juce::File& f)
		:ComponentCore(this, parent->parentManager->universals),
		favIcon(parent->parentManager->universals, true),
		label(this, parent->parentManager->universals),
		GUI_Universals::Listener(parent->parentManager->universals)
	{
		p = parent;
		index = idx;

		label.setEditable(false, false);
//		label.setLookAndFeel(LNF);
		label.setFontIndex(p->parentManager->pc.menuItemFontIndex);
		addAndMakeVisible(label);


		label.setJustificationType(juce::Justification::centred);


		updateFile(f);


		//================================================================================================================================================================================================




		Shape s(universals->commonPaths.star, p->parentManager->pc.tagBKGDSelected, juce::Rectangle<float>(0.05, 0.05, 0.9, 0.9));
		s.fillOff = p->parentManager->pc.emptyFav;

		favIcon.addShapes(s);




		favIcon.setToggleState(isFav(file), juce::sendNotification);
		favIcon.onClick = [this]()
		{
			p->favClick(this, favIcon.getToggleState());
		};

		addAndMakeVisible(favIcon, -1);


	}


	PresetMenuItem::PresetMenuItem(const PresetMenuItem& oldItem)
		:PresetMenuItem(oldItem.p, oldItem.index, oldItem.file)
	{

	}







	bool PresetMenuItem::operator == (const PresetMenuItem& other)
	{
		return file == other.file; // if files are the same items are the same
	}


	void PresetMenuItem::resized()
	{
		auto h = getHeight();
		auto shrink = h * 0.125;
		juce::Array<int> splits = p->getColumnSplits();

		favIcon.setBounds(juce::Rectangle<int>(h, h).reduced(shrink * 2).withCentre(juce::Point<int>(splits[0] / 2.0, h / 2.0)));

		int labelW;

		labelW = splits[1] - splits[0];


		label.setBounds(juce::Rectangle<int>(splits[0], 0, labelW, h).reduced(1.5 * shrink));

		jassert(superTagLabels.size() == splits.size() - 2);

		for (int i = 0; i < superTagLabels.size(); ++i)
		{
			//superTagLabels[i]->setBounds(juce::Rectangle<int>(splits[i+1], 0, i == superTagLabels.size() - 1 ? getWidth() - splits[i+1] : splits[i + 2] - splits[i+1], h).reduced(shrink));
			superTagLabels[i]->setBounds(juce::Rectangle<int>().leftTopRightBottom(splits[i + 1], 0, splits[i + 2], h).reduced(1.5 * shrink));
		}
	}

	void PresetMenuItem::mouseDown(const juce::MouseEvent& e)
	{
		bool c = e.mods.isCommandDown();
		bool s = e.mods.isShiftDown();

		if (e.mods.isPopupMenu()) // right click on WIN || cmd click on MAC
		{
			if (c || s)
			{
				p->modClick(this, c, s);
			}
			else if (!p->getHighlightQueue().contains(this))
			{
				p->getHighlightQueue().clear();
				p->getHighlightQueue().add(this);
				p->updateHighlights();
			}

			p->popupClick(e.getEventRelativeTo(&p->universalsPTR->desktopManager).getMouseDownPosition());
		}
		else
		{
			if (!(c || s))
			{
				p->singleClick(this);
			}
			else
			{
				p->modClick(this, c, s);
			}
		}

	}

	void PresetMenuItem::mouseDoubleClick(const juce::MouseEvent& e)
	{
		p->doubleClick(this);
	}

	juce::String PresetMenuItem::getLabelText()
	{
		return label.getText();
	}

	void PresetMenuItem::setHighlighted(bool state)
	{
		Highlighted = state;
		repaint();

	}

	bool PresetMenuItem::isHighlighted()
	{
		return Highlighted;
	}

	void PresetMenuItem::setFavorite(bool state)
	{
		p->parentManager->Handler->updatePreset(file, state);
		updateFile(file);
	}


	bool PresetMenuItem::isFavorite()
	{
		return favIcon.getToggleState();
	}

	void PresetMenuItem::paint(juce::Graphics& g)
	{
		g.setColour(getHoverColor(getColor(p->parentManager->pc.itemHighlight), getColor(p->parentManager->pc.browserBKGD).withAlpha(0.0f), Highlighted, isHovering(), universals->hoverMixAmt));

		auto inset = getHeight() * 0.125;
		//g.fillRect(inset, 0, getWidth() - 2 * inset, getHeight());
		g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(inset), universals->roundedRectangleCurve);

		/*g.setColour(getColor(p->parentManager->pc.searchBarBKGD));
		g.drawHorizontalLine(getHeight() , inset, getWidth() - inset);*/

		//if (file == p->parentManager->Handler->startupFile)
		//{
		//	g.setColour(p->parentManager->pc.universals->colors.getColor(p->parentManager->pc.tagBKGDSelected));

		//}
	}

	void PresetMenuItem::updateFile(const juce::File& newFile)
	{
		file = newFile;
		label.setText(newFile.getFileNameWithoutExtension(), juce::dontSendNotification);

		readTags(); // get tags from file, toggle them on and then add any others that aren't in the file

		for (auto l : listeners)
		{
			l->presetUpdated();
		}

	}

	void PresetMenuItem::readTags()
	{
		Tags.clear();
		superTagLabels.clear();
		auto xml = juce::parseXML(file);
		if (xml.get() != nullptr)
		{
			auto xmlTags = xml->getChildByName(TagsXMLTag);
			int num = xmlTags->getNumAttributes(); // get num tags in file
			for (int i = 0; i < num; ++i)
			{

				auto name = xmlTags->getAttributeName(i);

				if (xmlTags->getBoolAttribute(name))
				{
					Tags.add(XMLDataToString(name));
				}

			}

			auto xmlSuperTags = xml->getChildByName(SuperTagsXMLTag);
			num = xmlSuperTags->getNumAttributes();
			for (int i = 0; i < num; ++i)
			{
				auto name = xmlSuperTags->getAttributeName(i);
				auto s = xmlSuperTags->getStringAttribute(name);
				jassert(!s.isEmpty());

				superTagLabels.add(new Label(universals));
				superTagLabels.getLast()->setText(s, juce::dontSendNotification);
//				superTagLabels.getLast()->setLookAndFeel(LNF);
				superTagLabels.getLast()->setFontIndex(label.getFontIndex());
				superTagLabels.getLast()->setJustificationType(juce::Justification::centred);
				superTagLabels.getLast()->setInterceptsMouseClicks(false, false);
				addAndMakeVisible(superTagLabels.getLast());
			}

		}
		superTagLabels.add(new Label(universals));
		superTagLabels.getLast()->setText(file.getLastModificationTime().formatted("%x"), juce::dontSendNotification);
//		superTagLabels.getLast()->setLookAndFeel(LNF);
		superTagLabels.getLast()->setFontIndex(label.getFontIndex());
		superTagLabels.getLast()->setJustificationType(juce::Justification::centred);
		superTagLabels.getLast()->setInterceptsMouseClicks(false, false);
		addAndMakeVisible(superTagLabels.getLast());


	}

	void PresetMenuItem::setFile(const juce::File& newFile)
	{
		file = newFile;
	}

	juce::File PresetMenuItem::getFile()
	{
		return file;
	}

	juce::StringArray PresetMenuItem::getTags()
	{
		return Tags;
	}

	juce::OwnedArray<Label>* PresetMenuItem::getSuperTagLabels()
	{
		return &superTagLabels;
	}



	bool PresetMenuItem::testTags(const juce::StringArray& tagsToMatch)
	{
		bool out = true;

		for (auto s : tagsToMatch)
		{
			if (!Tags.contains(s))
			{
				out = false;
				break;
			}
		}
		return out;
	}

	juce::StringArray PresetMenuItem::getSuperTags()
	{
		juce::StringArray out;

		for (auto l : superTagLabels)
		{
			out.add(l->getText());
		}
		return out;
	}

	juce::String PresetMenuItem::getSuperTag(int tagIDX)
	{
		return superTagLabels[tagIDX]->getText();
	}



	bool PresetMenuItem::testSuperTags(const juce::StringArray& superTagsToMatch)
	{
		bool out = true;

		for (int i = 0; i < superTagLabels.size(); ++i)
		{
			if (superTagLabels[i]->getText().compare(superTagsToMatch[i]) != 0 && superTagsToMatch[i].isNotEmpty())
			{
				out = false;
				break;
			}
		}
		return out;
	}

	void PresetMenuItem::addListener(Listener* listenerToAdd)
	{
		listenerToAdd->setItem(this);
		listeners.add(listenerToAdd);
	}

	void PresetMenuItem::removeListener(Listener* listenerToRemove)
	{
		listenerToRemove->setItem(nullptr);
		listeners.removeAllInstancesOf(listenerToRemove);
	}
	void PresetMenuItem::GUI_UniversalsChanged()
	{
		b = getColor(p->parentManager->pc.browserBKGD).getBrightness();

		label.setColour(juce::Label::textColourId, getColor(p->parentManager->pc.presetText));
		label.setColour(juce::Label::textWhenEditingColourId, getColor(p->parentManager->pc.presetText).withMultipliedBrightness(1.25));


		label.setColour(juce::CaretComponent::caretColourId, getColor(p->parentManager->pc.presetText));

		label.setColour(juce::TextEditor::textColourId, getColor(p->parentManager->pc.presetText));
		label.setColour(juce::TextEditor::highlightedTextColourId, getColor(p->parentManager->pc.presetText).withMultipliedBrightness(1.25));
		label.setColour(juce::TextEditor::highlightColourId, getColor(p->parentManager->pc.presetText).withAlpha(universals->lowOpacity));
	}
	inline PresetMenuItem::PassthoughLabel::PassthoughLabel(PresetMenuItem* p, GUI_Universals* universalsToUse)
		:MousePassthrough<Label>(p, universalsToUse)
	{

	}
} // namespace bdsp
