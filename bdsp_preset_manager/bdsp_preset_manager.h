#pragma once



/** Config: BDSP_MAX_PRESET_NAME_LENGTH
	Max characters allowed for preset names
*/
#ifndef BDSP_MAX_PRESET_NAME_LENGTH
#define BDSP_MAX_PRESET_NAME_LENGTH 30
#endif // !BDSP_MAX_PRESET_NAME_LENGTH

/** Config: BDSP_MAX_TAG_NAME_LENGTH
	Max characters allowed for tag names
*/
#ifndef BDSP_MAX_TAG_NAME_LENGTH
#define BDSP_MAX_TAG_NAME_LENGTH 16
#endif // !BDSP_MAX_TAG_NAME_LENGTH

#define BDSP_PRESET_NAME_ALLOWED_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()~`':<>%$!+=_|?[]{}^#& "



namespace bdsp
{

	/*
	Preset XML Structure

	<Main>

		<Info Name="AppName" Version="0.0.0"/>

		<Fav FavoriteAttributeName = "0"/>

		<SuperTags>
			<Type "TagName1"/>
			<SuperTag2 "TagName2"/>
		<SuperTags/>

		<Tags>
			<TagName1 = 1/>
			<TagName3 = 1/>
			.
			.
			.
		<Tags/>

		<APVTS>
		.
		.
		.
		.
		<APVTS/>

	</Main>

	*/




#define FavoriteXMLTag "Favorite"
#define FavoriteAttributeName "Fav"

#define TagsXMLTag "Tags"
#define SuperTagsXMLTag "SuperTags"
#define InfoXMLTag "Info"

#define MacroNamesXMLTag "MacroNames"



#define isFav(f) (juce::parseXML(f)!=nullptr)? juce::parseXML(f)->getChildByName(FavoriteXMLTag)->getBoolAttribute(FavoriteAttributeName) : false



	enum PresetAction { close, stay, newPreset };

	enum PresetItemActions { Delete, Rename, OSDirectory, Refresh, Overwrite, SetAsDefault, ReInstall };

}// namnepace bdsp


//

#include "State_Handler/bdsp_StateHandler.h"

#include "Manager/bdsp_ManagerLNF.h"

#include "Menu_Item/bdsp_MenuItem.h"
#include "Menu/bdsp_Menu.h"
#include "Tag_Menu/bdsp_TagMenu.h"
#include "Save_Menu/bdsp_SaveMenu.h"
#include "Manager/bdsp_Manager.h"


