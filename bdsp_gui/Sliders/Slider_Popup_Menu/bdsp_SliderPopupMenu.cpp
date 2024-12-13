#include "bdsp_SliderPopupMenu.h"
namespace bdsp
{

	SliderPopupMenu::SliderPopupMenu(BaseSlider* p, GUI_Universals* universalsToUse)
		:PopupMenu(p, universalsToUse)
	{
		List.setColorSchemeClassic(p->knobColor, NamedColorsIdentifier(), NamedColorsIdentifier(BDSP_COLOR_BLACK), p->valueColor.withMultipliedAlpha(universals->lowOpacity));



		List.addItem("Copy Value", 0);
		List.addItem("Paste Value", 1);



		List.onSelect = [=](int i)
		{
			switch (i)
			{
			case 0: //Copy Value
				universals->sliderClipboard.normalizedValue = parent->getNormalisableRange().convertTo0to1(parent->getValue());
				break;

			case 1: // Paste Value
				parent->setValue(parent->getNormalisableRange().convertFrom0to1(universals->sliderClipboard.normalizedValue));
				break;

			case 2: //Copy Modulation
				for (int j = 0; j < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++j)
				{
					universals->sliderClipboard.modulationAmounts.set(j, rangedParent->influences[j]->getValue()); // should never get called on null ranged parent
				}
				break;


			case 3: // Paste Modulation
				for (int j = 0; j < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++j)
				{
					if (universals->sliderClipboard.modulationAmounts[j] != 0 && !rangedParent->influences[j]->isLinked())
					{
						rangedParent->influences[j]->createLink();
					}
					else if (universals->sliderClipboard.modulationAmounts[j] == 0 && rangedParent->influences[j]->isLinked())
					{
						rangedParent->influences[j]->removeLink();
					}

					rangedParent->influences[j]->setValue(universals->sliderClipboard.modulationAmounts[j]);
				}
				break;

			case 4: // Clear Modulation
				for (int j = 0; j < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++j)
				{
					rangedParent->influences[j]->setValue(0);
				}
				break;
			}

			hide();
		};

		onShow = [=]()
		{
			bool valueState = universals->sliderClipboard.normalizedValue >= 0;
			List.List[1]->setEnabled(valueState);


			//================================================================================================================================================================================================
			if (isRanged)
			{
				bool modState = !universals->sliderClipboard.modulationAmounts.isEmpty();
				List.List[3]->setEnabled(modState);

				bool hasModulation = !rangedParent->hoverMenu->isEmpty();
				List.List[2]->setEnabled(hasModulation);
				List.List[4]->setEnabled(hasModulation);
			}
		};


		parent = p;
	}
	void SliderPopupMenu::setRanged(RangedSlider* rangedP)
	{
		isRanged = true;
		rangedParent = rangedP;

		List.clearItems();

		List.addItem("Copy Value", 0);
		List.addItem("Paste Value", 1);

		if (isRanged)
		{
			List.addItem("Copy Mods", 2);
			List.addItem("Paste Mods", 3);
			List.addItem("Clear All Mods", 4);
		}

	}
	void SliderPopupMenu::resized()
	{
		if (List.getNum() != 0)
		{
			vp.setBoundsRelative(0, 0, 1, 1);
			List.setBounds(0, 0, vp.getWidth(), vp.getHeight());
			List.resized();
		}
	}
} // namespace bdsp
