#include "bdsp_SyncFraction.h"
#include "bdsp_ContainerSyncFraction.h"

namespace bdsp
{
	RangedContainerSyncFraction::RangedContainerSyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate)
		:SyncFractionBase(divParam, universalsToUse, BPMComp, baseName, isRate)
	{
		containerFrac = std::make_unique<RangedContainerSlider>(universalsToUse, baseName + "Fraction");
		containerFrac->slider.setCornerCurves(CornerCurves(CornerCurves::topLeft | CornerCurves::bottomLeft));
		containerFrac->slider.addListener(this); // BPMComp Listener which inherets from slider listener

		fractionSlider = &containerFrac->slider;

		fraction = containerFrac.get();

		containerTime = std::make_unique<RangedContainerSlider>(universalsToUse, baseName + "MsTime");
		containerTime->slider.setCornerCurves(CornerCurves(CornerCurves::topLeft | CornerCurves::bottomLeft));
		containerTime->slider.addListener(this); // BPMComp Listener which inherets from slider listener
		msTimeSlider = &containerTime->slider;
		msTime = containerTime.get();

		divisionCombo = std::make_unique<ComboBox>(divParam, universalsToUse);
		division = divisionCombo.get();
		divisionCombo->setCornerCurves(CornerCurves(0));
		//=====================================================================================================================//=====================================================================================================================//=====================================================================================================================//=====================================================================================================================


		addAndMakeVisible(divisionCombo.get());




		addChildComponent(containerFrac.get());

		addAndMakeVisible(containerTime.get());










		divisionCombo->setHeightRatio(0.25);

		init();

	}

	RangedContainerSyncFraction::~RangedContainerSyncFraction()
	{
	}

	void RangedContainerSyncFraction::attach(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterIDBase)
	{
		SyncFractionBase::attach(stateToUse, parameterIDBase);


		if (containerFrac->getControlParamter()->getModulationSnapParameter() != nullptr)
		{
			lockButton = std::make_unique<PathButton>(universals, true);
			lockButton->setPath(universals->commonPaths.locked, universals->commonPaths.unlocked);

			addAndMakeVisible(lockButton.get());
			lockButton->attach(stateToUse, parameterIDBase + "FractionSnapID");


			auto prevChange = divisionCombo->onChange;

			divisionCombo->onChange = [=](int idx)
			{
				if (prevChange.operator bool())
				{
					prevChange(idx);
				}
				lockButton->setEnabled(idx > 0);

			};




		}
	}


	void RangedContainerSyncFraction::resized()
	{
		containerFrac->setBoundsRelative(0, 0, 0.425, 1);
		containerTime->setBounds(containerFrac->getBounds());

		if (lockButton == nullptr)
		{
			divisionCombo->setBoundsRelative(0.425, 0, 0.575, 1);
		}
		else
		{
			divisionCombo->setBoundsRelative(0.4, 0, 0.425, 1);

			auto lockW = getWidth() * 0.15;

			lockButton->setPathBorder(universals->rectThicc);
			lockButton->setBounds(juce::Rectangle<int>(lockW, lockW).withCentre(juce::Point<int>((divisionCombo->getRight() + getWidth()) / 2, getHeight() / 2.0)));
		}


	}

	void RangedContainerSyncFraction::setFracColors(const NamedColorsIdentifier& background, const NamedColorsIdentifier& value, const NamedColorsIdentifier& comboBKGD, const NamedColorsIdentifier& comboText, const NamedColorsIdentifier& outline)
	{
		containerFrac->slider.setSliderColors(background, value, NamedColorsIdentifier(), NamedColorsIdentifier());
		containerTime->slider.setSliderColors(background, value, NamedColorsIdentifier(), NamedColorsIdentifier());

		divisionCombo->setColorSchemeClassic(comboBKGD, outline, comboText, value.withMultipliedAlpha(universals->lowOpacity));

		outlineColor = outline;
		if (lockButton != nullptr)
		{
			lockButton->setColor(comboText, comboText);
			lockBKGD = comboBKGD;
		}
	}

	void RangedContainerSyncFraction::paint(juce::Graphics& g)
	{
		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;
		if (lockButton != nullptr)
		{
			g.setColour(getColor(lockBKGD).withAlpha(alpha));

			juce::Path p;

			p.addRoundedRectangle(divisionCombo->getRight(), 0, getWidth() - divisionCombo->getRight(), getHeight(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, false, corners & CornerCurves::topRight, false, corners & CornerCurves::bottomRight);

			g.fillPath(p);

		}

	}

	void RangedContainerSyncFraction::paintOverChildren(juce::Graphics& g)
	{
		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;


		g.setColour(getColor(outlineColor).withAlpha(alpha));
		juce::Path p;

		p.addRoundedRectangle(divisionCombo->getRight(), 0, getWidth() - divisionCombo->getRight(), getHeight(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, false, corners & CornerCurves::topRight, false, corners & CornerCurves::bottomRight);
		g.strokePath(p, juce::PathStrokeType(universals->dividerSize));

		g.drawRect(divisionCombo->getBounds(), universals->dividerSize);

	}


	RangedContainerSlider* RangedContainerSyncFraction::getFraction()
	{
		return containerFrac.get();
	}

	RangedContainerSlider* RangedContainerSyncFraction::getMsSlider()
	{
		return containerTime.get();
	}

	ComboBox* RangedContainerSyncFraction::getDivision()
	{
		return divisionCombo.get();
	}

	PathButton* RangedContainerSyncFraction::getLockButton()
	{
		return lockButton.get();
	}

	void RangedContainerSyncFraction::setCorners(CornerCurves newCurves)
	{
		containerFrac->slider.setCornerCurves(CornerCurves(newCurves & CornerCurves::topLeft | newCurves & CornerCurves::bottomLeft));
		containerTime->slider.setCornerCurves(CornerCurves(newCurves & CornerCurves::topLeft | newCurves & CornerCurves::bottomLeft));

		corners = CornerCurves(newCurves & CornerCurves::topRight | newCurves & CornerCurves::bottomRight);
	}

}// namnepace bdsp

