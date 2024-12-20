#include "bdsp_StateHandler.h"

namespace bdsp
{
	StateHandler::StateHandler(juce::AudioProcessor* processorToUse, juce::AudioProcessorValueTreeState* APVTS, juce::ValueTree* settings, PropertiesFolder* folder, juce::Array<ControlParameter*>& controlParams, juce::Array<FactoryPreset>& presets)
		:controlParameters(controlParams)
	{
		pluginFolder = folder;
		processor = processorToUse;
		parameters = APVTS;
		settingsTree = settings;


		presetName.referTo(settingsTree->getPropertyAsValue("PresetName", nullptr));
		alteredState.referTo(settingsTree->getPropertyAsValue("PresetAltered", nullptr));

		factoryPresets = presets;



		installFactoryPresetsAndData();

	}

	StateHandler::~StateHandler()
	{
		parameters->state.removeListener(this);
		settingsTree->removeListener(this);

	}


	bool StateHandler::renamePreset(const juce::File& oldLocation, const juce::File& newLocation)
	{
		auto xmlState = juce::parseXML(oldLocation);
		xmlState->getChildByName(InfoXMLTag)->setAttribute("PresetName", stringToXMLData(newLocation.getFileNameWithoutExtension()));

		oldLocation.setReadOnly(false);
		if (oldLocation.deleteFile())
		{
			xmlState->writeTo(newLocation);
			return true;
		}
		else
		{
			return false;
		}


	}

	juce::File StateHandler::savePreset(const juce::File& preset, bool favorite, const juce::StringArray& tags, const juce::StringArray& superTags)
	{
		alteredState.setValue(false);
		presetName.setValue(preset.getFileNameWithoutExtension());
		auto xml = createSaveXML(false, preset.getFileNameWithoutExtension(), favorite, tags, superTags);

		auto infoNode = xml->getChildByName(InfoXMLTag);

		infoNode->setAttribute("PresetAltered", false);

		xml->writeTo(preset);

		preset.setReadOnly(false);

		return preset;
	}


	std::unique_ptr<juce::XmlElement> StateHandler::createSaveXML(bool setAlteredState, const juce::String& name, bool favorite, const juce::StringArray& tags, const juce::StringArray& superTags)
	{
		bool tagsUpdated = false;// flag to rebuild master file and tag menu

		std::unique_ptr<juce::XmlElement> xml(std::make_unique<juce::XmlElement>("Main"));



		auto infoNode = xml->createNewChildElement(InfoXMLTag);


		infoNode->setAttribute("Name", BDSP_APP_NAME);
		infoNode->setAttribute("Version", JucePlugin_VersionString);
		infoNode->setAttribute("PresetName", stringToXMLData(name));
		if (setAlteredState)
		{
			infoNode->setAttribute("PresetAltered", alteredState.getValue().operator bool());
		}


		auto favNode = xml->createNewChildElement(FavoriteXMLTag);
		favNode->setAttribute(FavoriteAttributeName, favorite);

		auto tagsNode = xml->createNewChildElement(TagsXMLTag);
		for (juce::String s : tags)
		{
			tagsUpdated = tagNames.addIfNotAlreadyThere(s) || tagsUpdated; // flips true if true doesn't filp back to false

			auto total = stringToXMLData(s);
			tagsNode->setAttribute(total, (int)true);
		}

		auto superTagsNode = xml->createNewChildElement(SuperTagsXMLTag);

		for (int i = 0; i < superTags.size(); ++i)
		{
			tagsUpdated = superTagNames[i].addIfNotAlreadyThere(superTags[i]) || tagsUpdated; // flips true if true, doesn't filp back to false
			superTagsNode->setAttribute(stringToXMLData(superTagTitles[i]), stringToXMLData(superTags[i]));
		}


		auto macroNamesNode = xml->createNewChildElement(MacroNamesXMLTag);

		for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
		{
			macroNamesNode->setAttribute("Macro" + juce::String(i + 1) + "NameID", settingsTree->getProperty("Macro" + juce::String(i + 1) + "NameID", "Macro " + juce::String(i + 1)).toString());
		}
		for (int i = 0; i < BDSP_NUMBER_OF_LFOS; ++i)
		{
			macroNamesNode->setAttribute("LFO" + juce::String(i + 1) + "NameID", settingsTree->getProperty("LFO" + juce::String(i + 1) + "NameID", "LFO " + juce::String(i + 1)).toString());
		}
		for (int i = 0; i < BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS; ++i)
		{
			macroNamesNode->setAttribute("Env" + juce::String(i + 1) + "NameID", settingsTree->getProperty("Env" + juce::String(i + 1) + "NameID", "Env " + juce::String(i + 1)).toString());
		}


		juce::ValueTree state = parameters->state;
		state.removeAllProperties(nullptr);

		xml->addChildElement(state.createXml().release());

		if (tagsUpdated)
		{
			updateMasterFile();
		}




		return xml;

	}


	void StateHandler::loadPreset(const juce::File& preset, bool isUndoable)
	{

		auto xmlState = juce::parseXML(preset);

		loadPreset(xmlState, isUndoable);
		currentPresetFile = preset;
		//alteredState.setValue(false);

	}

	void StateHandler::loadPreset(const juce::ValueTree& state, bool isUndoable)
	{
		{
			juce::ScopedLock lock(valueTreeChanging);

			if (isUndoable)
			{
				parameters->undoManager->beginNewTransaction("Change Preset");
			}

			auto data = PresetData(state);
			data.mergePresets(*initData.get());
			parameters->state.copyPropertiesAndChildrenFrom(data.getTree(true), isUndoable ? parameters->undoManager : nullptr);
		}


		settingsTree->addListener(this);

		for (auto c : controlParameters)
		{
			c->valueChanged();
		}
	}


	bool StateHandler::loadPreset(std::unique_ptr<juce::XmlElement>& xmlState, bool isUndoable)
	{
		bool out = false;
		if (xmlState.get() != nullptr && xmlState->getChildByName(InfoXMLTag)->getAttributeValue(0).compare(BDSP_APP_NAME) == 0) // if file is valid xml and for the same plug-in
		{
			out = true;
			//	auto newState = xmlState->getChildByName(parameters->state.getType());







			const juce::ValueTree tree = juce::ValueTree::fromXml(*xmlState);
			loadPreset(tree, isUndoable);

			parameters->state.addListener(this);


			settingsTree->setProperty("PresetName", XMLDataToString(xmlState->getChildByName(InfoXMLTag)->getStringAttribute("PresetName")), parameters->undoManager);
			//alteredState.setValue(xmlState->getChildByName(InfoXMLTag)->getBoolAttribute("Altered"));

			settingsTree->setProperty("PresetAltered", false, parameters->undoManager);

			auto macroNamesNode = xmlState->getChildByName(MacroNamesXMLTag);

			for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
			{
				auto name = macroNamesNode->getAttributeValue(i);
				if (name.isEmpty())
				{
					name = "Macro " + juce::String(i + 1);
				}
				settingsTree->setProperty("Macro" + juce::String(i + 1) + "NameID", name, parameters->undoManager);
			}
			for (int i = 0; i < BDSP_NUMBER_OF_LFOS; ++i)
			{
				auto name = macroNamesNode->getAttributeValue(BDSP_NUMBER_OF_MACROS + i);
				if (name.isEmpty())
				{
					name = "LFO " + juce::String(i + 1);
				}
				settingsTree->setProperty("LFO" + juce::String(i + 1) + "NameID", name, parameters->undoManager);
			}
			for (int i = 0; i < BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS; ++i)
			{
				auto name = macroNamesNode->getAttributeValue(BDSP_NUMBER_OF_MACROS + BDSP_NUMBER_OF_LFOS + i);
				if (name.isEmpty())
				{
					name = "Env " + juce::String(i + 1);
				}
				settingsTree->setProperty("Env" + juce::String(i + 1) + "NameID", name, parameters->undoManager);
			}

		}
		return out;
	}




	void StateHandler::updatePreset(const juce::File& preset, bool favorite)
	{
		auto xmlState = juce::parseXML(preset);


		xmlState->getChildByName(FavoriteXMLTag)->setAttribute(FavoriteAttributeName, favorite);

		xmlState->getChildByName(InfoXMLTag)->setAttribute("Name", BDSP_APP_NAME);

		xmlState->getChildByName(InfoXMLTag)->setAttribute("Version", JucePlugin_VersionString);

		xmlState->getChildByName(InfoXMLTag)->setAttribute("PresetName", stringToXMLData(preset.getFileNameWithoutExtension()));


		xmlState->writeTo(preset, juce::XmlElement::TextFormat());


	}

	void StateHandler::updatePreset(const juce::File& preset, const juce::StringArray& newTags, const juce::StringArray& newSuperTags)
	{
		auto xmlState = juce::parseXML(preset);

		xmlState->getChildByName(InfoXMLTag)->setAttribute("Name", BDSP_APP_NAME);

		xmlState->getChildByName(InfoXMLTag)->setAttribute("Version", JucePlugin_VersionString);

		xmlState->getChildByName(InfoXMLTag)->setAttribute("PresetName", stringToXMLData(preset.getFileNameWithoutExtension()));



		bool tagsUpdated = false;// flag to rebuild master file and tag menu


		auto tagsNode = xmlState->getChildByName(TagsXMLTag);

		tagsNode->removeAllAttributes();

		for (auto s : newTags)
		{
			tagsUpdated = tagNames.addIfNotAlreadyThere(s) || tagsUpdated; // flips true if true doesn't filp back to false

			auto total = stringToXMLData(s);
			tagsNode->setAttribute(total, int(true));
		}

		auto superTagsNode = xmlState->getChildByName(SuperTagsXMLTag);


		for (int i = 0; i < newSuperTags.size(); ++i)
		{
			tagsUpdated = superTagNames[i].addIfNotAlreadyThere(newSuperTags[i]) || tagsUpdated; // flips true if true doesn't filp back to false

			superTagsNode->setAttribute(stringToXMLData(superTagTitles[i]), stringToXMLData(newSuperTags[i]));
		}


		xmlState->writeTo(preset, juce::XmlElement::TextFormat());

		if (tagsUpdated)
		{
			updateMasterFile();
		}

	}





	void StateHandler::initializePlugin(bool isUndoable)
	{
		loadPreset(initFile, isUndoable);
	}

	void StateHandler::setStartupPreset(const juce::File& presetFile)
	{
		auto file = pluginFolder->getFolder().getChildFile("Data").getChildFile(BDSP_APP_NAME + juce::String(" Default Preset.xml"));
		startupFile = presetFile;

		std::unique_ptr<juce::XmlElement> xml(std::make_unique<juce::XmlElement>("Main"));
		xml->setAttribute("Default", stringToXMLData(presetFile.getFileNameWithoutExtension()));

		file.deleteFile();
		xml->writeTo(file);
		file.setReadOnly(false);
	}

	void StateHandler::loadStartupPreset()
	{
		if (startupFile.exists())
		{
			loadPreset(startupFile, false);
		}
		else
		{
			initializePlugin();
		}

		std::function<void()> Func = [=]()
		{
			getUndoManager()->clearUndoHistory();
		};

		juce::Timer::callAfterDelay(100, Func);
	}


	void StateHandler::rebuildMasterFile()
	{
		tagNames.clear();
		superTagTitles.clear();


		for (auto p : pluginFolder->getFolder().getChildFile("Presets").findChildFiles(juce::File::findFiles, false, "*.bdspreset"))
		{
			auto xmlState = juce::parseXML(p);
			if (xmlState.get() != nullptr && xmlState->getChildByName(InfoXMLTag)->getAttributeValue(0).compare(BDSP_APP_NAME) == 0) // if file is valid xml and for the same plug-in
			{
				auto tagsNode = xmlState->getChildByName(TagsXMLTag);
				if (tagsNode != nullptr)
				{
					for (int i = 0; i < tagsNode->getNumAttributes(); ++i)
					{
						tagNames.addIfNotAlreadyThere(XMLDataToString(tagsNode->getAttributeName(i)));
					}
				}

				auto superTagsNode = xmlState->getChildByName(SuperTagsXMLTag);
				if (superTagsNode != nullptr)
				{
					for (int i = 0; i < superTagsNode->getNumAttributes(); ++i)
					{
						auto title = superTagsNode->getAttributeName(i);
						superTagTitles.addIfNotAlreadyThere(XMLDataToString(title));
						superTagNames[superTagTitles.indexOf(title)].addIfNotAlreadyThere(XMLDataToString(superTagsNode->getStringAttribute(title)));
					}
				}
			}
		}

		updateMasterFile();

	}

	void StateHandler::updateMasterFile()
	{
		std::unique_ptr<juce::XmlElement> xml(std::make_unique<juce::XmlElement>("MasterTagList"));
		auto tagsNode = xml->createNewChildElement(TagsXMLTag);
		for (auto t : tagNames)
		{
			auto total = stringToXMLData(t);
			tagsNode->setAttribute(total, (int)true);
		}

		auto superTagsNode = xml->createNewChildElement(SuperTagsXMLTag);

		for (int i = 0; i < superTagTitles.size(); ++i)
		{
			auto node = superTagsNode->createNewChildElement(stringToXMLData(superTagTitles[i]));
			for (int j = 0; j < superTagNames[i].size(); ++j)
			{
				auto total = stringToXMLData(superTagNames[i][j]);
				node->setAttribute(total, (int)true);
			}
		}

		auto masterTagFile = pluginFolder->getFolder().getChildFile("Data").getChildFile("MasterTagList.xml");
		xml->writeTo(masterTagFile);

		tagNames.sortNatural();
		for (auto& arr : superTagNames)
		{
			arr.sortNatural();
		}

		if (manager != nullptr)
		{
			manager->menu->tags->rebuildMenu();
			//manager->tagMenu->rebuildMenu();
			manager->saveMenu->rebuildTags();
		}
	}

	void StateHandler::readMasterFile()
	{
		tagNames.clear();

		superTagTitles.clear();
		superTagNames.clear();

		auto masterTagFile = pluginFolder->getFolder().getChildFile("Data").getChildFile("MasterTagList.xml");
		auto xml = juce::parseXML(masterTagFile);

		auto tagsNode = xml->getChildByName(TagsXMLTag);

		for (int i = 0; i < tagsNode->getNumAttributes(); ++i)
		{
			tagNames.add(XMLDataToString(tagsNode->getAttributeName(i)));
		}

		auto superTagsNode = xml->getChildByName(SuperTagsXMLTag);
		int num = superTagsNode->getNumChildElements();
		superTagNames.resize(num);
		for (int i = 0; i < num; ++i)
		{
			superTagTitles.add(XMLDataToString(tagsNode->getChildElement(i)->getTagName()));
			for (int j = 0; j < superTagsNode->getChildElement(i)->getNumAttributes(); ++j)
			{
				superTagNames[i].add(XMLDataToString(tagsNode->getAttributeValue(i)));
			}
		}

		tagNames.sortNatural();
		for (auto& arr : superTagNames)
		{
			arr.sortNatural();
		}
	}


	void StateHandler::installFactoryPresetsAndData()
	{

		if (!pluginFolder->getFolder().getChildFile("Data").exists())
		{
			pluginFolder->getFolder().getChildFile("Data").createDirectory();
		}

		auto masterTagFile = pluginFolder->getFolder().getChildFile("Data").getChildFile("MasterTagList.xml");
		if (masterTagFile.exists())
		{
			if (masterTagFile.getLastModificationTime() < pluginFolder->getFolder().getChildFile("Presets").getLastModificationTime())
			{
				rebuildMasterFile();
			}
			else
			{
				readMasterFile();
			}
		}
		else
		{
			rebuildMasterFile();
		}


		//================================================================================================================================================================================================

		initFile = pluginFolder->getFolder().getChildFile("Data").getChildFile("init.xml");
		bool needsNewInitFile = false;
		if (initFile.exists())
		{
			auto initVersion = juce::parseXML(initFile)->getChildByName(InfoXMLTag)->getStringAttribute("Version");

			needsNewInitFile = versionStringCompare(initVersion, JucePlugin_VersionString) == -1;
		}
#if JUCE_DEBUG
		needsNewInitFile = true; // init file will always be up to date with parameter changes while developing
#endif

		if (!initFile.exists() || needsNewInitFile)
		{
			for (auto p : processor->getParameters())
			{
				p->setValue(p->getDefaultValue());
			}

			for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
			{
				settingsTree->setProperty("Macro" + juce::String(i + 1) + "NameID", "Macro " + juce::String(i + 1), nullptr);
			}
			for (int i = 0; i < BDSP_NUMBER_OF_LFOS; ++i)
			{
				settingsTree->setProperty("LFO" + juce::String(i + 1) + "NameID", "LFO " + juce::String(i + 1), nullptr);
			}
			for (int i = 0; i < BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS; ++i)
			{
				settingsTree->setProperty("Env" + juce::String(i + 1) + "NameID", "Env " + juce::String(i + 1), nullptr);
			}

			savePreset(initFile);

		}
		initData = std::make_unique<PresetData>(initFile);

		//================================================================================================================================================================================================


		auto presetInstallHistory = pluginFolder->getFolder().getChildFile("Data").getChildFile("FactoryPresetInstallHistory.xml");
		juce::String latestInstall;
		if (!presetInstallHistory.exists())
		{
			latestInstall = "0.0.0";
		}
		else
		{
			latestInstall = juce::parseXML(presetInstallHistory)->getAttributeValue(0);
		}

		if (!pluginFolder->getPresetFolder().exists())
		{
			pluginFolder->getPresetFolder().createDirectory();
			pluginFolder->getPresetFolder().setReadOnly(false);
		}

		for (auto p : factoryPresets)
		{

			if (p.fileName.endsWith(".pdf"))
			{
				juce::File f(pluginFolder->getFolder().getChildFile(p.fileName));
				f.replaceWithData(p.data, p.dataSize);
			}
			else
			{

				juce::File f(pluginFolder->getPresetFolder().getChildFile(p.fileName));

				if (!f.exists())
				{
					f.replaceWithData(p.data, p.dataSize);
					auto xmlState = juce::parseXML(f);
					bool writePreset = versionStringCompare(latestInstall, xmlState->getChildByName("Info")->getStringAttribute("Version")) == -1; // true if latest install was before preset was introduced (aka preset needs to be installed)

					if (writePreset)
					{
						xmlState->getChildByName("Favorite")->setAttribute("Fav", false); // set all new presets to unfavorited
						xmlState->writeTo(f, juce::XmlElement::TextFormat());
					}
					else
					{
						f.deleteFile();
					}
				}

			}

		}

		// update data file with correct version number
		std::unique_ptr<juce::XmlElement> xml(std::make_unique<juce::XmlElement>("Main"));
		xml->setAttribute("LatestInstall", JucePlugin_VersionString);

		presetInstallHistory.deleteFile();
		xml->writeTo(presetInstallHistory);
		presetInstallHistory.setReadOnly(false);
		//================================================================================================================================================================================================

		auto deafultIDFile = pluginFolder->getFolder().getChildFile("Data").getChildFile(BDSP_APP_NAME + juce::String(" Default Preset.xml"));
		if (deafultIDFile.exists())
		{
			startupFile = pluginFolder->getFolder().getChildFile("Presets").getChildFile(XMLDataToString(juce::parseXML(deafultIDFile)->getAttributeValue(0)) + ".bdspreset");
		}

		if (!startupFile.exists())
		{
			auto array = pluginFolder->getFolder().getChildFile("Presets").findChildFiles(juce::File::findFiles, false, "*.bdspreset");
			array.sort();
			setStartupPreset(array.getFirst());

		}
	}

	void StateHandler::reInstallFactoryPresets(bool overwrite)
	{
		for (auto p : factoryPresets)
		{

			if (!p.fileName.endsWith(".pdf"))
			{



				juce::File f(pluginFolder->getPresetFolder().getChildFile(p.fileName));

				if (!f.exists() || overwrite)
				{
					f.replaceWithData(p.data, p.dataSize);
					auto xmlState = juce::parseXML(f);

					xmlState->getChildByName("Favorite")->setAttribute("Fav", false); // set all new presets to unfavorited
					xmlState->writeTo(f, juce::XmlElement::TextFormat());

				}

			}
		}
	}

	void StateHandler::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
	{
		if (treeWhosePropertyHasChanged.getType().toString() != "UISettings" || (property.toString().startsWith("Macro") && property.toString().endsWith("NameID")) || (property.toString().startsWith("LFO") && property.toString().endsWith("NameID")) || (property.toString().startsWith("Env") && property.toString().endsWith("NameID"))) // either needs to be parameter or macro name
		{
			alteredState.setValue(true);
			parameters->state.removeListener(this);
			settingsTree->removeListener(this);

		}


	}




	juce::ValueTree* StateHandler::getPluginState()
	{
		return &parameters->state;
	}

	void StateHandler::setManager(PresetManager* m)
	{
		manager = m;
	}

	juce::UndoManager* StateHandler::getUndoManager()
	{
		return parameters->undoManager;
	}

	void StateHandler::replaceSaveMenuState(SaveMenuState newState)
	{
		getUndoManager()->perform(new UndoableSaveMenuStateSet(this, newState));
	}







	StateHandler::UndoableSaveMenuStateSet::UndoableSaveMenuStateSet(StateHandler* handlerToUse, SaveMenuState state)
	{
		handler = handlerToUse;
		newState = state;
	}

	bool StateHandler::UndoableSaveMenuStateSet::perform()
	{
		handler->currentSaveMenuState = newState;
		return true;
	}

	bool StateHandler::UndoableSaveMenuStateSet::undo()
	{
		handler->currentSaveMenuState = oldState;
		return true;
	}

}// namnepace bdsp


