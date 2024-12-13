#include "bdsp_DestopComponentManager.h"

namespace bdsp
{
	DesktopComponentManager::DesktopComponentManager()
	{
		setInterceptsMouseClicks(false, true);
	}

	DesktopComponentManager::~DesktopComponentManager()
	{
	}

	void DesktopComponentManager::addComponent(DesktopComponent* componentToAdd)
	{
		addChildComponent(componentToAdd);
		componentsToManage.add(componentToAdd);
		componentToAdd->manager = this;
	}
	void DesktopComponentManager::removeComponent(DesktopComponent* componentToRemove)
	{
		removeChildComponent(componentToRemove);
		componentsToManage.removeAllInstancesOf(componentToRemove);
	}
	void DesktopComponentManager::hideAllComponents(bool excludeCompsThatDisreagardLossOfFocus)
	{

		if (onHide.operator bool())
		{
			onHide();
		}
		if (excludeCompsThatDisreagardLossOfFocus)
		{

			for (auto d : componentsToManage)
			{
				if (d->isVisible() && d->lossOfFocusClosesWindow())
				{
					d->hide();
				}
			}
		}
		else
		{
			for (auto d : componentsToManage)
			{
				if (d->isVisible())
				{
					d->hide();
				}
			}

		}
	}


	void DesktopComponentManager::hideAllComponentsExcept(const juce::Array<DesktopComponent*>& componentsToExclude)
	{
		if (onHide.operator bool())
		{
			onHide();
		}
		for (auto d : componentsToManage)
		{
			if (!componentsToExclude.contains(d))
			{
				d->hide();
			}
		}
	}

	void DesktopComponentManager::paintOverChildren(juce::Graphics& g)
	{
		if (paintOverGUIFunction.operator bool())
		{
			paintOverGUIFunction(g);
		}
	}




	//========================================================================================================================================================================================================================





}// namnepace bdsp
