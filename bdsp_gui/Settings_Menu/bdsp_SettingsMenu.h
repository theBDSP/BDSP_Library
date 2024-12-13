#pragma once



namespace bdsp
{
	struct Settings // holds all the settings and values for the plugin
	{
		struct Setting
		{
			juce::Identifier description;
			std::function<void(int)> changeFunc;
			juce::StringArray options;
			int v = 0; // index of selected option
			bool universal = false; // true if setting is shared between all BDSP plugins
			juce::String category;
			bool isDropDown = false;
			float dropDownMenuW = 0;

			Setting()
			{
			}

			Setting(const juce::Identifier& ID, const std::function<void(int)>& func, const juce::StringArray& choices, int val, const juce::String& Category, const std::pair<bool, float>& dropDown = std::pair<bool, float>(false, 1.5f), bool isUniversal = false)
			{
				description = ID;
				changeFunc = func;
				options = choices;
				v = val;
				category = Category;
				isDropDown = dropDown.first;
				dropDownMenuW = dropDown.second;
				universal = isUniversal;
			}


			void set(int newValue)
			{
				v = newValue;
				if (changeFunc.operator bool())
				{
					changeFunc(newValue);
				}
			}


			juce::String getKeyDescription(const juce::KeyPress& key)
			{
				return key.getTextDescriptionWithIcons();
			}

			juce::String getKeyDescription(const juce::ModifierKeys& mod)
			{
				juce::String out;

				if (mod.isAnyModifierKeyDown())
				{
					juce::KeyPress key(juce::KeyPress::F1Key, mod, 0);
					out = key.getTextDescription().upToLastOccurrenceOf(" + " + juce::KeyPress(juce::KeyPress::F1Key).getTextDescription(), false, true);
				}
				else
				{
					out = "None";
				}

				return out;
			}

		};

		//=====================================================================================================================

		Settings()
			: tree("Settings"),
			universalTree("UniversalSettings")
		{
			mods.add(juce::ModifierKeys::noModifiers);
			mods.add(juce::ModifierKeys::commandModifier);
			mods.add(juce::ModifierKeys::shiftModifier);
			mods.add(juce::ModifierKeys::altModifier);

			for (auto& m : mods)
			{
				modStrings.add(Setting().getKeyDescription(m));
			}
		}

		void set(juce::Identifier ID, int newValue)
		{
			for (auto v : arr)
			{
				if (v->description == ID) // if the setting matches the provided ID
				{
					v->set(newValue); // set the new value for the setting
					auto tagName = stringToXMLData(ID.toString());


					if (v->universal)
					{
						universalTree.setAttribute(tagName, newValue); // update the tree
					}
					else
					{
						tree.setAttribute(tagName, newValue); // update the tree
					}
					break;
				}

			}
		}

		int get(const juce::Identifier& ID, int defaultValue = 0)
		{
			int out = defaultValue;
			for (auto v : arr)
			{
				if (v->description == ID)
				{
					out = v->v;
					break;
				}
			}
			return out;
		}

		juce::String getCategory(const juce::Identifier& ID)
		{
			juce::String out;
			for (auto v : arr)
			{
				if (v->description == ID)
				{
					out = v->category;
					break;
				}
			}
			return out;
		}

		void loadSettings(const juce::File& settingsFile, const juce::File& universalSettingsFile)
		{

			XMLLocation = settingsFile;
			universalXML = universalSettingsFile;

			if (!XMLLocation.exists())
			{
				XMLLocation.create();
				XMLLocation.setReadOnly(false);
			}

			if (!universalXML.exists())
			{
				universalXML.create();
				universalXML.setReadOnly(false);
			}


			auto XML = juce::parseXML(settingsFile);

			if (XML != nullptr)
			{
				auto num = XML->getNumAttributes();
				for (int i = 0; i < num; ++i)
				{
					set(XMLDataToString(XML->getAttributeName(i)), XML->getAttributeValue(i).getIntValue());
				}
			}


			//================================================================================================================================================================================================
			XML = juce::parseXML(universalSettingsFile);

			if (XML != nullptr)
			{
				auto num = XML->getNumAttributes();
				for (int i = 0; i < num; ++i)
				{
					set(XMLDataToString(XML->getAttributeName(i)), XML->getAttributeValue(i).getIntValue());
				}
			}

		}

		void saveSettings()
		{
			tree.writeTo(XMLLocation);
			universalTree.writeTo(universalXML);
		}

		class SettingSorter
		{
		public:
			SettingSorter(const juce::StringArray& categoryOrder, bool alphabetizeSettings)
			{
				catOrder = categoryOrder;
				alphabetize = alphabetizeSettings;
			}

			int compareElements(Setting* first, Setting* second)
			{

				int cat = catOrder.indexOf(first->category) - catOrder.indexOf(second->category);
				int desc = alphabetize ? first->description.toString().compareNatural(second->description.toString()) : 0;


				return cat == 0 ? desc : cat;
			}

		private:
			juce::StringArray catOrder;
			bool alphabetize;
		};

		void sort(const juce::StringArray& categoryOrder = juce::StringArray(), bool alphabetize = false)
		{
			juce::Array<Setting*> settingReferences;
			for (auto& array : exclusionGroups)
			{
				for (auto i : array)
				{
					settingReferences.add(arr[i]);
				}
			}
			if (categoryOrder.isEmpty())
			{
				juce::StringArray alphabetized;
				for (auto s : arr)
				{
					if (!alphabetized.contains(s->category))
					{
						alphabetized.add(s->category);
					}
				}
				alphabetized.sortNatural();
				SettingSorter sorter(alphabetized, alphabetize);
				arr.sort(sorter, !alphabetize);
			}
			else
			{
				SettingSorter sorter(categoryOrder, alphabetize);
				arr.sort(sorter, !alphabetize);
			}

			int k = 0;
			for (int i = 0; i < exclusionGroups.size(); ++i)
			{
				juce::Array<int> temp;
				for (int j = 0; j < exclusionGroups[i].size(); ++j)
				{
					temp.set(j, arr.indexOf(settingReferences[k++]));
				}
				exclusionGroups.set(i, temp);
			}
		}


		juce::XmlElement tree, universalTree;
		juce::File XMLLocation, universalXML;
		juce::OwnedArray<Setting> arr;

		juce::StringArray modStrings;
		juce::Array<juce::ModifierKeys::Flags> mods;
		juce::Array<juce::Array<int>> exclusionGroups;
	};





	class SettingsMenu : public juce::Component, public ComponentCore, public juce::Button::Listener
	{
	public:
		SettingsMenu(GUI_Universals* universalsToUse, Settings* structToLink, const juce::File& userManual);

		~SettingsMenu() = default;


		void load(const juce::File& settingsFile, const juce::File& universalSettingsFile);

		void addButtons(const juce::Identifier& description, const juce::StringArray& options, const juce::String& category);
		void addDropDown(const juce::Identifier& description, const juce::String& category, const juce::StringArray& options, float menuW = 1.5);


		void setExclusiveSelectionGroup(const juce::Array<int>& indecies);

		int getSettingValue(const juce::Identifier& ID);


		void resized() override;

		void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;

		void linkButton(Button* b);

		// Inherited via Listener
		void buttonClicked(juce::Button* b) override;

		;
	private:
		class SettingComponent : public juce::Component, public ComponentCore
		{
		public:
			SettingComponent(SettingsMenu* parent, const juce::String& description, const juce::String& category);
			~SettingComponent() = default;

			void addButtons(const juce::StringArray& options);

			void setGroup(const juce::Array<SettingComponent*>& restOfGroup);

			void setValue(int idx);

			void resized() override;

			void paint(juce::Graphics& g) override;



			juce::String descriptionText;
			juce::String categoryText;

			juce::OwnedArray<TextButton> buttons;
		private:
			SettingsMenu* p;



			juce::Array<SettingComponent*> othersInGroup;

		};


		//================================================================================================================================================================================================


		class DropDownSettingsComponent : public bdsp::Component
		{
		public:

			DropDownSettingsComponent(SettingsMenu* parent, const juce::String& description, const juce::String& category, const juce::StringArray& options, float menuW);
			~DropDownSettingsComponent() = default;


			void setGroup(const juce::Array<DropDownSettingsComponent*>& restOfGroup);

			void setValue(int idx);


			void resized() override;

			void paint(juce::Graphics& g) override;

			ComboBox* getComboBox();

			juce::String descriptionText;
			juce::String categoryText;

			std::unique_ptr<ComboBox> cb;
		private:
			SettingsMenu* p;

			juce::Array<DropDownSettingsComponent*> othersInGroup;

			float menuWidth = 0;

		};

		Viewport vp;

		juce::Component settingsuniversals;

		TextButton manual;

		juce::File manualLocation;


		Settings* settingsStruct;

	public:

		juce::OwnedArray<SettingComponent> settings;
		juce::OwnedArray<DropDownSettingsComponent> dropDowns;



		juce::Array<juce::Component*> compOrder;



		int border;
		float CategoryHScale = 2;

	};





}// namnepace bdsp
