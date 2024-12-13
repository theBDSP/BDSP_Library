#pragma once

#include "bdsp_ChoiceComponentCore.h"
namespace bdsp
{

	class RangedComponentCore
	{
	public:
		RangedComponentCore()
		{

		}
		virtual ~RangedComponentCore()
		{

		}



		virtual void setActiveControl(int i) = 0;

		virtual ComponentCore* getComponent() = 0;

		virtual bool isLinked() = 0;

		virtual bool isLinkedToControl(int idx) = 0;
		virtual bool isLinkedToAnimatedControl() = 0;
		bool isLinkedToControl(LinkableControlComponent* l)
		{
			return isLinkedToControl(l->getIndex());
		}



	};
	//=====================================================================================================================
	//Ranged Choice Component
	class RangedChoiceComponent : public RangedComponentCore
	{
	public:
		RangedChoiceComponent(ChoiceComponentCore* baseComponent)
			:RangedComponentCore()
		{
			baseComp = baseComponent;
		}
		virtual ~RangedChoiceComponent() = default;



		virtual void link(juce::Colour& c)
		{
		}

		virtual void updateMacroValue(int i, double newValue)
		{
			if (details.isLinked())
			{
				int size = details.getNumberOfRangeIndecies();
				int idx = juce::jlimit(0, size - 1, (int)floor(newValue * size));

				setLinkIndex(details.getRangeIndecies()[idx]);
			}
		}

		virtual void setLinkIndex(int i)
		{
			if (!details.isOverriden())
			{
				baseComp->selectItem(i);
			}
			details.setLinkIndex(i);
		}

		void setActiveControl(int i) override
		{

		}

		virtual void setMacroBypassState(int i, bool newState)
		{
			//if (newState)
			//{
			//	details.setCurrentRange(i);
			//}
			//else if (details.currentRange == i) // only set no active range if one being removed is current (in case current one is removed after new one is added)
			//{
			//	details.setCurrentRange(-1);
			//}
		}

		virtual bool getMacroBypassState(int i)
		{
			return false;
		}

		ComponentCore* getComponent() override
		{
			return dynamic_cast<ComponentCore*>(baseComp);
		}
		bool isLinked() override
		{
			return details.isLinked();
		}

		bool isLinkedToControl(int idx) override
		{
			return details.currentRange == idx;
		}


	protected:

		struct RangeDetails
		{
			void addRange(const juce::String& ID, juce::Array<int>& indecies, juce::Colour& color)
			{
				rangeIDs.add(ID);
				rangeIndexArrays.add(indecies);
				rangeColors.add(color);

				linkIndecies.add(0);
				overrideValues.add(false);
				currentRange = rangeIDs.size() - 1;
			}

			void removeRange(const juce::String& ID)
			{
				auto idx = getIndex(ID);
				rangeIDs.remove(idx);
				rangeIndexArrays.remove(idx);
				rangeColors.remove(idx);
				linkIndecies.remove(idx);
				overrideValues.remove(idx);

				if (currentRange == idx)
				{
					currentRange = -1;
				}
				else if (currentRange > idx)
				{
					currentRange--;
				}
			}

			int getIndex(const juce::String& ID)
			{
				return rangeIDs.indexOf(ID);
			}

			void setCurrentRange(int i)
			{
				currentRange = i;
			}

			bool isLinked()
			{
				return currentRange > -1;
			}

			bool isOverriden()
			{
				bool out = false;
				if (isLinked())
				{
					out = overrideValues[currentRange];
				}
				return out;
			}

			void setLinkIndex(int idx)
			{
				if (isLinked())
				{
					linkIndecies.set(currentRange, idx);
				}
			}

			int getNumberOfRangeIndecies()
			{
				return isLinked() ? getRangeIndecies().size() : -1;
			}

			juce::Array<int> getRangeIndecies()
			{
				juce::Array<int> out;
				if (isLinked())
				{
					out = rangeIndexArrays[currentRange];
				}
				return out;
			}

			juce::Colour getColor()
			{
				return isLinked() ? rangeColors[currentRange] : juce::Colour();
			}

			void setOverridden(bool newState)
			{
				if (isLinked())
				{
					overrideValues.set(currentRange, newState);
				}
			}

			int getLinkedIndex()
			{
				return isLinked() ? linkIndecies[currentRange] : 0;
			}

			juce::Array<bool> overrideValues;
			juce::Array<int> linkIndecies;
			juce::Array<juce::Colour> rangeColors;

			juce::Array<juce::Array<int>> rangeIndexArrays;
			juce::StringArray rangeIDs;

			int currentRange = -1;
		} details;



		ChoiceComponentCore* baseComp;



	};
} // namespace bdsp
