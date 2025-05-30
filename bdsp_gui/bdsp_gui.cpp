#include "bdsp_gui.h"

namespace bdsp
{



	PropertiesFolder::PropertiesFolder()
	{


		if (!getFolder().exists() || !getPresetFolder().exists())
		{
			getFolder().getParentDirectory().getParentDirectory().createDirectory();
			getFolder().getParentDirectory().createDirectory();
			getFolder().createDirectory();
			getPresetFolder().createDirectory();

		}
		getFolder().getParentDirectory().setReadOnly(false, true);

	}


	juce::File PropertiesFolder::getFolder()
	{
#if JUCE_WINDOWS
		return juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory).getChildFile("BDSP").getChildFile(BDSP_APP_NAME);
#elif JUCE_MAC
		return juce::File("~//Music//Audio Music Apps//BDSP").getChildFile(BDSP_APP_NAME);
#endif
	}

	juce::File PropertiesFolder::getPresetFolder()
	{
		return getFolder().getChildFile("Presets");
	}









}// namnepace bdsp

#include "LookAndFeel/bdsp_AnimationTimer.cpp"
#include "LookAndFeel/bdsp_LookAndFeel.cpp"
#include "Component/bdsp_Component.cpp"

#include "Desktop_Component/bdsp_DestopComponent.cpp"
#include "Desktop_Component/bdsp_DestopComponentManager.cpp"

#include "Basic_Components/bdsp_DSPComponent.cpp"

//================================================================================================================================================================================================
// Parameter Attributes
#include "Parameter_Attributes/bdsp_ParameterAttributes.cpp"
#include "Ordered_List_Parameter/bdsp_OrderedListParameter.cpp"

//================================================================================================================================================================================================
// Parameter
#include "Parameter/bdsp_Parameter.cpp" 


//=====================================================================================================================
//Path Extensions
#include "Path_Extensions/ConicGradient.cpp"
#include "Path_Extensions/ExtraShapes.cpp"
#include "Path_Extensions/GradientAlongPath.cpp"

//================================================================================================================================================================================================
// Pre Rendered Component
#include "Pre_Rendered_Component/bdsp_PreRenderedComponent.cpp"

//=====================================================================================================================
//Buttons
#include "Buttons/Path_Button/bdsp_PathButton.cpp"

#include "Buttons/Multi_Shape_Button/bdsp_MulitShapeButton.cpp"

#include "Buttons/Text_Button/bdsp_TextButton.cpp"

#include "Buttons/Square_Button/bdsp_SquareButton.cpp"


#include "Buttons/Tabs_Component/bdsp_TabsComponent.cpp"

#include "Buttons/Choice_Buttons/bdsp_ChoiceButtons.cpp"

#include "Buttons/Toggle_Slider/bdsp_ToggleSlider.cpp"

#include "Buttons/Undo_Redo_Buttons/bdsp_UndoRedoButtons.cpp"

//=====================================================================================================================
//Alert
#include "Alert_Window/bdsp_AlertWindow.cpp"


//================================================================================================================================================================================================
// Rearrangeable Components
#include "Rearrangeable_Components/Drag_Handle/bdsp_DragHandle.cpp"
#include "Rearrangeable_Components/bdsp_RearrangeableComponent.cpp"
#include "Rearrangeable_Components/bdsp_RearrangeableTabsComponent.cpp"


//=====================================================================================================================
// Sticky ViewPort
#include "Sticky_Viewport/bdsp_StickyViewport.cpp"

//================================================================================================================================================================================================
// List Component
#include "List_Component/bdsp_ListComponent.cpp"

//=====================================================================================================================
//Popup Menu
#include "Popup_Menu/bdsp_PopupMenu.cpp"

//=====================================================================================================================
// Linkable Control

#include "Linkable_Control/bdsp_LinkableControl.cpp"
#include "Linkable_Control//bdsp_ControlParameter.cpp"


//================================================================================================================================================================================================
// Open GL
#include "Open_GL_Component/bdsp_OpenGLContextHolder.cpp"
#include "Open_GL_Component/bdsp_OpenGLComponent.cpp"
#include "Open_GL_Component/Line/bdsp_LineRenderer.cpp"
#include "Open_GL_Component/Circle/bdsp_CircleRenderer.cpp"
#include "Open_GL_Component/Circle/Ring/bdsp_OpenGLRing.cpp"

#include "Open_GL_Component/Data_Vis/Function_Visulaizer/bdsp_FunctionVisualizer.cpp"


#include "Open_GL_Component/Grids/bdsp_GridBase.cpp"



//=====================================================================================================================
//ComboBox
#include "Combo_Box/bdsp_ComboBox.cpp"
#include "Combo_Box/bdsp_AdvancedComboBox.cpp"
//#include "Combo_Box/bdsp_IncrementalComboBox.cpp"
#include "Combo_Box/bdsp_DistortionTypeComboBox.cpp"



//================================================================================================================================================================================================
//Choice Slider
#include "Choice_Slider/bdsp_ChoiceSlider.cpp"




//=====================================================================================================================
//Logo
#include "Logo/bdsp_Logo.cpp"

//=====================================================================================================================
//Sliders

#include "Sliders/Slider_Popup_Menu/bdsp_SliderPopupMenu.cpp"

#include "Sliders/Base_Slider/bdsp_BaseSlider.cpp"

#include "Sliders/Ranged_Slider/bdsp_RangedSliderInfluence.cpp"
#include "Sliders/Ranged_Slider/bdsp_RangedSlider.cpp"


#include "Sliders/Bar_Slider/bdsp_BarSlider.cpp"


#include "Sliders/Bar_Slider/bdsp_RangedBarSlider.cpp"




#include "Sliders/Circle_Slider/bdsp_CircleSlider.cpp"
#include "Sliders/Circle_Slider/bdsp_RangedCircleSlider.cpp"

#include "Sliders/Circle_Slider/SideLabel/bdsp_CircleSliderSideLabel.cpp"
#include "Sliders/Circle_Slider/SideLabel/bdsp_RangedCircleSliderSideLabel.cpp"





#include "Sliders/Number_Slider/bdsp_NumberSlider.cpp"
#include "Sliders/Number_Slider/bdsp_RangedNumberSlider.cpp"


#include "Sliders/Container_Slider/bdsp_ContainerSlider.cpp"
#include "Sliders/Container_Slider/bdsp_RangedContainerSlider.cpp"


//=====================================================================================================================
//Sync Fraction

#include "Sync_Fraction/bdsp_BPMComponent.cpp"
#include "Sync_Fraction/bdsp_SyncFraction.cpp"
#include "Sync_Fraction/bdsp_RangedSyncFraction.cpp"
#include "Sync_Fraction/bdsp_ContainerSyncFraction.cpp"
#include "Sync_Fraction/bdsp_RangedContainerSyncFraction.cpp"



//=====================================================================================================================
// Control Components
#include "Linkable_Control/Components/General_Control_Component/bdsp_GeneralControlComponent.cpp"
#include "Linkable_Control/Components/Macro_Component/bdsp_MacroComponent.cpp"
#include "Linkable_Control/Components/LFO_Component/bdsp_LFOComponent.cpp"
#include "Linkable_Control/Components/Envelope_Follower_Component/bdsp_EnvelopeFollowerComponent.cpp"
#include "Linkable_Control/Components/Sequencer_Component/bdsp_SequencerComponent.cpp"

#include "Linkable_Control/Components/All_Linkable_Control_Components/All_Linkable_Control_Components.cpp"

//================================================================================================================================================================================================
//Color Scheme Editor
#include "Color_Scheme_Editor/Color_Selector/bdsp_ColorSelector.cpp"
#include "Color_Scheme_Editor/bdsp_ColorSchemeEditor.cpp"

//=====================================================================================================================
//Settings Menu

#include "Settings_Menu/bdsp_SettingsMenu.cpp"

//================================================================================================================================================================================================
//DSP Visualizers
#include "DSP_Visualizers/Distortion/bdsp_DistortionVisualizer.cpp"
#include "DSP_Visualizers/Filter/bdsp_FilterVisualizer.cpp"
#include "DSP_Visualizers/Filter/bdsp_EQVisualizer.cpp"
#include "DSP_Visualizers/Level_Meter/bdsp_LevelMeter.cpp"
#include "DSP_Visualizers/Demo_Alert/bdsp_DemoAlert.cpp"
#include "DSP_Visualizers/LFO/bdsp_LFOVisualizer.cpp"
#include "DSP_Visualizers/Envelope_Follower/bdsp_EnvelopeFollowerVisualizer.cpp"
#include "DSP_Visualizers/Sequencer/bdsp_SequencerVisualizer.cpp"


#include "DSP_Visualizers/Bit_Crush/bdsp_BitCrushVisualizer.cpp"
#include "DSP_Visualizers/Pitch_Shifter/bdsp_PitchShifterVisualizer.cpp"

#include "DSP_Visualizers/Modulation/Phaser/bdsp_PhaserVisualizer.cpp"
#include "DSP_Visualizers/Modulation/Chorus/bdsp_ChorusVisualizer.cpp"
#include "DSP_Visualizers/Modulation/Flanger/bdsp_FlangerVisualizer.cpp"
#include "DSP_Visualizers/Modulation/Ring_Mod/bdsp_RingModVisualizer.cpp"

#include "DSP_Visualizers/Dynamics/bdsp_CompressorVisualizer.cpp"
#include "DSP_Visualizers/Generator/bdsp_NoiseVisualizer.cpp"

//================================================================================================================================================================================================
//Signal Path Manipulator (gotta be a better name for this)
#include "Signal_Path_Manipulator/bdsp_SignalPathManipulator.cpp"


