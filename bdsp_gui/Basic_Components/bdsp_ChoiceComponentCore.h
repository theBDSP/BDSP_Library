#pragma once

namespace bdsp
{
	class ChoiceComponentCore
	{
	public:
		ChoiceComponentCore(juce::AudioParameterChoice* param, juce::Component* c)
		{
			parameter = param;

			comp = c;

			internalOnParameterChange = [=](float val)
			{
				parameterChanged(val);

				if (onParameterChanged.operator bool())
				{
					onParameterChanged(val);
				}
			};
		}
		virtual ~ChoiceComponentCore() = default;

		int getIndex()
		{
			return currentIndex;
		}
		void setIndex(int idx)
		{
			currentIndex = idx;
		}

		void selectItem(int idx, bool sendNotifcation = true)
		{
			currentIndex = idx;

			if (attachment != nullptr)
			{
				attachment->setValueAsCompleteGesture(idx);
			}
			else
			{
				internalOnParameterChange(idx);
			}

			if (sendNotifcation)
			{
				updateListeners();
			}
		}
		virtual void scrollItems(bool up)
		{
			auto idx = getIndex();
			auto newIdx = juce::jlimit(0, numItems - 1, up ? --idx : ++idx);


			selectItem(newIdx);

		}

		void reset()
		{
			selectItem(defaultIndex);
		}

		virtual void attach(juce::RangedAudioParameter* param, juce::UndoManager* undo = nullptr)
		{
			auto choice = dynamic_cast<juce::AudioParameterChoice*>(param);
			if (choice != nullptr)
			{
				numItems = choice->choices.size();
			}
			attachment = std::make_unique<juce::ParameterAttachment>(*param, internalOnParameterChange, undo);
			attachment->sendInitialUpdate();
			defaultIndex = param->convertFrom0to1(param->getDefaultValue());
			currentIndex = param->convertFrom0to1(param->getValue());
		}

		virtual void parameterChanged(int idx) = 0; // called after attached parameter changes value

		class Listener
		{
		public:
			Listener()
			{

			}

			virtual ~Listener()
			{

			}

			virtual void choiceComponentValueChanged(ChoiceComponentCore* componentChanged) = 0;

		};

		void addListener(Listener* listenerToAdd)
		{
			listeners.add(listenerToAdd);
		}

		void removeListener(Listener* listenerToRemove)
		{
			listeners.remove(listenerToRemove);
		}

		int getNumItems()
		{
			return numItems;
		}

		juce::AudioParameterChoice* getParameter()
		{
			return parameter;
		}

		std::function<void(float)> onParameterChanged;
	protected:
		juce::AudioParameterChoice* parameter = nullptr;

		std::unique_ptr<juce::ParameterAttachment> attachment;

		std::function<void(float)> internalOnParameterChange;

		juce::ListenerList<Listener> listeners;

		int currentIndex = 0;
		int numItems = 0;

		int defaultIndex = 0;

		juce::Component* comp = nullptr;

		void updateListeners()
		{
			for (auto l : listeners.getListeners())
			{
				l->choiceComponentValueChanged(this);
			}
		}
	};
} // namespace bdsp