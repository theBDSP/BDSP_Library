#pragma once

#include "juce_core/juce_core.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include "juce_opengl/juce_opengl.h"


#ifndef BDSP_NUMBER_OF_MACROS
#define BDSP_NUMBER_OF_MACROS 0
#endif // !BDSP_NUMBER_OF_MACROS

#ifndef BDSP_NUMBER_OF_LFOS
#define BDSP_NUMBER_OF_LFOS 0
#endif // !BDSP_NUMBER_OF_LFOS

#ifndef BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS
#define BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS 0
#endif // !BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS


// Needs to be multiple of 2 otherwise textUp editor centering gets fucked
#ifndef BDSP_MAX_MACRO_NAME_LENGTH
#define BDSP_MAX_MACRO_NAME_LENGTH 16
#endif // !BDSP_MAX_MACRO_NAME_LENGTH


#define BDSP_NUMBER_OF_LINKABLE_CONTROLS (BDSP_NUMBER_OF_MACROS+BDSP_NUMBER_OF_LFOS+BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS)



// For compatibility with AU, change to next integer after any parameter changes
#ifndef BDSP_PARAMETER_VERSION_NUMBER
#define BDSP_PARAMETER_VERSION_NUMBER 1
#endif // !BDSP_PARAMETER_VERSION_NUMBER





#define BDSP_VAL_SIZE 1.0f // ratio of value track to range tracks

#define BDSP_RANGED_GRADIENT_ALPHA 0.5f // alpha at the sart of the gradient for a range

#define BDSP_SLIDER_VALUE_STROKE_RATIO 0.66 // ratio of value stroke size to track stroke size


#define BDSP_CIRCULAR_SLIDER_TRACK_RATIO 0.33
#define BDSP_CIRCULAR_SLIDER_KNOB_RATIO 0.95
#define BDSP_CIRCULAR_SLIDER_TRACK_INSIDE_RATIO 0.5
#define BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO 0.075
#define BDSP_CIRCULAR_SLIDER_THUMB_RATIO 0.15
#define BDSP_CIRCULAR_SLIDER_TRACK_WIDTH 1.0

#define BDSP_LINEAR_SLIDER_POINTER_SIZE 0.05
#define BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT 0.5




#ifndef DBL_MAX
#include <cfloat>
#endif


#ifndef PI

#define PI juce::MathConstants<float>::pi

#endif // !PI

//================================================================================================================================================================================================


namespace bdsp
{


	using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
	using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;




	struct FactoryPreset
	{
		FactoryPreset(juce::String name, const char* d, int size)
		{
			fileName = name;
			data = d;
			dataSize = size;
		}
		juce::String fileName;
		const char* data;
		int dataSize;
	};




	struct PropertiesFolder
	{
		PropertiesFolder();
		juce::File getFolder();
		juce::File getPresetFolder();
	};


	class LookAndFeel;
	class HintBar;
}; //namespace bdsp






namespace bdsp
{

	class LinkableControlComponents;
	class LinkableControl;
	class ControlTransferPopupMenu;
	class DesktopComponentManager;
	class GlContextHolder;

} // namespace bdsp

//=====================================================================================================================
//Path Extensions
#include "Path_Extensions/ConicGradient.h"
#include "Path_Extensions/ExtraShapes.h"
#include "Path_Extensions/GradientAlongPath.h"
#include "Path_Extensions/bdsp_NotePaths.h"
#include "Path_Extensions/bdsp_CommonPaths.h"


//=====================================================================================================================
//Look And Feels
#include "Desktop_Component/bdsp_DestopComponentManager.h"
#include "LookAndFeel/bdsp_ColorScheme.h"
#include "LookAndFeel/bdsp_AnimationTimer.h"
#include "LookAndFeel/bdsp_LookAndFeel.h"
#include "LookAndFeel/bdsp_GUI_Universals.h"
#include "Hint_Bar/bdsp_hintBar.h"



//#include "Alert_Window/bdsp_AlertLNF.h"
//#include "Popup_Menu/bdsp_PopupMenuLNF.h"
//#include "Combo_Box/bdsp_ComboBoxLNF.h"
//
//#include "Sliders/Circle_Slider/bdsp_CircleSliderLNF.h"
//#include "Sliders/Bar_Slider/bdsp_BarSliderLNF.h"
//#include "Sliders/Ranged_Slider/bdsp_RangedSliderLNF.h"
//#include "Sync_Fraction/bdsp_BPMComponentLNF.h"
//
//
//#include "Choice_Slider/bdsp_ChoiceSliderLNF.h"
//
//
//#include "Linkable_Control/Components/Macro_Component/bdsp_MacroComponentLNF.h"
#include "Component/bdsp_Component.h"

#include "Basic_Components/bdsp_AnimatedComponent.h"


#include "Desktop_Component/bdsp_DestopComponent.h"
//#include "LookAndFeel/bdsp_LNFuniversals.h"
//================================================================================================================================================================================================



#include "Basic_Components/bdsp_CaretComponent.h"
#include "Basic_Components/bdsp_Label.h"
#include "Basic_Components/bdsp_TextEditor.h"
#include "Basic_Components/bdsp_Button.h"
#include "Basic_Components/bdsp_Viewport.h"
#include "Basic_Components/bdsp_ChoiceComponentCore.h"
#include "Basic_Components/bdsp_DSPComponent.h"

//================================================================================================================================================================================================
// Parameter Attributes
#include "Parameter_Attributes/bdsp_ParameterAttributes.h"

//================================================================================================================================================================================================
// Parameter
#include "Parameter/bdsp_Parameter.h" 







//================================================================================================================================================================================================
// Pre Rendered Component
#include "Pre_Rendered_Component/bdsp_PreRenderedComponent.h"










//=====================================================================================================================
//Buttons
#include "Buttons/Path_Button/bdsp_PathButton.h"
#include "Buttons/Multi_Shape_Button/bdsp_MulitShapeButton.h"

#include "Buttons/Text_Button/bdsp_TextButton.h"

#include "Buttons/Square_Button/bdsp_SquareButton.h"


#include "Buttons/Tabs_Component/bdsp_TabsComponent.h"

#include "Buttons/Choice_Buttons/bdsp_ChoiceButtons.h"

#include "Buttons/Toggle_Slider/bdsp_ToggleSlider.h"

#include "Buttons/Undo_Redo_Buttons/bdsp_UndoRedoButtons.h"
//=====================================================================================================================
//Alert

#include "Alert_Window/bdsp_AlertWindow.h"



//================================================================================================================================================================================================
// Rearrangeable Components
#include "Rearrangeable_Components/Drag_Handle/bdsp_DragHandle.h"
#include "Rearrangeable_Components/bdsp_RearrangeableComponent.h"

#include "Rearrangeable_Components/bdsp_RearrangeableTabsComponent.h"


//=====================================================================================================================
// Sticky ViewPort
#include "Sticky_Viewport/bdsp_StickyViewport.h"



//================================================================================================================================================================================================
// List Component
#include "List_Component/bdsp_ListComponent.h"



//=====================================================================================================================
//Popup Menu


#include "Popup_Menu/bdsp_PopupMenu.h"

//=====================================================================================================================
// Linkable Control

#include "Linkable_Control/bdsp_LinkableControl.h"
#include "Linkable_Control//bdsp_ControlParameter.h"

#include "Basic_Components/bdsp_RangedComponentCore.h"


//================================================================================================================================================================================================
// OpenGL 
#include "Open_GL_Component/bdsp_OpenGLContextHolder.h"
#include "Open_GL_Component/bdsp_OpenGLComponent.h"
#include "Open_GL_Component/Line/bdsp_LineRenderer.h"
#include "Open_GL_Component/Circle/bdsp_CircleRenderer.h"
#include "Open_GL_Component/Data_Vis/Function_Visulaizer/bdsp_FunctionVisualizer.h"
#include "Open_GL_Component/Data_Vis/bdsp_FFTLineVisualizer.h"
#include "Open_GL_Component/Data_Vis/bdsp_ControlValuesOverTime.h"

#include "Open_GL_Component/Grids/bdsp_GridBase.h"
#include "Open_GL_Component/Grids/bdsp_LineGrid.h"
#include "Open_GL_Component/Grids/bdsp_DotGrid.h"


//=====================================================================================================================
//ComboBox

#include "Combo_Box/bdsp_ComboBox.h"
#include "Combo_Box/bdsp_IncrementalComboBox.h"
#include "Combo_Box/bdsp_AdvancedComboBox.h"
#include "Combo_Box/bdsp_DistortionTypeComboBox.h"



//================================================================================================================================================================================================
//Choice Slider
#include "Choice_Slider/bdsp_ChoiceSlider.h"


//=====================================================================================================================
//Logo
#include "Logo/bdsp_Logo.h"

//=====================================================================================================================
//Sliders

#include "Sliders/Slider_Popup_Menu/bdsp_SliderPopupMenu.h"

#include "Sliders/Base_Slider/bdsp_BaseSlider.h"

#include "Sliders/Circle_Slider/bdsp_CircleSlider.h"
#include "Sliders/Circle_Slider/SideLabel/bdsp_CircleSliderSideLabel.h"
#include "Sliders/Ranged_Slider/bdsp_RangedSliderInfluence.h"
#include "Sliders/Ranged_Slider/bdsp_RangedSlider.h"



#include "Sliders/Bar_Slider/bdsp_BarSlider.h"
#include "Sliders/Bar_Slider/bdsp_RangedBarSlider.h"

#include "Sliders/Circle_Slider/bdsp_RangedCircleSlider.h"
#include "Sliders/Circle_Slider/SideLabel/bdsp_RangedCircleSliderSideLabel.h"


#include "Sliders/Number_Slider/bdsp_NumberSlider.h"
#include "Sliders/Number_Slider/bdsp_RangedNumberSlider.h"


#include "Sliders/Container_Slider/bdsp_ContainerSlider.h"
#include "Sliders/Container_Slider/bdsp_RangedContainerSlider.h"



//=====================================================================================================================
//Sync Fraction

#include "Sync_Fraction/bdsp_BPMComponent.h"
#include "Sync_Fraction/bdsp_SyncFraction.h"
#include "Sync_Fraction/bdsp_RangedSyncFraction.h"
#include "Sync_Fraction/bdsp_ContainerSyncFraction.h"
#include "Sync_Fraction/bdsp_RangedContainerSyncFraction.h"




//=====================================================================================================================
// Control Components

#include "Linkable_Control/Components/General_Control_Component/bdsp_GeneralControlComponent.h"
#include "Linkable_Control/Components/Macro_Component/bdsp_MacroComponent.h"
#include "Linkable_Control/Components/LFO_Component/bdsp_LFOComponent.h"
#include "Linkable_Control/Components/Envelope_Follower_Component/bdsp_envelopeFollowerComponent.h"

#include "Linkable_Control/Components/All_Linkable_Control_Components/All_Linkable_Control_Components.h"


//================================================================================================================================================================================================
//Color Scheme Editor
#include "Color_Scheme_Editor/Color_Selector/bdsp_ColorSelector.h"
#include "Color_Scheme_Editor/bdsp_ColorSchemeEditor.h"

//=====================================================================================================================
//Settings Menu
#include "Settings_Menu/bdsp_SettingsMenu.h"

//================================================================================================================================================================================================
//DSP Visualizers
#include "DSP_Visualizers/Distortion/bdsp_DistortionVisualizer.h"
#include "DSP_Visualizers/Filter/bdsp_FilterVisualizer.h"
#include "DSP_Visualizers/Filter/bdsp_EQVisualizer.h"
#include "DSP_Visualizers/Level_Meter/bdsp_LevelMeter.h"
#include "DSP_Visualizers/Demo_Alert/bdsp_DemoAlert.h"
#include "DSP_Visualizers/LFO/bdsp_LFOVisualizer.h"
#include "DSP_Visualizers/Envelope_Follower/bdsp_EnvelopeFollowerVisualizer.h"
#include "DSP_Visualizers/Bit_Crush/bdsp_BitCrushVisualizer.h"
#include "DSP_Visualizers/Pitch_Shifter/bdsp_PitchShifterVisualizer.h"


#include "DSP_Visualizers/Modulation/Phaser/bdsp_PhaserVisualizer.h"
#include "DSP_Visualizers/Modulation/Chorus/bdsp_ChorusVisualizer.h"
#include "DSP_Visualizers/Modulation/Flanger/bdsp_FlangerVisualizer.h"
#include "DSP_Visualizers/Modulation/Ring_Mod/bdsp_RingModVisualizer.h"

#include "DSP_Visualizers/Dynamics/bdsp_CompressorVisualizer.h"

#include "DSP_Visualizers/Generator/bdsp_NoiseVisualizer.h"

//================================================================================================================================================================================================
//Signal Path Manipulator (gotta be a better name for this)
#include "Signal_Path_Manipulator/bdsp_SignalPathManipulator.h"


