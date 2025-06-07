#pragma once

namespace bdsp
{
	class PresetManager;
	class StateHandler;
	struct SaveMenuState
	{
		juce::String presetName;
		juce::StringArray activeTags;
		juce::StringArray activePendingTags;
		juce::StringArray inactivePendingTags;

		bool favortie = false;

		bool isEmpty()
		{
			return presetName.isEmpty() && activeTags.isEmpty() && activePendingTags.isEmpty() && inactivePendingTags.isEmpty();
		}


	};

	class StateHandler : public juce::ValueTree::Listener
	{
	public:
		StateHandler(juce::AudioProcessor* processorToUse, juce::AudioProcessorValueTreeState* APVTS, juce::AudioProcessorValueTreeState* hiddenAPVTS, juce::ValueTree* settings, PropertiesFolder* folder, juce::Array<ControlParameter*>& controlParams, juce::Array<OrderedListParameter*>& orderParams, juce::Array<FactoryPreset>& presets);
		~StateHandler();

		bool renamePreset(const juce::File& oldLocation, const juce::File& newLocation);
		juce::File savePreset(const juce::File& preset, bool favorite = false, const juce::StringArray& tags = juce::StringArray(), const juce::StringArray& superTags = juce::StringArray());
		std::unique_ptr<juce::XmlElement> createSaveXML(bool setAlteredState = false, const juce::String& name = juce::String(), bool favorite = false, const juce::StringArray& tags = juce::StringArray(), const juce::StringArray& superTags = juce::StringArray());
		void updatePreset(const juce::File& preset, bool favorite);
		void updatePreset(const juce::File& preset, const juce::StringArray& newTags, const juce::StringArray& newSuperTags);
		void loadPreset(const juce::File& preset, bool isUndoable = true);
		void loadPreset(const juce::ValueTree& state, bool isUndoable = true);
		bool loadPreset(std::unique_ptr<juce::XmlElement>& xmlState, bool isUndoable = true);




		void initializePlugin(bool isUndoable = true);
		void setStartupPreset(const juce::File& presetFile);
		void loadStartupPreset();


		void rebuildMasterFile();
		void updateMasterFile();
		void readMasterFile();

		void installFactoryPresetsAndData();
		void reInstallFactoryPresets(bool overwrite);
		void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;


		void setManager(PresetManager* m);

		juce::UndoManager* getUndoManager();


		//================================================================================================================================================================================================


		struct PresetData
		{
		public:
			PresetData(juce::XmlElement* xmlData)
				:xmlState("Main")
			{
				if (xmlData == nullptr || !(xmlData->getTagName() == "Main" || xmlData->getTagName() == "APVTS"))
				{
					valid = false;
				}
				else if (xmlData->getChildByName(InfoXMLTag)->getStringAttribute("Name").compare(BDSP_APP_NAME) != 0)
				{
					valid = false;
				}
				else
				{
					valid = true;
					isAPVTSOnly = xmlData->getTagName() == "APVTS";
					if (!isAPVTSOnly)
					{

						auto fillArray = [=](juce::XmlElement* e, juce::StringPairArray& arr)
						{
							if (e != nullptr)
							{
								for (int i = 0; i < e->getNumAttributes(); ++i)
								{
									arr.set(e->getAttributeName(i), e->getAttributeValue(i));
								}
							}
						};

						fillArray(xmlData->getChildByName(InfoXMLTag), info);
						fillArray(xmlData->getChildByName(TagsXMLTag), tags);
						fillArray(xmlData->getChildByName(SuperTagsXMLTag), superTags);
						fillArray(xmlData->getChildByName(MacroNamesXMLTag), macroNames);

						auto favNode = xmlData->getChildByName(FavoriteXMLTag);
						if (favNode != nullptr)
						{
							favorite = favNode->getBoolAttribute(FavoriteAttributeName);
						}

						auto APVTS = xmlData->getChildByName("APVTS");
						for (int i = 0; i < APVTS->getNumChildElements(); ++i)
						{
							parameters.set(APVTS->getChildElement(i)->getAttributeValue(0), APVTS->getChildElement(i)->getAttributeValue(1));
						}
						auto hiddenAPVTS = xmlData->getChildByName("APVTSHidden");
						if (hiddenAPVTS != nullptr)
						{
							for (int i = 0; i < hiddenAPVTS->getNumChildElements(); ++i)
							{
								hiddenParameters.set(hiddenAPVTS->getChildElement(i)->getAttributeValue(0), hiddenAPVTS->getChildElement(i)->getAttributeValue(1));
							}
						}
					}
					else
					{
						for (int i = 0; i < xmlData->getNumChildElements(); ++i)
						{
							parameters.set(xmlData->getChildElement(i)->getAttributeValue(0), xmlData->getChildElement(i)->getAttributeValue(1));
						}

					}

				}
			}
			PresetData(const juce::ValueTree& treeData)
				:PresetData(juce::parseXML(treeData.toXmlString()).get())
			{

			}
			PresetData(juce::File presetFile)
				:PresetData(juce::parseXML(presetFile).get())
			{

			}

			juce::XmlElement* getXML(bool onlyParameters = false)
			{
				if (!(isAPVTSOnly || onlyParameters))
				{

					xmlState = juce::XmlElement("Main");
					auto fillXML = [&](juce::String tagName, juce::StringPairArray& arr)
					{
						auto newChild = xmlState.createNewChildElement(tagName);
						auto& keys = arr.getAllKeys();
						auto& values = arr.getAllValues();
						for (int i = 0; i < keys.size(); ++i)
						{
							newChild->setAttribute(keys[i], values[i]);
						}

					};

					fillXML(InfoXMLTag, info);
					xmlState.createNewChildElement(FavoriteXMLTag)->setAttribute(FavoriteAttributeName, favorite);
					fillXML(TagsXMLTag, tags);
					fillXML(SuperTagsXMLTag, superTags);
					fillXML(MacroNamesXMLTag, macroNames);

					auto APVTS = xmlState.createNewChildElement("APVTS");
					auto& keys = parameters.getAllKeys();
					auto& values = parameters.getAllValues();
					for (int i = 0; i < keys.size(); ++i)
					{
						auto child = APVTS->createNewChildElement("PARAM");
						child->setAttribute("id", keys[i]);
						child->setAttribute("value", values[i]);
					}

					auto hiddenAPVTS = xmlState.createNewChildElement("APVTSHidden");
					if (hiddenAPVTS != nullptr)
					{
						auto& hiddenKeys = hiddenParameters.getAllKeys();
						auto& hiddenValues = hiddenParameters.getAllValues();
						for (int i = 0; i < hiddenKeys.size(); ++i)
						{
							auto child = hiddenAPVTS->createNewChildElement("PARAM");
							child->setAttribute("id", hiddenKeys[i]);
							child->setAttribute("value", hiddenValues[i]);
						}
					}
					return &xmlState;
				}
				else
				{
					xmlState = juce::XmlElement("APVTS");

					auto& keys = parameters.getAllKeys();
					auto& values = parameters.getAllValues();
					for (int i = 0; i < keys.size(); ++i)
					{
						auto child = xmlState.createNewChildElement("PARAM");
						child->setAttribute("id", keys[i]);
						child->setAttribute("value", values[i]);
					}
					return &xmlState;

				}
			}
			juce::ValueTree getTree(bool onlyParameters = false)
			{
				if (!(isAPVTSOnly || onlyParameters))
				{

					juce::ValueTree out("Main");
					auto fillTree = [&](juce::String tagName, juce::StringPairArray& arr)
					{
						auto newChild = out.getOrCreateChildWithName(tagName, nullptr);
						auto& keys = arr.getAllKeys();
						auto& values = arr.getAllValues();
						for (int i = 0; i < keys.size(); ++i)
						{
							newChild.setProperty(keys[i], juce::var(values[i]), nullptr);
						}

					};

					fillTree(InfoXMLTag, info);
					out.getOrCreateChildWithName(FavoriteXMLTag, nullptr).setProperty(FavoriteAttributeName, favorite, nullptr);
					fillTree(TagsXMLTag, tags);
					fillTree(SuperTagsXMLTag, superTags);
					fillTree(MacroNamesXMLTag, macroNames);

					auto APVTS = out.getOrCreateChildWithName("APVTS", nullptr);
					auto& keys = parameters.getAllKeys();
					auto& values = parameters.getAllValues();
					for (int i = 0; i < keys.size(); ++i)
					{
						juce::ValueTree child("PARAM");
						child.setProperty("id", juce::var(keys[i]), nullptr);
						child.setProperty("value", juce::var(values[i]), nullptr);
						APVTS.appendChild(child, nullptr);
					}
					auto hiddenAPVTS = out.getOrCreateChildWithName("APVTSHidden", nullptr);
					auto& hiddenKeys = hiddenParameters.getAllKeys();
					auto& hiddenValues = hiddenParameters.getAllValues();
					for (int i = 0; i < hiddenKeys.size(); ++i)
					{
						juce::ValueTree child("PARAM");
						child.setProperty("id", juce::var(hiddenKeys[i]), nullptr);
						child.setProperty("value", juce::var(hiddenValues[i]), nullptr);
						hiddenAPVTS.appendChild(child, nullptr);
					}
					return out;
				}
				else
				{
					juce::ValueTree out("APVTS");

					auto& keys = parameters.getAllKeys();
					auto& values = parameters.getAllValues();
					for (int i = 0; i < keys.size(); ++i)
					{
						juce::ValueTree child("PARAM");
						child.setProperty("id", juce::var(keys[i]), nullptr);
						child.setProperty("value", juce::var(values[i]), nullptr);
						out.appendChild(child, nullptr);
					}
					return out;
				}
			}
			void mergePresets(const PresetData& base) // base is unmodified, anything not present in this preset will be added from base
			{
				auto mergeArray = [=](const juce::StringPairArray& base, juce::StringPairArray& arr)
				{
					auto& keys = base.getAllKeys();
					auto& values = base.getAllValues();
					for (int i = 0; i < keys.size(); ++i)
					{
						if (!arr.containsKey(keys[i]))
						{
							arr.set(keys[i], values[i]);
						}
					}
				};

				mergeArray(base.info, info);
				mergeArray(base.tags, tags);
				mergeArray(base.superTags, superTags);
				mergeArray(base.macroNames, macroNames);
				mergeArray(base.parameters, parameters);
				mergeArray(base.hiddenParameters, hiddenParameters);
			}
			bool isValid()
			{
				return valid;
			}
		private:
			bool valid = false;
			juce::StringPairArray info, tags, superTags, macroNames, parameters, hiddenParameters;
			bool favorite = false;
			bool isAPVTSOnly = true;
			juce::XmlElement xmlState;
		};



		//================================================================================================================================================================================================




		juce::Value alteredState, presetName;
		juce::File initFile, startupFile, currentPresetFile;
		std::unique_ptr<PresetData> initData;
		juce::ValueTree initTree;

		juce::Array<FactoryPreset> factoryPresets;

		PropertiesFolder* pluginFolder = nullptr;

		juce::StringArray tagNames; // all tags for every preset in correct folder
		juce::StringArray superTagTitles;
		juce::Array<juce::StringArray> superTagNames;

		juce::ValueTree* settingsTree;

		SaveMenuState currentSaveMenuState;


		class UndoableSaveMenuStateSet : public juce::UndoableAction
		{
		public:
			UndoableSaveMenuStateSet(StateHandler* handlerToUse, SaveMenuState state);
			bool perform()	override;
			bool undo() override;
		private:
			StateHandler* handler;
			SaveMenuState newState, oldState;
		};

		void replaceSaveMenuState(SaveMenuState newState);
	private:

		juce::Array<ControlParameter*>& controlParameters;
		juce::Array<OrderedListParameter*>& orderParameters;
		juce::AudioProcessor* processor;
		juce::AudioProcessorValueTreeState* parameters, * hiddenParameters;
		PresetManager* manager = nullptr;



		juce::CriticalSection valueTreeChanging;


		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateHandler)


	};
}// namnepace bdsp
