#pragma once

#include "juce_core/juce_core.h"



#define PI juce::MathConstants<float>::pi

#ifdef JUCE_WINDOWS
#define BDSP_POPUP_ACTION_TEXT "right click"
#define BDSP_FONT_HEIGHT_SCALING 1
#endif

#ifdef JUCE_MAC
#define BDSP_POPUP_ACTION_TEXT "ctrl-click"
#define BDSP_FONT_HEIGHT_SCALING 0.8
#endif




#include "Containers/Lazy_Load/bdsp_LazyLoad.h"
#include "Math/bdsp_MathFunctions.h"
#include "Flags/bdsp_FlagFunctions.h"
#include "Containers/Circular_Buffer/bdsp_CircularBuffer.h"
#include "Containers/Hash_Functions/bdsp_HashFunctions.h"
#include "Containers/Double_Hashed_Map/bdsp_DoubleHashedMap.h"
#include "Containers/Owned_Map/bdsp_OwnedMap.h"
#include "Text/bdsp_TextFunctions.h"
#include "Text/bdsp_Font.h"
#include "Rectangle/bdsp_RectangleFunctions.h"
#include "Path/bdsp_PathFunctions.h"
#include "Color/bdsp_ColorFunctions.h"
