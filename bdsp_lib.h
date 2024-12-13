#pragma once

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 bdsp_lib
  vendor:             BDSP
  version:            1.0.0
  name:               BDSP Library
  description:        Every module needed to make a BDSP Plugin
  website:            http://www.theBDSP.com


  dependencies:		  juce_core,juce_dsp, juce_events, juce_gui_basics, juce_gui_extra,juce_opengl, juce_audio_processors


 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


/** Config: BDSP_DEMO_VERSION
	Toggles between normal and demo version
*/
#ifndef BDSP_DEMO_VERSION
#define BDSP_DEMO_VERSION 0
#endif // !BDSP_DEMO_VERSION


#include "angusj_clipper/angusj_clipper.hpp"
#include "melatonin_blur/melatonin_blur.h"
#include "signalsmith-stretch/signalsmith-stretch.h"

#include "bdsp_core/bdsp_core.h"
#include "bdsp_dsp/bdsp_dsp.h"
#include "bdsp_gui/bdsp_gui.h"
#include "bdsp_preset_manager/bdsp_preset_manager.h"
#include "bdsp_processor_and_editor/bdsp_processor_and_editor.h"