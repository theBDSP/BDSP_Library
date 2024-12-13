#include "bdsp_Manager.h"
namespace bdsp
{

	PresetManager::PresetManager(GUI_Universals* universalsToUse, StateHandler* handlerToUse, AlertWindow* alertToUse)
		:fav(universalsToUse, true),
		left(universalsToUse),
		right(universalsToUse),
		titleBarHamburgerMenu(universalsToUse, true),
		browser(this, universalsToUse),
		titleBar(this, universalsToUse),
		pc(universalsToUse)
	{
		universals = universalsToUse;
		Handler = handlerToUse;
		Handler->setManager(this);
		Handler->alteredState.addListener(this);
		Handler->presetName.addListener(this);


		universals->undoManager->addChangeListener(this);
		alert = alertToUse;
		pFolder = Handler->pluginFolder;



		tagMenu = std::make_unique<TagMenu>(this);
		menu = std::make_unique<PresetMenu>(this);
		saveMenu = std::make_unique<SaveMenu>(this);


		menu->addAndMakeVisible(tagMenu.get());


		left.onClick = [this]()
		{
			menu->scroll(true);
		};

		right.onClick = [this]()
		{
			menu->scroll(false);
		};

		fav.onStateChange = [=]()
		{
			fav.setHintBarText((fav.getToggleState() ? juce::String("Un-Favorites ") : juce::String("Favorites ")) + "this preset");
		};

		fav.onClick = [=]()
		{
			if (loadedPreset != nullptr)
			{

				updateFavorite(loadedPreset, fav.getToggleState()); // fav has already toggled on click

			}
		};


		//open.getMaxText = [=]()
		//{
		//	returnuniversals->Fonts[open.getFontIndex()].getWidestString(BDSP_MAX_PRESET_NAME_LENGTH / 2.5, BDSP_PRESET_NAME_ALLOWED_CHARACTERS);
		//};


		//Load.onClick = [=]()
		//{
		//	openBrowser();
		//};

		//revert.onClick = [=]()
		//{
		//	menu->resetHighlights();
		//	if (undoState.item != nullptr)
		//	{
		//		menu->toggleSingleHighlight(undoState.item);
		//	}

		//	loadedPreset = undoState.item;
		//	Handler->loadPreset(undoState.state);
		//	Handler->presetName.setValue(undoState.name);
		//	Handler->alteredState.setValue(undoState.altered);

		//	for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
		//	{
		//		if (undoState.macroNames[i].isNotEmpty())
		//		{
		//			macroNameValueLocation->setPropertyExcludingListener(Handler, "Macro" + juce::String(i + 1) + "NameID", undoState.macroNames[i], nullptr);
		//		}
		//		else
		//		{
		//			macroNameValueLocation->setPropertyExcludingListener(Handler, "Macro" + juce::String(i + 1) + "NameID", "Macro " + juce::String(i + 1), nullptr);
		//		}
		//	}


		//};





		//open.textUp = pc.tagBKGDSelected;
		//open.textDown = pc.tagBKGDSelected;

		//open.backgroundDown = juce::Colour();
		//open.backgroundUp = juce::Colour();

		updateLoadedPreset(menu->findItem(Handler->presetName.toString()));









		Shape s(pc.universals->commonPaths.star, pc.presetText, juce::Rectangle<float>(1, 1), pc.presetText, strokeThicc);
		s.fillOff = NamedColorsIdentifier();
		fav.addShapes(s);

		setFontIndex(titleBar.getFontIndex(), 0.1);
		//auto triWidth = 2.5;
		// 
		//juce::Path leftPathTemp;
		//leftPathTemp.addTriangle(0, 0, triWidth, 1, triWidth, -1);


		//down.addShapes(Shape(leftPathTemp, pc.tagBKGDSelected, juce::Rectangle<float>(0, 0.25, 1, 0.5), pc.tagBKGDSelected, strokeThicc));



		//juce::Path rightPathTemp;
		//rightPathTemp.addTriangle(0, 0, -triWidth, 1, -triWidth, -1);




		//================================================================================================================================================================================================

		//Save.setText("Save");

		//Save.backgroundDown = juce::Colour();
		//Save.backgroundUp = juce::Colour();

		//Save.textDown = pc.scrollBar;
		//Save.textUp = pc.scrollBarDown;

		//Save.setHasOutline(false);

		//Load.setText("Load");

		//Load.backgroundDown = juce::Colour();
		//Load.backgroundUp = juce::Colour();

		//Load.textDown = Save.textDown;
		//Load.textUp = Save.textUp;
		//Load.setHasOutline(false);






		//================================================================================================================================================================================================





		titleBarPopup = std::make_unique<PopupMenu>(&titleBarHamburgerMenu, universals);
		titleBarPopup->List.addItem("Browse Presets", 0);
		titleBarPopup->List.addItem("Save Preset", 1);
		titleBarPopup->List.addItem("Initialize Preset", 2);

		titleBarPopup->List.onSelect = [=](int i)
		{
			if (i == 0)
			{
				browser.isVisible() ? closeBrowser() : openBrowser();
			}
			else if (i == 1)
			{
				if (loadedPreset == nullptr)
				{
					saveNewPreset();
				}
				else
				{
					overwritePreset(loadedPreset);
				}

			}
			else if (i == 2)
			{
				initializePlugin();
			}

			titleBarPopup->List.clearHighlights();
		};

		titleBarPopup->onShow = [=]()
		{
			auto b = titleBarHamburgerMenu.getBoundsRelativeToDesktopManager().toFloat().translated(0, titleBarHamburgerMenu.getHeight()).withSizeKeepingCentre(1.5 * pc.universals->sliderPopupWidth, 2);
			titleBarPopup->setBounds(expandRectangleToInt(b.withHeight(titleBarPopup->List.getIdealHeight(b.getWidth()))));

		};

		titleBarPopup->List.setColorSchemeClassic(pc.browserBKGD, NamedColorsIdentifier(), pc.presetText, pc.itemHighlight);

		titleBarHamburgerMenu.onClick = [=]()
		{
			titleBarPopup->show();
		};

		//================================================================================================================================================================================================


		browser.addAndMakeVisible(menu.get());


		//================================================================================================================================================================================================

		titleBar.addAndMakeVisible(left);
		titleBar.addAndMakeVisible(right);
		titleBar.addAndMakeVisible(fav);
		titleBar.addAndMakeVisible(titleBarHamburgerMenu);



	}

	PresetManager::~PresetManager()
	{
		Handler->alteredState.removeListener(this);
		Handler->presetName.removeListener(this);

		universals->undoManager->removeChangeListener(this);

	}


	void PresetManager::setFontIndex(int idx, float revertRatio)
	{
		right.clearShapes();
		left.clearShapes();
		titleBarHamburgerMenu.clearShapes();



		titleBar.setFontIndex(idx);
		titleBar.nameButton->setFontIndex(idx);


		juce::PathStrokeType stroke(2 * revertRatio, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
		auto rect = juce::Rectangle<float>(0.2, 0.2, 0.6, 0.6);

		//================================================================================================================================================================================================




		//================================================================================================================================================================================================



		static const unsigned char hamburgerData[] = { 110,109,159,34,90,196,227,126,99,196,108,159,34,90,196,227,126,99,196,108,189,36,90,196,213,126,99,196,108,217,38,90,196,171,126,99,196,108,243,40,90,196,103,126,99,196,108,8,43,90,196,8,126,99,196,108,24,45,90,196,142,125,99,196,108,33,47,90,196,250,
		124,99,196,108,34,49,90,196,76,124,99,196,108,26,51,90,196,133,123,99,196,108,7,53,90,196,165,122,99,196,108,233,54,90,196,172,121,99,196,108,189,56,90,196,156,120,99,196,108,132,58,90,196,117,119,99,196,108,58,60,90,196,55,118,99,196,108,225,61,90,196,
		228,116,99,196,108,118,63,90,196,124,115,99,196,108,248,64,90,196,0,114,99,196,108,103,66,90,196,113,112,99,196,108,194,67,90,196,209,110,99,196,108,7,69,90,196,32,109,99,196,108,55,70,90,196,95,107,99,196,108,79,71,90,196,143,105,99,196,108,80,72,90,
		196,178,103,99,196,108,57,73,90,196,201,101,99,196,108,9,74,90,196,213,99,99,196,108,192,74,90,196,215,97,99,196,108,93,75,90,196,208,95,99,196,108,224,75,90,196,195,93,99,196,108,73,76,90,196,175,91,99,196,108,150,76,90,196,151,89,99,196,108,201,76,
		90,196,123,87,99,196,108,225,76,90,196,94,85,99,196,108,227,76,90,196,125,84,99,196,108,227,76,90,196,125,84,99,196,108,227,76,90,196,125,84,99,196,108,214,76,90,196,95,82,99,196,108,174,76,90,196,67,80,99,196,108,107,76,90,196,41,78,99,196,108,13,76,
		90,196,19,76,99,196,108,149,75,90,196,3,74,99,196,108,2,75,90,196,250,71,99,196,108,85,74,90,196,248,69,99,196,108,143,73,90,196,0,68,99,196,108,176,72,90,196,18,66,99,196,108,185,71,90,196,48,64,99,196,108,170,70,90,196,91,62,99,196,108,131,69,90,196,
		148,60,99,196,108,71,68,90,196,220,58,99,196,108,244,66,90,196,53,57,99,196,108,141,65,90,196,159,55,99,196,108,19,64,90,196,28,54,99,196,108,133,62,90,196,172,52,99,196,108,229,60,90,196,80,51,99,196,108,53,59,90,196,10,50,99,196,108,117,57,90,196,217,
		48,99,196,108,166,55,90,196,192,47,99,196,108,202,53,90,196,190,46,99,196,108,225,51,90,196,212,45,99,196,108,237,49,90,196,2,45,99,196,108,239,47,90,196,74,44,99,196,108,233,45,90,196,172,43,99,196,108,220,43,90,196,40,43,99,196,108,201,41,90,196,190,
		42,99,196,108,177,39,90,196,111,42,99,196,108,149,37,90,196,58,42,99,196,108,120,35,90,196,33,42,99,196,108,159,34,90,196,31,42,99,196,108,159,34,90,196,31,42,99,196,108,1,106,87,196,31,42,99,196,108,1,106,87,196,31,42,99,196,108,227,103,87,196,44,42,
		99,196,108,199,101,87,196,84,42,99,196,108,173,99,87,196,151,42,99,196,108,151,97,87,196,245,42,99,196,108,135,95,87,196,110,43,99,196,108,126,93,87,196,0,44,99,196,108,124,91,87,196,173,44,99,196,108,132,89,87,196,115,45,99,196,108,150,87,87,196,82,
		46,99,196,108,180,85,87,196,74,47,99,196,108,223,83,87,196,89,48,99,196,108,24,82,87,196,127,49,99,196,108,96,80,87,196,188,50,99,196,108,185,78,87,196,14,52,99,196,108,35,77,87,196,117,53,99,196,108,160,75,87,196,240,54,99,196,108,48,74,87,196,126,56,
		99,196,108,213,72,87,196,29,58,99,196,108,142,71,87,196,206,59,99,196,108,94,70,87,196,142,61,99,196,108,68,69,87,196,93,63,99,196,108,66,68,87,196,57,65,99,196,108,88,67,87,196,34,67,99,196,108,135,66,87,196,22,69,99,196,108,207,65,87,196,19,71,99,196,
		108,49,65,87,196,26,73,99,196,108,172,64,87,196,39,75,99,196,108,67,64,87,196,58,77,99,196,108,244,63,87,196,82,79,99,196,108,191,63,87,196,110,81,99,196,108,166,63,87,196,139,83,99,196,108,164,63,87,196,125,84,99,196,108,164,63,87,196,125,84,99,196,
		108,164,63,87,196,125,84,99,196,108,176,63,87,196,155,86,99,196,108,216,63,87,196,183,88,99,196,108,27,64,87,196,209,90,99,196,108,120,64,87,196,231,92,99,196,108,240,64,87,196,247,94,99,196,108,131,65,87,196,1,97,99,196,108,47,66,87,196,2,99,99,196,
		108,245,66,87,196,251,100,99,196,108,211,67,87,196,233,102,99,196,108,202,68,87,196,203,104,99,196,108,217,69,87,196,160,106,99,196,108,255,70,87,196,103,108,99,196,108,59,72,87,196,31,110,99,196,108,141,73,87,196,199,111,99,196,108,244,74,87,196,93,
		113,99,196,108,111,76,87,196,225,114,99,196,108,252,77,87,196,81,116,99,196,108,155,79,87,196,173,117,99,196,108,76,81,87,196,244,118,99,196,108,12,83,87,196,36,120,99,196,108,218,84,87,196,62,121,99,196,108,182,86,87,196,65,122,99,196,108,159,88,87,
		196,43,123,99,196,108,146,90,87,196,253,123,99,196,108,144,92,87,196,181,124,99,196,108,150,94,87,196,84,125,99,196,108,163,96,87,196,217,125,99,196,108,183,98,87,196,67,126,99,196,108,207,100,87,196,146,126,99,196,108,234,102,87,196,199,126,99,196,108,
		7,105,87,196,225,126,99,196,108,1,106,87,196,227,126,99,196,108,1,106,87,196,227,126,99,196,108,159,34,90,196,227,126,99,196,99,109,159,34,90,196,129,177,98,196,108,159,34,90,196,129,177,98,196,108,189,36,90,196,115,177,98,196,108,217,38,90,196,73,177,
		98,196,108,243,40,90,196,5,177,98,196,108,8,43,90,196,166,176,98,196,108,24,45,90,196,44,176,98,196,108,33,47,90,196,152,175,98,196,108,34,49,90,196,234,174,98,196,108,26,51,90,196,35,174,98,196,108,8,53,90,196,67,173,98,196,108,233,54,90,196,74,172,
		98,196,108,189,56,90,196,58,171,98,196,108,132,58,90,196,19,170,98,196,108,58,60,90,196,213,168,98,196,108,225,61,90,196,130,167,98,196,108,118,63,90,196,26,166,98,196,108,248,64,90,196,158,164,98,196,108,103,66,90,196,15,163,98,196,108,194,67,90,196,
		111,161,98,196,108,7,69,90,196,190,159,98,196,108,55,70,90,196,253,157,98,196,108,79,71,90,196,45,156,98,196,108,80,72,90,196,80,154,98,196,108,57,73,90,196,103,152,98,196,108,9,74,90,196,115,150,98,196,108,192,74,90,196,117,148,98,196,108,93,75,90,196,
		110,146,98,196,108,224,75,90,196,97,144,98,196,108,73,76,90,196,77,142,98,196,108,150,76,90,196,53,140,98,196,108,201,76,90,196,25,138,98,196,108,225,76,90,196,252,135,98,196,108,227,76,90,196,35,135,98,196,108,227,76,90,196,35,135,98,196,108,227,76,
		90,196,35,135,98,196,108,213,76,90,196,5,133,98,196,108,171,76,90,196,233,130,98,196,108,103,76,90,196,207,128,98,196,108,8,76,90,196,186,126,98,196,108,142,75,90,196,170,124,98,196,108,250,74,90,196,161,122,98,196,108,76,74,90,196,160,120,98,196,108,
		133,73,90,196,168,118,98,196,108,165,72,90,196,186,116,98,196,108,172,71,90,196,217,114,98,196,108,156,70,90,196,5,113,98,196,108,117,69,90,196,62,111,98,196,108,55,68,90,196,136,109,98,196,108,228,66,90,196,225,107,98,196,108,124,65,90,196,76,106,98,
		196,108,0,64,90,196,202,104,98,196,108,113,62,90,196,91,103,98,196,108,209,60,90,196,0,102,98,196,108,32,59,90,196,187,100,98,196,108,95,57,90,196,139,99,98,196,108,143,55,90,196,115,98,98,196,108,178,53,90,196,114,97,98,196,108,201,51,90,196,137,96,
		98,196,108,213,49,90,196,185,95,98,196,108,215,47,90,196,2,95,98,196,108,208,45,90,196,101,94,98,196,108,195,43,90,196,226,93,98,196,108,175,41,90,196,121,93,98,196,108,151,39,90,196,44,93,98,196,108,123,37,90,196,249,92,98,196,108,94,35,90,196,225,92,
		98,196,108,159,34,90,196,223,92,98,196,108,159,34,90,196,223,92,98,196,108,1,106,87,196,223,92,98,196,108,1,106,87,196,223,92,98,196,108,227,103,87,196,236,92,98,196,108,199,101,87,196,20,93,98,196,108,173,99,87,196,87,93,98,196,108,151,97,87,196,181,
		93,98,196,108,135,95,87,196,46,94,98,196,108,126,93,87,196,192,94,98,196,108,124,91,87,196,109,95,98,196,108,132,89,87,196,51,96,98,196,108,150,87,87,196,18,97,98,196,108,180,85,87,196,10,98,98,196,108,223,83,87,196,25,99,98,196,108,24,82,87,196,63,100,
		98,196,108,96,80,87,196,124,101,98,196,108,185,78,87,196,206,102,98,196,108,35,77,87,196,53,104,98,196,108,160,75,87,196,176,105,98,196,108,48,74,87,196,62,107,98,196,108,213,72,87,196,221,108,98,196,108,142,71,87,196,142,110,98,196,108,94,70,87,196,
		78,112,98,196,108,68,69,87,196,29,114,98,196,108,66,68,87,196,249,115,98,196,108,88,67,87,196,226,117,98,196,108,135,66,87,196,214,119,98,196,108,207,65,87,196,211,121,98,196,108,49,65,87,196,218,123,98,196,108,172,64,87,196,231,125,98,196,108,67,64,
		87,196,250,127,98,196,108,244,63,87,196,18,130,98,196,108,191,63,87,196,46,132,98,196,108,166,63,87,196,75,134,98,196,108,164,63,87,196,35,135,98,196,108,164,63,87,196,35,135,98,196,108,164,63,87,196,35,135,98,196,108,177,63,87,196,65,137,98,196,108,
		217,63,87,196,93,139,98,196,108,28,64,87,196,119,141,98,196,108,122,64,87,196,141,143,98,196,108,242,64,87,196,157,145,98,196,108,133,65,87,196,166,147,98,196,108,50,66,87,196,168,149,98,196,108,248,66,87,196,160,151,98,196,108,215,67,87,196,142,153,
		98,196,108,206,68,87,196,112,155,98,196,108,221,69,87,196,69,157,98,196,108,4,71,87,196,12,159,98,196,108,64,72,87,196,196,160,98,196,108,147,73,87,196,107,162,98,196,108,250,74,87,196,1,164,98,196,108,116,76,87,196,132,165,98,196,108,2,78,87,196,244,
		166,98,196,108,162,79,87,196,80,168,98,196,108,82,81,87,196,150,169,98,196,108,18,83,87,196,199,170,98,196,108,225,84,87,196,224,171,98,196,108,190,86,87,196,226,172,98,196,108,166,88,87,196,204,173,98,196,108,154,90,87,196,158,174,98,196,108,152,92,
		87,196,86,175,98,196,108,158,94,87,196,244,175,98,196,108,171,96,87,196,120,176,98,196,108,190,98,87,196,226,176,98,196,108,214,100,87,196,49,177,98,196,108,242,102,87,196,102,177,98,196,108,15,105,87,196,127,177,98,196,108,1,106,87,196,129,177,98,196,
		108,1,106,87,196,129,177,98,196,108,159,34,90,196,129,177,98,196,99,109,159,34,90,196,65,228,97,196,108,159,34,90,196,65,228,97,196,108,189,36,90,196,51,228,97,196,108,217,38,90,196,9,228,97,196,108,243,40,90,196,197,227,97,196,108,8,43,90,196,102,227,
		97,196,108,24,45,90,196,236,226,97,196,108,33,47,90,196,88,226,97,196,108,34,49,90,196,170,225,97,196,108,26,51,90,196,227,224,97,196,108,8,53,90,196,3,224,97,196,108,233,54,90,196,10,223,97,196,108,189,56,90,196,250,221,97,196,108,132,58,90,196,211,
		220,97,196,108,58,60,90,196,149,219,97,196,108,225,61,90,196,66,218,97,196,108,118,63,90,196,218,216,97,196,108,248,64,90,196,94,215,97,196,108,103,66,90,196,207,213,97,196,108,194,67,90,196,47,212,97,196,108,7,69,90,196,126,210,97,196,108,55,70,90,196,
		189,208,97,196,108,79,71,90,196,237,206,97,196,108,80,72,90,196,16,205,97,196,108,57,73,90,196,39,203,97,196,108,9,74,90,196,51,201,97,196,108,192,74,90,196,53,199,97,196,108,93,75,90,196,46,197,97,196,108,224,75,90,196,33,195,97,196,108,73,76,90,196,
		13,193,97,196,108,150,76,90,196,245,190,97,196,108,201,76,90,196,217,188,97,196,108,225,76,90,196,188,186,97,196,108,227,76,90,196,227,185,97,196,108,227,76,90,196,227,185,97,196,108,227,76,90,196,227,185,97,196,108,215,76,90,196,197,183,97,196,108,175,
		76,90,196,169,181,97,196,108,108,76,90,196,143,179,97,196,108,15,76,90,196,121,177,97,196,108,151,75,90,196,105,175,97,196,108,4,75,90,196,95,173,97,196,108,88,74,90,196,94,171,97,196,108,146,73,90,196,101,169,97,196,108,180,72,90,196,119,167,97,196,
		108,189,71,90,196,149,165,97,196,108,174,70,90,196,192,163,97,196,108,136,69,90,196,249,161,97,196,108,76,68,90,196,65,160,97,196,108,250,66,90,196,153,158,97,196,108,147,65,90,196,3,157,97,196,108,24,64,90,196,127,155,97,196,108,139,62,90,196,15,154,
		97,196,108,236,60,90,196,179,152,97,196,108,59,59,90,196,108,151,97,196,108,123,57,90,196,60,150,97,196,108,173,55,90,196,34,149,97,196,108,209,53,90,196,31,148,97,196,108,232,51,90,196,53,147,97,196,108,245,49,90,196,99,146,97,196,108,247,47,90,196,
		171,145,97,196,108,241,45,90,196,12,145,97,196,108,228,43,90,196,135,144,97,196,108,208,41,90,196,29,144,97,196,108,184,39,90,196,206,143,97,196,108,157,37,90,196,153,143,97,196,108,128,35,90,196,127,143,97,196,108,159,34,90,196,125,143,97,196,108,159,
		34,90,196,125,143,97,196,108,1,106,87,196,125,143,97,196,108,1,106,87,196,125,143,97,196,108,227,103,87,196,138,143,97,196,108,199,101,87,196,178,143,97,196,108,173,99,87,196,245,143,97,196,108,151,97,87,196,83,144,97,196,108,135,95,87,196,204,144,97,
		196,108,126,93,87,196,94,145,97,196,108,124,91,87,196,11,146,97,196,108,132,89,87,196,209,146,97,196,108,150,87,87,196,176,147,97,196,108,180,85,87,196,168,148,97,196,108,223,83,87,196,183,149,97,196,108,24,82,87,196,221,150,97,196,108,96,80,87,196,26,
		152,97,196,108,185,78,87,196,108,153,97,196,108,35,77,87,196,211,154,97,196,108,160,75,87,196,78,156,97,196,108,48,74,87,196,220,157,97,196,108,213,72,87,196,123,159,97,196,108,142,71,87,196,44,161,97,196,108,94,70,87,196,236,162,97,196,108,68,69,87,
		196,187,164,97,196,108,66,68,87,196,151,166,97,196,108,88,67,87,196,128,168,97,196,108,135,66,87,196,116,170,97,196,108,207,65,87,196,113,172,97,196,108,49,65,87,196,120,174,97,196,108,172,64,87,196,133,176,97,196,108,67,64,87,196,152,178,97,196,108,
		244,63,87,196,176,180,97,196,108,191,63,87,196,204,182,97,196,108,166,63,87,196,233,184,97,196,108,164,63,87,196,227,185,97,196,108,164,63,87,196,227,185,97,196,108,164,63,87,196,227,185,97,196,108,177,63,87,196,1,188,97,196,108,217,63,87,196,29,190,
		97,196,108,28,64,87,196,55,192,97,196,108,122,64,87,196,77,194,97,196,108,242,64,87,196,93,196,97,196,108,133,65,87,196,102,198,97,196,108,50,66,87,196,104,200,97,196,108,248,66,87,196,96,202,97,196,108,215,67,87,196,78,204,97,196,108,206,68,87,196,48,
		206,97,196,108,221,69,87,196,5,208,97,196,108,4,71,87,196,204,209,97,196,108,64,72,87,196,132,211,97,196,108,147,73,87,196,43,213,97,196,108,250,74,87,196,193,214,97,196,108,116,76,87,196,68,216,97,196,108,2,78,87,196,180,217,97,196,108,162,79,87,196,
		16,219,97,196,108,82,81,87,196,86,220,97,196,108,18,83,87,196,135,221,97,196,108,225,84,87,196,160,222,97,196,108,190,86,87,196,162,223,97,196,108,166,88,87,196,140,224,97,196,108,154,90,87,196,94,225,97,196,108,152,92,87,196,22,226,97,196,108,158,94,
		87,196,180,226,97,196,108,171,96,87,196,56,227,97,196,108,190,98,87,196,162,227,97,196,108,214,100,87,196,241,227,97,196,108,242,102,87,196,38,228,97,196,108,15,105,87,196,63,228,97,196,108,1,106,87,196,65,228,97,196,108,1,106,87,196,65,228,97,196,108,
		159,34,90,196,65,228,97,196,99,101,0,0 };

		juce::Path hamburgerPath;
		hamburgerPath.loadPathFromData(hamburgerData, sizeof(hamburgerData));

		titleBarHamburgerMenu.addShapes(Shape(hamburgerPath, pc.presetText, juce::Rectangle<float>(0, 0, 1, 1)));
		//================================================================================================================================================================================================


		juce::Path p;
		p.startNewSubPath(0, 0);
		p.lineTo(1, -1);
		p.lineTo(0, -2);

		juce::PathStrokeType(revertRatio).createStrokedPath(p, p);
		right.addShapes(Shape(p, pc.presetText, juce::Rectangle<float>(0, 0.25, 1, 0.5)));


		p.applyTransform(juce::AffineTransform().rotated(PI));
		left.addShapes(Shape(p, pc.presetText, juce::Rectangle<float>(0, 0.25, 1, 0.5)));



	}


	void PresetManager::saveNewPreset()
	{
		//saveMenu->clear();

		saveMenu->matchPreviousState(&Handler->currentSaveMenuState);

		saveMenu->show();
	}

	void PresetManager::updateFavorite(PresetMenuItem* MI, bool state)
	{
		Handler->updatePreset(MI->getFile(), state);
		MI->favIcon.setToggleState(state, juce::dontSendNotification);

		if (MI == loadedPreset)
		{
			fav.setToggleState(state, juce::sendNotification);
		}

		if (menu->currentSorting.fav)
		{
			menu->reSort(menu->currentSorting.col, menu->currentSorting.rev);
		}
	}

	void PresetManager::overwritePreset(PresetMenuItem* MI)
	{
		if (Handler->currentSaveMenuState.presetName.compare(MI->getFile().getFileNameWithoutExtension()) != 0) // no saved state or overriding different preset from one loaded
		{
			saveMenu->matchItem(MI);
		}
		else
		{
			saveMenu->matchPreviousState(&Handler->currentSaveMenuState);
		}
		saveMenu->show();
	}

	void PresetManager::loadPreset(PresetMenuItem* MI, bool performOnLoadFunction, bool isUndoable)
	{
		loadedPreset = MI;

		if (MI != nullptr)
		{
			Handler->loadPreset(MI->getFile(), isUndoable);
			fav.setToggleState(MI->isFavorite(), juce::sendNotification);
			saveMenu->hide();
			saveMenu->clear();
			saveMenu->createHandlerSaveMenuState(isUndoable);
		}

		if (performOnLoadFunction && onPresetLoad.operator bool())
		{
			onPresetLoad();
		}




	}




	void PresetManager::initializePlugin(bool isUndoable)
	{
		Handler->initializePlugin(isUndoable);
		menu->resetHighlights();
		saveMenu->clear();
		saveMenu->createHandlerSaveMenuState(isUndoable);

		if (onPresetLoad.operator bool())
		{
			onPresetLoad();
		}
	}

	void PresetManager::openBrowser()
	{
		browser.setVisible(true);
		//browser.grabKeyboardFocus();
		tagMenu->setVisible(true);
		tagMenu->initMenuOpen();

		titleBarPopup->List.renameItem(0, "Close Browser");

		menu->initToProperItem();
		browserOpenClose(true);



	}

	void PresetManager::closeBrowser()
	{
		//menu->clearSearch.triggerClick();
		//tagMenu->clearTags();
		tagMenu->setVisible(false);
		browser.setVisible(false);
		browser.giveAwayKeyboardFocus();

		titleBarPopup->List.renameItem(0, "Browse Presets");


		browserOpenClose(false);


	}

	void PresetManager::valueChanged(juce::Value& value)
	{
		//if (value.refersToSameSourceAs(Handler->alteredState))
		//{
		//	if (value.getValue().operator bool())
		//	{
		//		//auto name = open.getText().trimCharactersAtStart("*");
		//		//open.setText("*" + name);

		//		presetName = "*" + presetName.trimCharactersAtStart("*");
		//	}
		//	else
		//	{
		//		presetName = Handler->presetName.toString();
		//		//open.setText(Handler->presetName.toString());
		//	}
		//}

		//else if (value.refersToSameSourceAs(Handler->presetName))
		//{
		//	//open.setText(value.getValue().toString());
		//	presetName = value.getValue().toString();
		//}

		if (value.refersToSameSourceAs(Handler->presetName))
		{
			updateLoadedPreset(menu->findItem(Handler->presetName.toString()));
		}
		titleBar.repaint();
	}

	void PresetManager::updateLoadedPreset(PresetMenuItem* MI)
	{
		loadedPreset = MI;
		if (loadedPreset != nullptr)
		{
			fav.setToggleState(loadedPreset->isFavorite(), juce::sendNotification);

			menu->getHighlightQueue().clear();
			menu->getHighlightQueue().add(loadedPreset);

			menu->updateHighlights();
		}
		else
		{
			fav.setToggleState(false, juce::sendNotification);
		}
		titleBar.repaint();
	}

	void PresetManager::changeListenerCallback(juce::ChangeBroadcaster* source)
	{
		auto cast = dynamic_cast<juce::UndoManager*>(source);
		jassert(cast != nullptr);


		if (!cast->canUndo() || cast->getCurrentTransactionName().compare("Change Preset") == 0) // undone back to original preset
		{
			Handler->settingsTree->setProperty("PresetAltered", false, nullptr);
		}
		else
		{
			Handler->settingsTree->setProperty("PresetAltered", true, nullptr);
		}

	}



	//================================================================================================================================================================================================





	//================================================================================================================================================================================================




	//================================================================================================================================================================================================


	PresetManager::Browser::Browser(PresetManager* p, GUI_Universals* universalsToUse)
	{
		parent = p;
	}

	void PresetManager::Browser::resized()
	{
		parent->scrollBarW = getWidth() * 0.001;
		auto border = getHeight() * 0.02;
		//parent->menu->setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(0, 0.05, 1, 0.95)).withBottom(getHeight() - macroH - borderColor));
		parent->menu->tagMenuH = getHeight() * 0.3;
		parent->menu->setBounds(getLocalBounds().reduced(border));

		//parent->alert->setBounds(getScreenBounds());





	}

	void PresetManager::Browser::paint(juce::Graphics& g)
	{
		g.setColour(parent->universals->colors.getColor(parent->pc.browserBKGD));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), parent->universals->roundedRectangleCurve);



		//auto bot = parent->menu->getY() + parent->menu->search.getY() - getY();

		//g.setColour(parent->universals->colors.getColor(parent->pc.presetText));
		//drawText(g, parent->universals->Fonts[1].getFont(), "Presets", getLocalBounds().withBottom(bot), true, juce::Justification::centred, 0.5);

		drawText(g, parent->universals->Fonts[1].getFont(), parent->numItems + parent->totalItems, getLocalBounds().withTop(getLocalPoint(parent->menu.get(), parent->menu->vp.getBounds().getBottomLeft()).y), false, juce::Justification::centred);

	}

	void PresetManager::Browser::setMacroH(int newH)
	{
		macroH = newH;
	}





	//================================================================================================================================================================================================


	PresetManager::TitleBar::TitleBar(PresetManager* p, GUI_Universals* universalsToUse)
		:Component(universalsToUse)
	{
		parent = p;

		nameButton = std::make_unique<TitleBarButton>(this, universalsToUse);
		addAndMakeVisible(nameButton.get());

	}




	void PresetManager::TitleBar::resized()
	{
		auto w = 0.075;


		auto square = confineToAspectRatio(getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, 0.25, w, 0.5)), 1);

		parent->left.setBounds(expandRectangleToInt(square.withX(getWidth() * 0.25 * w)));
		parent->fav.setBounds(expandRectangleToInt(square.withX(getWidth() * 0.25 * w + square.getWidth())));
		parent->right.setBounds(expandRectangleToInt(square.withX(getWidth() * 0.25 * w + 2 * square.getWidth())));
		//parent->left.setBoundsRelative(0 * w, 0.25, w, 0.5);
		//parent->fav.setBoundsRelative(1 * w, 0.25, w, 0.5);
		//parent->right.setBoundsRelative(2 * w, 0.25, w, 0.5);
		parent->titleBarHamburgerMenu.setBounds(expandRectangleToInt(square.withRightX(getWidth() * (1 - 0.25 * w))));


		auto maxW = juce::jmax(parent->right.getRight(), getWidth() - parent->titleBarHamburgerMenu.getX());
		auto b = square.getWidth() * 0.5;
		nameButton->setBounds(getLocalBounds().withX(maxW + b).withRight(getWidth() - maxW - b));

	}

	void PresetManager::TitleBar::paint(juce::Graphics& g)
	{
		g.setColour(parent->universals->colors.getColor(BDSP_COLOR_PURE_BLACK));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), parent->universals->roundedRectangleCurve);
	}









	//================================================================================================================================================================================================



	PresetManager::TitleBar::TitleBarButton::TitleBarButton(TitleBar* p, GUI_Universals* universalsToUse)
		:Button(universalsToUse)
	{
		parent = p;

		onClick = [=]()
		{
			if (parent->parent->browser.isShowing())
			{
				parent->parent->closeBrowser();
				setHintBarText("Opens preset browser");
			}
			else
			{
				parent->parent->openBrowser();
				setHintBarText("Closes preset browser");
			}

		};
		setHintBarText("Opens preset browser");
	}

	void PresetManager::TitleBar::TitleBarButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{


		auto h = getHeight() * 0.9;
		auto rect = juce::Rectangle<float>().leftTopRightBottom(parent->parent->right.getRight(), (getHeight() - h) / 2, getWidth() - parent->parent->right.getRight(), (getHeight() + h) / 2);


		auto font = parent->parent->universals->Fonts[getFontIndex()].getFont().withHeight(h);



		auto text = (parent->parent->Handler->alteredState.getValue().operator bool() ? "*" : "") + parent->parent->Handler->presetName.toString();
		g.setFont(font);

		g.setColour(getHoverColor(getColor(parent->parent->pc.presetText).withAlpha(0.75f), getColor(parent->parent->pc.presetText), shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted, universals->hoverMixAmt));
		drawTextVerticallyCentered(g, font, text, rect, CapHeight, Baseline, true);
	}


} // namespace bdsp
