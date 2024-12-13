#include "bdsp_SettingsMenu.h"

constexpr double settingsHeightRatio = 1.5;
constexpr double SettingsFontH = 0.7;


#ifndef JucePlugin_VersionString

#define JucePlugin_VersionString ""

#endif // ! JucePlugin_VersionString


namespace bdsp
{


	SettingsMenu::SettingsMenu(GUI_Universals* universalsToUse, Settings* structToLink, const juce::File& userManual)
		:ComponentCore(this, universalsToUse),
		manual(universalsToUse),
		vp(universalsToUse)
	{
		;
		settingsStruct = structToLink;
		manualLocation = userManual;


		//manual.setTwoColorScheme(BDSP_COLOR_COLOR, BDSP_COLOR_BLACK);
		manual.backgroundUp = BDSP_COLOR_WHITE;
		manual.backgroundDown = BDSP_COLOR_COLOR;

		manual.textUp = BDSP_COLOR_BLACK;
		manual.textDown = BDSP_COLOR_BLACK;


		manual.setHasOutline(false);

		manual.setText("Open Manual");

		manual.onClick = [&]()
		{
			manualLocation.startAsProcess();
		};
		addAndMakeVisible(manual);


		//================================================================================================================================================================================================


		addAndMakeVisible(vp);
		vp.setViewedComponent(&settingsuniversals);

		vp.setScrollBarsShown(true, false);
		for (auto setting : settingsStruct->arr)
		{
			if (setting->isDropDown)
			{
				addDropDown(setting->description, setting->category, setting->options, setting->dropDownMenuW);
			}
			else
			{
				addButtons(setting->description, setting->options, setting->category);
			}

		}

		setFontIndex(1);
	}

	void SettingsMenu::SettingsMenu::load(const juce::File& settingsFile, const juce::File& universalSettingsFile)
	{
		settingsStruct->loadSettings(settingsFile, universalSettingsFile);
		for (auto s : settings)
		{
			s->setValue(settingsStruct->get(s->descriptionText.upToLastOccurrenceOf(":", false, true)));
		}
		for (auto kb : dropDowns)
		{
			kb->getComboBox()->selectItem(settingsStruct->get(kb->descriptionText.upToLastOccurrenceOf(":", false, true)));
		}

	}

	void SettingsMenu::SettingsMenu::addButtons(const juce::Identifier& description, const juce::StringArray& options, const juce::String& category)
	{
		settings.add(new SettingComponent(this, description.toString(), category));
		settingsuniversals.addAndMakeVisible(settings.getLast());
		settings.getLast()->addButtons(options);

		compOrder.add(settings.getLast());
	}



	void SettingsMenu::addDropDown(const juce::Identifier& description, const juce::String& category, const juce::StringArray& options, float menuW)
	{
		dropDowns.add(new DropDownSettingsComponent(this, description.toString(), category, options, menuW));
		settingsuniversals.addAndMakeVisible(dropDowns.getLast());

		universals->bindings.add(juce::ModifierKeys::noModifiers);

		compOrder.add(dropDowns.getLast());

	}

	void SettingsMenu::SettingsMenu::setExclusiveSelectionGroup(const juce::Array<int>& indecies)
	{
		juce::Array<SettingComponent*> buttonGroup;
		juce::Array<DropDownSettingsComponent*> dropDownGroup;
		for (auto i : indecies)
		{
			auto* button = dynamic_cast<SettingComponent*>(compOrder[i]);
			auto* dropdown = dynamic_cast<DropDownSettingsComponent*>(compOrder[i]);
			if (button != nullptr)
			{
				buttonGroup.add(button);
			}
			else if (dropdown != nullptr)
			{
				dropDownGroup.add(dropdown);
			}
		}

		for (int i = 0; i < buttonGroup.size(); ++i)
		{
			auto copy = buttonGroup;
			copy.removeAllInstancesOf(buttonGroup[i]);
			buttonGroup[i]->setGroup(copy);
		}

		for (int i = 0; i < dropDownGroup.size(); ++i)
		{
			auto copy = dropDownGroup;
			copy.removeAllInstancesOf(dropDownGroup[i]);
			dropDownGroup[i]->setGroup(copy);
		}
	}

	int SettingsMenu::SettingsMenu::getSettingValue(const juce::Identifier& ID)
	{
		return settingsStruct->get(ID);
	}



	void SettingsMenu::SettingsMenu::resized()
	{
		auto h = getWidth() / 15.0;
		border = h / 5;
		vp.setBounds(border, 0, getWidth() - 2 * border, getHeight() - 1.5 * h);
		vp.setScrollDistance(h);
		int j = 0;
		juce::String cat;
		for (int i = 0; i < settings.size(); ++i)
		{
			if (settings[i]->categoryText != cat)
			{
				++j;
				cat = settings[i]->categoryText;
			}
			settings[i]->setBounds(vp.getX() + border, i * 1.25 * h + j * CategoryHScale * h, vp.getWidth() - 2 * border, h);
		}

		for (int i = 0; i < dropDowns.size(); ++i)
		{
			if (dropDowns[i]->categoryText != cat)
			{
				++j;
				cat = dropDowns[i]->categoryText;
			}
			dropDowns[i]->setBounds(vp.getX() + border, (settings.size() + i) * 1.25 * h + j * CategoryHScale * h, vp.getWidth() - 2 * border, h);
		}

		auto settingsBot = settings.getLast() == nullptr ? 0 : settings.getLast()->getBottom();
		auto bindingsBot = dropDowns.getLast() == nullptr ? 0 : dropDowns.getLast()->getBottom();

		settingsuniversals.setBounds(0, 0, vp.getWidth(), juce::jmax(settingsBot, bindingsBot));


		manual.setBounds(juce::Rectangle<int>(0, vp.getBottom(), getWidth() / 2.0, getHeight() - vp.getBottom()).reduced(border));
		manual.setSize(manual.getTextWidth() * 1.25, manual.getHeight());
	}

	void SettingsMenu::SettingsMenu::paint(juce::Graphics& g)
	{



		//g.drawHorizontalLine(vp.getBottom(), borderColor, getWidth() - borderColor);
		auto y = vp.getBottom() - universals->dividerSize;
		bdsp::drawDivider(g, juce::Line<float>(border, y, getWidth() - border, y), getColor(BDSP_COLOR_LIGHT), universals->dividerSize);


		g.setColour(getColor(BDSP_COLOR_WHITE));
		g.setFont(resizeFontToFit(universals->Fonts[getFontIndex()].getFont(), manual.getWidth(), manual.getHeight(), juce::String("Version ") + JucePlugin_VersionString));
		drawText(g, g.getCurrentFont(), juce::String("Version ") + JucePlugin_VersionString, manual.getBounds().withRight(getWidth() - border), false, juce::Justification::centredRight);


	}

	void SettingsMenu::paintOverChildren(juce::Graphics& g)
	{

		auto catH = CategoryHScale * settings.getFirst()->getHeight();
		g.setColour(getColor(BDSP_COLOR_WHITE));





		g.setFont(universals->Fonts[getFontIndex()].getFont().withHeight(catH / 2));

		juce::String cat;
		for (int i = 0; i < settings.size(); ++i)
		{
			if (settings[i]->categoryText != cat)
			{
				cat = settings[i]->categoryText;

				drawText(g, g.getCurrentFont(), cat, juce::Rectangle<int>(vp.getX(), settings[i]->getY() - catH - vp.getViewPositionY(), vp.getWidth(), catH * 0.9), false, juce::Justification::bottomLeft);

				int idx = compOrder.indexOf(settings[i]);
				if (idx != 0)
				{
					auto y = (compOrder[idx]->getY() + compOrder[idx - 1]->getBottom()) / 2.0 + vp.getY() - vp.getViewPositionY() - catH / 2;
					bdsp::drawDivider(g, juce::Line<float>(border, y, getWidth() - 2 * border, y), getColor(BDSP_COLOR_LIGHT), universals->dividerSize);
				}

			}

		}

		for (int i = 0; i < dropDowns.size(); ++i)
		{
			if (dropDowns[i]->categoryText != cat)
			{
				cat = dropDowns[i]->categoryText;

				drawText(g, g.getCurrentFont(), cat, juce::Rectangle<int>(vp.getX(), dropDowns[i]->getY() - vp.getViewPositionY() - catH, vp.getWidth(), catH * 0.9), false, juce::Justification::bottomLeft);

				int idx = compOrder.indexOf(dropDowns[i]);
				if (idx != 0)
				{
					auto y = (compOrder[idx]->getY() - g.getCurrentFont().getHeight() + compOrder[idx - 1]->getBottom()) / 2.0 + vp.getY() - vp.getViewPositionY();
					bdsp::drawDivider(g, juce::Line<float>(border, y, getWidth() - 2 * border, y), getColor(BDSP_COLOR_LIGHT), universals->dividerSize);

				}

			}

		}

	}

	void SettingsMenu::linkButton(Button* b)
	{
		b->addListener(this);
	}

	void SettingsMenu::buttonClicked(juce::Button* b)
	{
		setVisible(b->getToggleState());
	}






	//=====================================================================================================================

	SettingsMenu::SettingsMenu::SettingComponent::SettingComponent(SettingsMenu* parent, const juce::String& description, const juce::String& category)
		:ComponentCore(this, parent->universals)
	{
		p = parent;
		descriptionText = description;
		categoryText = category;
		setFontIndex(1);
	}

	void SettingsMenu::SettingsMenu::SettingComponent::addButtons(const juce::StringArray& options)
	{

		for (int i = 0; i < options.size(); ++i)
		{
			buttons.add(new TextButton(p->universals));
			auto current = buttons.getLast();
			current->setText(options[i]);
			current->setClickingTogglesState(false);

			current->setHasOutline(false);
			current->onClick = [=]()
			{
				setValue(i);
			};

			//current->setTwoColorScheme(BDSP_COLOR_COLOR, BDSP_COLOR_BLACK);
			current->backgroundDown = p->manual.backgroundDown;
			current->backgroundUp = p->manual.backgroundUp;
			current->textDown = p->manual.textDown;
			current->textUp = p->manual.textUp;


			current->getMaxText = [=]()
			{
				auto font = p->universals->Fonts[buttons[i]->getFontIndex()];
				juce::String maxText;
				float max = 0;
				for (auto& o : options)
				{
					auto textW = font.getFont().getStringWidthFloat(o);
					if (textW > max)
					{
						max = textW;
						maxText = o;
					}
				}
				return maxText;
			};

			addAndMakeVisible(current);
		}
		//setValue(selectedOption);


	}

	void SettingsMenu::SettingsMenu::SettingComponent::setGroup(const juce::Array<SettingComponent*>& restOfGroup)
	{
		othersInGroup = restOfGroup;
	}


	void SettingsMenu::SettingsMenu::SettingComponent::setValue(int idx)
	{
		for (auto b : buttons)
		{
			b->setToggleState(false, juce::dontSendNotification);
		}

		buttons[idx]->setToggleState(true, juce::dontSendNotification);

		for (auto c : othersInGroup)
		{
			for (int i = 0; i < c->buttons.size(); ++i)
			{
				c->buttons[i]->setEnabled(i != idx);
			}
		}

		p->settingsStruct->set(descriptionText.upToLastOccurrenceOf(":", false, true), idx);
		p->settingsStruct->saveSettings();


	}

	void SettingsMenu::SettingsMenu::SettingComponent::resized()
	{

		auto w = getWidth() / buttons.size();

		auto borderW = w * 0.05;
		auto midX = getWidth() / 3.0;


		w = (getWidth() - midX - (buttons.size() + 1) * borderW) / buttons.size();

		for (int i = 0; i < buttons.size(); ++i)
		{
			buttons[i]->getComponent()->setBounds(midX + i * w + (i + 1) * borderW, 0, w, getHeight());
		}
	}

	void SettingsMenu::SettingsMenu::SettingComponent::paint(juce::Graphics& g)
	{
		g.setColour(getColor(BDSP_COLOR_WHITE));
		//g.setFont(resizeFontToFit(p->universals->Fonts[getFontIndex()].getFont(), getWidth() * 0.9, (buttons.getFirst()->getHeight() + getHeight() / 2.0) / 2.0, descriptionText));
		g.setFont(p->universals->Fonts[getFontIndex()].getFont().withHeight(getHeight() * SettingsFontH));
		drawText(g, g.getCurrentFont(), descriptionText, juce::Rectangle<float>(0, 0, getWidth() / 3.0, getHeight()), false, juce::Justification::centredLeft);
	}


	//================================================================================================================================================================================================


	SettingsMenu::SettingsMenu::DropDownSettingsComponent::DropDownSettingsComponent(SettingsMenu* parent, const juce::String& description, const juce::String& category, const juce::StringArray& options, float menuW)
		:Component(parent->universals)
	{
		p = parent;
		descriptionText = description;
		categoryText = category;
		setFontIndex(1);

		std::function<void(int)> cbFunc = [=](int i)
		{
			setValue(i);
		};

		cb = std::make_unique<ComboBox>(options, nullptr, p->universals, juce::StringArray(), cbFunc);
		cb->setFontIndex(0);
		//cb->setColors(BDSP_COLOR_WHITE, BDSP_COLOR_BLACK, BDSP_COLOR_WHITE, BDSP_COLOR_WHITE, BDSP_COLOR_BLACK, BDSP_COLOR_MID, BDSP_COLOR_WHITE);
		cb->setColorSchemeClassic(BDSP_COLOR_KNOB, NamedColorsIdentifier(), NamedColorsIdentifier(BDSP_COLOR_BLACK), NamedColorsIdentifier(BDSP_COLOR_COLOR).withMultipliedAlpha(universals->lowOpacity));

		//	cb->setColors(BDSP_COLOR_WHITE, p->manual.backgroundUp, p->manual.textUp);
		///*	auto mid = p->manual.backgroundUp;
		//	mid.adjustmentFunction = [=](juce::Colour c)
		//	{
		//		return c.interpolatedWith(getColor(p->manual.textUp), 0.5);
		//	};*/
		//	cb->Menu.setColors(p->manual.backgroundUp, BDSP_COLOR_MID, p->manual.textUp);
			//cb->setMenuWidth(menuW);
		menuWidth = menuW;
		addAndMakeVisible(cb.get());




	}

	void SettingsMenu::SettingsMenu::DropDownSettingsComponent::setGroup(const juce::Array<DropDownSettingsComponent*>& restOfGroup)
	{
		othersInGroup = restOfGroup;
	}


	void SettingsMenu::SettingsMenu::DropDownSettingsComponent::setValue(int idx)
	{
		p->settingsStruct->set(descriptionText.upToLastOccurrenceOf(":", false, true), idx);
		for (auto c : othersInGroup)
		{
			for (int i = 0; i < c->cb->getListComponent()->List.size(); ++i)
			{
				c->cb->getListComponent()->List[i]->setEnabled(c->cb->getListComponent()->List[i]->getText().compare(cb->getListComponent()->List[idx]->getText()) != 0);
			}

			for (int i = 0; i < cb->getListComponent()->List.size(); ++i)
			{
				cb->getListComponent()->List[i]->setEnabled(cb->getListComponent()->List[i]->getText().compare(c->cb->getListComponent()->List[c->cb->getIndex()]->getText()) != 0);
			}
		}
		p->settingsStruct->saveSettings();
	}


	void SettingsMenu::SettingsMenu::DropDownSettingsComponent::resized()
	{
		auto w = 0.2 * menuWidth;
		auto x = 0.95 - w;
		cb->setBoundsRelative(x, 0, w, 1);
	}

	void SettingsMenu::SettingsMenu::DropDownSettingsComponent::paint(juce::Graphics& g)
	{
		g.setColour(getColor(BDSP_COLOR_WHITE));
		g.setFont(p->universals->Fonts[getFontIndex()].getFont().withHeight(getHeight() * SettingsFontH));

		drawText(g, g.getCurrentFont(), descriptionText, getLocalBounds().withRight(cb->getX()), false, juce::Justification::centredLeft);
	}



	ComboBox* SettingsMenu::DropDownSettingsComponent::getComboBox()
	{
		return cb.get();
	}


}// namnepace bdsp
