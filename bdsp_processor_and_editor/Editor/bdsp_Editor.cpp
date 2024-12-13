#include "bdsp_Editor.h"
namespace bdsp
{

	Editor::Editor(Processor& p)
		: juce::AudioProcessorEditor(&p),
		audioProcessor(p),
		corner(&GUIUniversals, this, &boundsConstrainer),
		contextHolder(*this)
	{

		//================================================================================================================================================================================================
		//setInterceptsMouseClicks(true, true);



		setConstrainer(&boundsConstrainer);

		p.settingsTree.addListener(this);
		p.editor = this;

		GUIListener = std::make_unique<GUI_Listener>(this, &GUIUniversals);

		corner.color = BDSP_COLOR_WHITE;

#if BDSP_DEMO_VERSION
		demoAlert = std::make_unique<bdsp::DemoAlert<float>>(this, p.Demo.get());
#endif

		prevUIScaleFactor = p.settings.get("UI Scale");

		//================================================================================================================================================================================================

		GUIUniversals.freqRange = audioProcessor.parameterAttributes.getFloatAttribute("Frequency").range;

		if (audioProcessor.specSet)
		{
			GUIUniversals.freqRange.end = audioProcessor.subBlockSpec->sampleRate / 2;
		}
	}



	Editor::~Editor()
	{
		contextHolder.detach();





#if BDSP_DEMO_VERSION

		//audioProcessor.Demo->setAlertWindow(nullptr);

#endif

		closeAllMenus();

		audioProcessor.settingsTree.removeListener(this);

		resized();

		//contextHolder.detach();


	}

	//==============================================================================



	void Editor::resized()
	{
		closeAllMenus();

		auto scale = (audioProcessor.settings.get("UI Scale") - 2) * 0.25 + 1;

		audioProcessor.settingsTree.setPropertyExcludingListener(this, "WindowWidth", topLevelComp->mainArea.getWidth() / scale, nullptr);

		GUIUniversals.visualizerLineThickness = getHeight() / 1500.0f;


		//topLevelComp->aspectRatio = (double)getWidth() / getHeight();
		topLevelComp->setBounds(getLocalBounds());
		if (topLevelComp->sidebarOpen)
		{
			topLevelComp->sideBarContainer->setBounds(topLevelComp->sideBarContainer->getBounds().reduced(0, GUIUniversals.rectThicc).withWidth(topLevelComp->sideBarContainer->getWidth() - GUIUniversals.rectThicc));
		}

		GUIUniversals.desktopManager.setBounds(getLocalBounds());
		GUIUniversals.desktopManager.mainArea = topLevelComp->mainArea.getBounds();
	}

	void Editor::paintOverChildren(juce::Graphics& g)
	{
		//jassert(getNumChildComponents() == 1); // there should be no direct children other than the desktop manager
#if BDSP_DEMO_VERSION
		if (demoIsSilencing)
		{
			paintDemoSilencingMessage(g);
		}
		else if (pluginIsDimmed)
		{
			paintDimmedPlugin(g);
		}
#endif

	}


	void Editor::closeAllMenus()
	{
		GUIUniversals.desktopManager.hideAllComponents(true);
	}







	void Editor::mouseDown(const juce::MouseEvent& e)
	{
		if (!e.mods.isPopupMenu())
		{
			closeAllMenus();

			for (int i = 0; i < BDSP_NUMBER_OF_MACROS; i++)
			{
				auto title = &dynamic_cast<MacroComponent*>(linkables->controls[i])->getSlider()->label.getTitle();
				if (!title->contains(e.getEventRelativeTo(title).position))
				{
					title->giveAwayKeyboardFocus();
				}
			}
		}
	}

	void Editor::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{
		auto cast = dynamic_cast<ComboBox*>(e.originalComponent);
		if (cast != nullptr)
		{
			GUIUniversals.desktopManager.hideAllComponentsExcept(juce::Array<DesktopComponent*>(cast->getListuniversals()));
		}
		else
		{
			closeAllMenus();
		}
	}



#if BDSP_DEMO_VERSION
	void Editor::paintDemoSilencingMessage(juce::Graphics& g)
	{
		g.setColour(juce::Colours::black.withAlpha(BDSP_OVERLAY_ALPHA));
		g.fillAll();
		auto alertBounds = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0.25, 0.25, 0.5, 0.5));

		g.setColour(GUIUniversals.colors.getColor(BDSP_COLOR_DARK));
		g.fillRoundedRectangle(alertBounds, GUIUniversals.roundedRectangleCurve);

		auto contentW = 0.95;

		auto headerRect = alertBounds.getProportion(juce::Rectangle<float>((1 - contentW) / 2.0, 0, contentW, 0.15));
		auto bodyRect = alertBounds.getProportion(juce::Rectangle<float>((1 - contentW) / 2.0, 0.2, contentW, 0.75));

		g.setColour(GUIUniversals.colors.getColor(BDSP_COLOR_WHITE));
		g.setFont(resizeFontToFit(GUIUniversals.Fonts[1].getFont(), headerRect.getWidth(), headerRect.getHeight() * 0.9, demoAlert->Header));
		drawText(g, g.getCurrentFont(), demoAlert->Header, headerRect);



		drawDivider(g, juce::Line<float>(headerRect.getX(), headerRect.getBottom(), headerRect.getRight(), headerRect.getBottom()), GUIUniversals.colors.getColor(BDSP_COLOR_WHITE).withAlpha(0.5f), GUIUniversals.dividerSize);

		g.setColour(GUIUniversals.colors.getColor(BDSP_COLOR_WHITE));
		g.setFont(GUIUniversals.Fonts[2].getFont().withHeight(g.getCurrentFont().getHeight() * 0.9));
		g.drawFittedText(demoAlert->Body, shrinkToInt(bodyRect), juce::Justification::centredTop, bodyRect.getHeight() / g.getCurrentFont().getHeight());


		g.setColour(GUIUniversals.colors.getColor(BDSP_COLOR_LIGHT));
		g.drawRoundedRectangle(alertBounds.toFloat(), GUIUniversals.roundedRectangleCurve, GUIUniversals.roundedRectangleCurve / 4);
	}



	void Editor::demoDisable(bool enable)
	{
		demoIsSilencing = !enable;
		setEnabled(enable);

		repaint();
	}
#endif




	void Editor::loadSettings()
	{
		topLevelComp->settings->load(audioProcessor.propertiesFolder.getFolder().getChildFile("Data").getChildFile(BDSP_APP_NAME + juce::String(" Settings.xml")), audioProcessor.propertiesFolder.getFolder().getParentDirectory().getChildFile("Global Settings.xml"));
	}





	void Editor::setNoBPM()
	{
		topLevelComp->BPM->setNoHost();
	}

	void Editor::resizeForSidebar(bool expand)
	{
		topLevelComp->sidebarOpen = expand;
		topLevelComp->sideBarContainer->setVisible(expand);
		boundsConstrainer.setFixedAspectRatio(expand ? topLevelComp->sidebarAspectRatio : topLevelComp->aspectRatio);

		setSize(getHeight() * boundsConstrainer.getFixedAspectRatio(), getHeight());


	}

	void Editor::setMainAreaSize(int width, int height)
	{
		if (topLevelComp->sidebarOpen)
		{
			setSize(width / topLevelComp->aspectRatio * topLevelComp->sidebarAspectRatio, height);
		}
		else
		{
			setSize(width, height);
		}
	}

	void Editor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
	{
		if (property == juce::Identifier("WindowWidth"))
		{
			float w = treeWhosePropertyHasChanged.getProperty("WindowWidth");
			w = w == 0 ? BDSP_DEFAULT_PLUGIN_WIDTH : w;
			w *= (audioProcessor.settings.get("UI Scale") - 2) * 0.25 + 1;

			setSize(w, w / (topLevelComp->sidebarOpen ? topLevelComp->sidebarAspectRatio : topLevelComp->aspectRatio));
		}
	}

	void Editor::setScaleFactor(float newScale)
	{
		juce::AudioProcessorEditor::setScaleFactor(newScale);
		GUIUniversals.systemScaleFactor = newScale;
		resized();
	}

	void Editor::dimPlugin(bool dim)
	{
		topLevelComp->pluginIsDimmed = dim;
		topLevelComp->setEnabled(!dim);
		topLevelComp->repaint();
	}

	void Editor::init()
	{
		setOpaque(true);
		setUniversals();
		GUIUniversals.undoManager = &audioProcessor.undoManager;
		GUIUniversals.repaintChecker = &audioProcessor.repaintChecker;
		GUIUniversals.contextHolder = &contextHolder;
		GUIUniversals.openGLContext = &contextHolder.context;




		//================================================================================================================================================================================================




		for (auto* c : audioProcessor.controlObjects)
		{
			GUIUniversals.controls.add(std::move(c));
		}

		GUIUniversals.hintBar = std::make_unique<HintBar>(&GUIUniversals);
		controlTransferMenu = std::make_unique<ControlTransferPopupMenu>(&GUIUniversals);
		GUIUniversals.controlPopup = controlTransferMenu.get();
		GUIUniversals.APVTS = audioProcessor.parameters.get();
		topLevelComp = std::make_unique<TopLevelGUIComponent>(this);

		//topLevelComp->setInterceptsMouseClicks(true, true);

		addAndMakeVisible(GUIUniversals.desktopManager);
		GUIUniversals.desktopManager.addAndMakeVisible(topLevelComp.get());
		//GUIUniversals.desktopManager.setInterceptsMouseClicks(true, true);

		//topLevelComp->mainArea.setInterceptsMouseClicks(true, true);
		//topLevelComp->sideBarContainer->setInterceptsMouseClicks(true, true);
		topLevelComp->setInterceptsMouseClicks(false, true);
		topLevelComp->setName("Top Level Comp");
		topLevelComp->mainArea.setName("Main Area");
		//================================================================================================================================================================================================



		addAndMakeVisible(corner);
		corner.setEnabled(true);
		corner.setAlwaysOnTop(true);


	}

	void Editor::startOpenGlAnimation()
	{
		contextHolder.context.setContinuousRepainting(true);
	}

	void Editor::shutdownOpenGlAnimation()
	{
		contextHolder.detach();
	}

	Editor::TopLevelGUIComponent* Editor::getTopLevelGUIComponent()
	{
		return topLevelComp.get();
	}

	bool Editor::isInterestedInFileDrag(const juce::StringArray& files)
	{
		for (auto f : files)
		{
			if (f.endsWith(".bdspreset") || !f.contains("."))
			{
				return true;
			}
		}
		return false;
	}

	void Editor::filesDropped(const juce::StringArray& files, int x, int y)
	{
		int count = 0;
		bool fileConflicts = false;
		for (auto f : files)
		{
			auto file = juce::File(f);
			if (file.isDirectory())
			{
				juce::StringArray subFilePaths;
				auto subFiles = file.findChildFiles(juce::File::findFiles, true, "*.bdspreset");
				for (auto sf : subFiles)
				{
					subFilePaths.add(sf.getFullPathName());
				}
				filesDropped(subFilePaths, x, y);
			}
			auto fileData = StateHandler::PresetData(file);
			if (fileData.isValid())
			{
				count++;
				auto newLoc = topLevelComp->preset->Handler->pluginFolder->getPresetFolder().getChildFile(file.getFileName());
				if (newLoc.existsAsFile())
				{
					fileConflicts = true;
				}
			}
		}

		auto writeFunc = [=](bool overrideExisiting)
		{
			for (auto f : files)
			{
				auto file = juce::File(f);
				auto fileData = StateHandler::PresetData(file);
				if (fileData.isValid())
				{
					auto newLoc = topLevelComp->preset->Handler->pluginFolder->getPresetFolder().getChildFile(file.getFileName());
					if (!newLoc.existsAsFile() || overrideExisiting)
					{
						file.copyFileTo(newLoc);
					}
				}
				topLevelComp->preset->menu->reBuildMenu();
			}
			topLevelComp->Alert->hide();

		};

		if (count > 0)
		{
			if (fileConflicts)
			{
				auto overrideFunc = [=](int idx)
				{
					if (idx == 0)
					{
						topLevelComp->Alert->hide();
					}
					else
					{
						writeFunc(idx == 2);
					}
				};
				auto Title = count == 1 ? "Importing Existing Preset" : "Importing Existing Presets";
				auto Body = count == 1 ? "The preset you are trying to import already exists, how would you like to handle this conflict?" : "At least one of the presets you are trying to import already exist, how would you like to handle these conflicts?";

				topLevelComp->Alert->addItemToQueue(AlertWindowItem("Cancel", "Don't Override", "Override", Title, Body, overrideFunc));
			}
			else
			{
				auto importFunc = [=](int idx)
				{
					if (idx == 0)
					{
						topLevelComp->Alert->hide();
					}
					else
					{
						writeFunc(false);
					}
				};
				auto Title = count == 1 ? "Importing Preset" : "Importing Presets";
				auto Body = count == 1 ? "Are you sure you wish to import this preset?" : "Are you sure you wish to import these presets?";

				topLevelComp->Alert->addItemToQueue(AlertWindowItem("Cancel", "", "Import", Title, Body, importFunc));

			}
		}
	}













	//================================================================================================================================================================================================


	Editor::TopLevelGUIComponent::TopLevelGUIComponent(Editor* e)
		:settingsButton(&e->GUIUniversals, true)
	{
		addAndMakeVisible(mainArea);


		//================================================================================================================================================================================================
		auto& audioProcessor = e->audioProcessor;
		auto* GUIUniversals = &e->GUIUniversals;

		//================================================================================================================================================================================================

		std::function<void(bool)> AlertShowHideFunc = [=](bool state)
		{
			setInterceptsMouseClicks(!state, !state);
			if (e != nullptr)
			{
				e->dimPlugin(state);
			}
		};

		Alert = std::make_unique<AlertWindow>(AlertShowHideFunc, GUIUniversals);


		//================================================================================================================================================================================================


		BPM = std::make_unique<BPMComponent>(audioProcessor.BPMFollow.get(), GUIUniversals, Alert.get(), BDSP_COLOR_WHITE, BDSP_COLOR_WHITE, BDSP_COLOR_LIGHT);
		BPM->attach(*audioProcessor.parameters.get());

		BPM->getHostSlider()->setHintBarText(juce::String("BPM set by the host"));
		BPM->getUserSlider()->setHintBarText(juce::String("BPM set by the user"));

		auto& prevBPMFunc = BPM->button.onClick;
		BPM->button.onClick = [=]()
		{
			prevBPMFunc();
			BPM->button.setHintBarText((BPM->button.getToggleState() ? juce::String("Unlinks") : juce::String("Links")) + " BPM to the host's BPM");

			const auto& suffix = BPM->button.getToggleState() ? juce::String(" (when unlinked)") : juce::String();

			BPM->inc.setHintBarText("Increases BPM by 1" + suffix);
			BPM->fineInc.setHintBarText("Increases BPM by 0.1" + suffix);
			BPM->dec.setHintBarText("Decreases BPM by 1" + suffix);
			BPM->fineDec.setHintBarText("Decreases BPM by 0.1" + suffix);



		};
		BPM->button.onClick(); // sets all hint texts automatically


		//================================================================================================================================================================================================
		e->linkables = std::make_unique<LinkableControlComponents>(&audioProcessor.envelopeSourceList, GUIUniversals, &audioProcessor.settingsTree, BPM.get());

		e->Macros = e->linkables->getMacros();
		e->LFOs = e->linkables->getLFOs();
		e->EnvelopeFollowers = e->linkables->getEnvelopeFolowers();

		GUIUniversals->controlComps = e->linkables.get();

		for (int i = 0; i < audioProcessor.linkableControlIDs.size(); i++)
		{
			auto newID = juce::Identifier(audioProcessor.linkableControlIDs[i]);
			e->linkables->controls[i]->setID(newID);
			//GUIUniversals.e->linkables->controls[i]->name = audioProcessor.linkableControlNames[i];
			e->linkables->controls[i]->setControlObject(audioProcessor.controlObjects[i]);

			e->linkables->controls[i]->setColor(BDSP_COLOR_WHITE, BDSP_COLOR_WHITE);

			auto controlComp = e->linkables->controls[i];
			auto compCast = dynamic_cast<GeneralControlComponent*>(controlComp);
			auto macroCast = dynamic_cast<MacroComponent*>(controlComp);


			GUIUniversals->hintBar->addTextToRecolor({ controlComp->defaultName, BDSP_COLOR_WHITE });
			GUIUniversals->hintBar->addTextToRecolor({ controlComp->defaultName + "'s", BDSP_COLOR_WHITE });



			juce::Array<bdsp::ComponentCore*> compsThatCopyText;
			compsThatCopyText.add(&controlComp->dragComp);
			juce::String text;
			if (macroCast != nullptr)
			{
				macroCast->getSlider()->attach(*audioProcessor.parameters.get(), audioProcessor.linkableControlIDs[i]);

				text = controlComp->defaultName + " - Modify multiple parameters simultaneously";
				macroCast->getSlider()->setHintBarText(text);
				compsThatCopyText.add(&macroCast->getSlider()->label.getTitle());
			}
			else if (i < BDSP_NUMBER_OF_MACROS + BDSP_NUMBER_OF_LFOS) // LFO
			{
				text = controlComp->defaultName + " - Modifiy multiple parameters over time";
				compCast->getHintBackground()->setHintBarText(text);
				compsThatCopyText.add(compCast->getVisualizer());
				compsThatCopyText.add(compCast->getTitleComponent());

			}
			else
			{
				text = controlComp->defaultName + " - Modify multiple parameters in response to an audio signal";
				compCast->getHintBackground()->setHintBarText(text);
				compsThatCopyText.add(compCast->getVisualizer());
				compsThatCopyText.add(compCast->getTitleComponent());
			}

			for (auto c : compsThatCopyText)
			{
				c->setHintBarText(text);
			}

		}
		//for (auto c : GUIUniversals.universals->e->linkables->controls)
		//{
		//	LFOComponent* lfo = dynamic_cast<LFOComponent*>(c);
		//	if (lfo)
		//	{
		//		lfo->attach();
		//	}
		//}

		GUIUniversals->propertiesFolder = &audioProcessor.propertiesFolder;

		mainArea.addAndMakeVisible(GUIUniversals->hintBar.get());
//		//juce::Desktop::getInstance().setDefaultLookAndFeel(&GUIUniversals.MasterBaseLNF);


		sideBarContainer = std::make_unique<TexturedContainerComponent>(GUIUniversals, BDSP_COLOR_DARK);
		//sideBarContainer->addMouseListener(this, true);
		addAndMakeVisible(sideBarContainer.get());


		sideBarContainerCloser = std::make_unique<MultiShapeButton>(GUIUniversals);


		sideBarContainerCloser->addShapes(Shape(GUIUniversals->commonPaths.closeButton, BDSP_COLOR_WHITE, juce::Rectangle<float>(0, 0, 1, 1)));

		sideBarContainerCloser->onClick = [=]()
		{
			settings->isVisible() ? settingsButton.triggerClick() : preset->closeBrowser();
		};



		//================================================================================================================================================================================================

		undoRedo = std::make_unique<bdsp::UndoRedoButtons>(GUIUniversals, BDSP_COLOR_WHITE);
		mainArea.addAndMakeVisible(undoRedo.get());

		//================================================================================================================================================================================================




		logo = std::make_unique<Logo>(GUIUniversals);


		mainArea.addAndMakeVisible(logo.get());
		logo->setColors({ BDSP_COLOR_COLOR });



		settings = std::make_unique<SettingsMenu>(GUIUniversals, &audioProcessor.settings, audioProcessor.propertiesFolder.getFolder().findChildFiles(juce::File::findFiles, false, "*.pdf").getFirst());

		for (auto i : audioProcessor.settings.exclusionGroups)
		{
			settings->setExclusiveSelectionGroup(i);
		}

		settingsButton.setHintBarText(juce::String("Opens the settings menu"));

		settingsButton.onClick = [=]()
		{
			if (settingsButton.getToggleState())
			{
				preset->closeBrowser();
			}

			settings->setVisible(settingsButton.getToggleState());
			e->resizeForSidebar(settingsButton.getToggleState());


			settingsButton.setHintBarText((settings->isVisible() ? juce::String("Closes ") : juce::String("Opens ")) + "the settings menu");
		};


		settings->linkButton(&settingsButton);

		sideBarContainer->addChildComponent(settings.get());




		//================================================================================================================================================================================================



		auto gear = createGear(juce::Point<float>(), 1, 0.5, 8, 0.25, PI / 9);
		Shape s(gear, BDSP_COLOR_WHITE, juce::Rectangle<float>(1, 1).reduced(2 * SettingsButtonOutlineRatio), BDSP_COLOR_WHITE, SettingsButtonOutlineRatio);
		s.fillOff = NamedColorsIdentifier();

		settingsButton.addShapes(s);

		settingsButton.setClickingTogglesState(true);

		settingsButton.setInterceptsMouseClicks(true, false);

		mainArea.addAndMakeVisible(settingsButton);












		//================================================================================================================================================================================================




		preset = std::make_unique<PresetManager>(GUIUniversals, audioProcessor.Handler.get(), Alert.get());
		preset->browserOpenClose = [=](bool open)
		{


			if (open)
			{
				settingsButton.setToggleState(false, juce::dontSendNotification);
				settings->setVisible(false);
			}

			preset->browser.setVisible(open);
			preset->tagMenu->setVisible(open);
			e->resizeForSidebar(open);

		};
		mainArea.addAndMakeVisible(preset->titleBar);
		sideBarContainer->addChildComponent(preset->browser, 0);


		//preset->Load.setHintBarText( "Opens the preset browser (" + juce::String(BDSP_POPUP_ACTION_TEXT) + " to initialize the plugin)");
		//preset->Save.setHintBarText( "Saves the current state as a preset");
		//preset->close.setHintBarText( "Closes the preset browser");



		//preset->fav.onClick();

		preset->left.setHintBarText(juce::String("Previous preset"));
		preset->right.setHintBarText(juce::String("Next preset"));




		//================================================================================================================================================================================================



		sideBarContainer->addAndMakeVisible(sideBarContainerCloser.get());

	}

	inline void Editor::TopLevelGUIComponent::resized()
	{
		mainArea.setSize(getHeight() * aspectRatio, getHeight());
		if (sidebarOpen)
		{
			auto sideRect = juce::Rectangle<int>(mainArea.getRight(), 0, getWidth() - mainArea.getWidth(), getHeight());
			sideBarContainer->setBounds(sideRect);
			sideBarContainerCloser->setBoundsRelative(0.95, 0.01, 0.025, 0.025);
			settings->setBoundsRelative(0.025, 0.01, 0.95, 0.98);
			preset->browser.setBoundsRelative(0.025, 0.01, 0.95, 0.98);
		}
	}

	void Editor::TopLevelGUIComponent::addComponent(juce::Component* c, bool shouldBeVisible, bool inMainArea)
	{
		juce::Component* p = inMainArea ? &mainArea : sideBarContainer.get();
		if (shouldBeVisible)
		{
			c->setVisible(true);
		}
		p->addChildComponent(c);

	}

	void Editor::TopLevelGUIComponent::paintDimmedPlugin(juce::Graphics& g)
	{
		g.setColour(juce::Colours::black.withAlpha(BDSP_OVERLAY_ALPHA));
		g.fillAll();
	}

	inline void Editor::TopLevelGUIComponent::paintOverChildren(juce::Graphics& g)
	{
		if (paintOverChildrenFunction.operator bool())
		{
			paintOverChildrenFunction(g);
		}
		if (pluginIsDimmed)
		{
			paintDimmedPlugin(g);
		}
	}


}// namnepace bdsp
