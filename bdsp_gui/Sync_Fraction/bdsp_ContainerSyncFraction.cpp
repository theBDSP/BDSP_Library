#include "bdsp_SyncFraction.h"
#include "bdsp_ContainerSyncFraction.h"

namespace bdsp
{

	bdsp::ContainerSyncFraction::ContainerSyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate)
		:SyncFractionBase(divParam, universalsToUse, BPMComp, baseName, isRate)
	{
		containerFrac = std::make_unique<ContainerSlider>(universalsToUse, baseName + "Fraction");
		containerFrac->addListener(this); // BPMComp Listener which inherets from slider listener
		containerFrac->setCornerCurves(CornerCurves::left);
		fractionSlider = containerFrac.get();
		fraction = containerFrac.get();

		containerTime = std::make_unique<ContainerSlider>(universalsToUse, baseName + "MsTime");
		containerTime->addListener(this); // BPMComp Listener which inherets from slider listener
		containerTime->setCornerCurves(CornerCurves::left);
		msTimeSlider = containerTime.get();
		msTime = containerTime.get();


		divisionCombo = std::make_unique<ComboBox>(divParam, universalsToUse);
		division = divisionCombo.get();

		divisionCombo->setCornerCurves(CornerCurves::right);

		//=====================================================================================================================//=====================================================================================================================//=====================================================================================================================//=====================================================================================================================

		addAndMakeVisible(divisionCombo.get());




		addChildComponent(containerFrac.get());

		addAndMakeVisible(containerTime.get());










		divisionCombo->setHeightRatio(0.25);

		init();

	}


	ContainerSyncFraction::~ContainerSyncFraction()
	{
	}

	void ContainerSyncFraction::resized()
	{
		containerFrac->setBoundsRelative(0, 0, 0.4, 1);
		containerTime->setBounds(containerFrac->getBounds());


		divisionCombo->setBoundsRelative(0.4, 0, 0.6, 1);
	}

	void ContainerSyncFraction::setFracColors(const NamedColorsIdentifier& background, const NamedColorsIdentifier& value, const NamedColorsIdentifier& comboBKGD, const NamedColorsIdentifier& comboText, const NamedColorsIdentifier& outline)
	{
		containerFrac->setSliderColors(background, value, NamedColorsIdentifier(), NamedColorsIdentifier());
		containerTime->setSliderColors(background, value, NamedColorsIdentifier(), NamedColorsIdentifier());

		divisionCombo->setColorSchemeClassic(comboBKGD, NamedColorsIdentifier(), comboText, value);

		outlineColor = outline;
	}



	void ContainerSyncFraction::paintOverChildren(juce::Graphics& g)
	{
		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;


		g.setColour(getColor(outlineColor).withAlpha(alpha));
		g.drawRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve, universals->dividerSize);
	}

	ContainerSlider* ContainerSyncFraction::getFraction()
	{
		return containerFrac.get();
	}

	ContainerSlider* ContainerSyncFraction::getMsSlider()
	{
		return containerTime.get();
	}

	ComboBox* ContainerSyncFraction::getDivision()
	{
		return divisionCombo.get();
	}

}// namnepace bdsp

