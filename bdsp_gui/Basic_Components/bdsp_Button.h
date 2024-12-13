#pragma once

namespace bdsp
{

	class Button : public juce::Button, public ComponentCore
	{
	public:

		Button(GUI_Universals* universalsToUse)
			:Button(juce::String(), universalsToUse)
		{

		}

		Button(const juce::String& s, GUI_Universals* universalsToUse)
			:juce::Button(s),
			ComponentCore(this, universalsToUse)
		{

		}
		~Button() = default;

		void swapToggleStates(Button* other, juce::NotificationType notificationType = juce::NotificationType::sendNotification)
		{
			bool tmp = other->getToggleState();
			other->setToggleState(getToggleState(), notificationType);
			setToggleState(tmp, notificationType);
		}


		virtual void attach(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
		{
			param = apvts.getParameter(parameterID);
			attachment = std::make_unique<ButtonAttachment>(apvts, parameterID, *this);
		}


		void mouseDown(const juce::MouseEvent& e) override
		{
			if (!allowPopupClick)
			{
				if (!e.mods.isPopupMenu())
				{
					juce::Button::mouseDown(e);
				}
			}
			else
			{
				juce::Button::mouseDown(e);
			}
		}
		void mouseUp(const juce::MouseEvent& e) override
		{
			if (!allowPopupClick)
			{
				if (!e.mods.isPopupMenu())
				{
					juce::Button::mouseUp(e);
				}
			}
			else
			{
				juce::Button::mouseUp(e);
			}
		}

		void setAllowsPopupClick(bool shouldAllowPopupClick)
		{
			allowPopupClick = shouldAllowPopupClick;
		}

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
		{

		}

		juce::RangedAudioParameter* param = nullptr;
	private:
		std::unique_ptr<ButtonAttachment> attachment;
		bool allowPopupClick = true;

	};
} // namespace bdsp