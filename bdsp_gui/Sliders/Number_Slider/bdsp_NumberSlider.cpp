#include "bdsp_NumberSlider.h"

namespace bdsp
{
	NumberSlider::NumberSlider(GUI_Universals* universalsToUse, const juce::String& baseName)
		:NumberSlider(nullptr, universalsToUse, baseName)
	{
	}

	NumberSlider::NumberSlider(juce::Component* parent, GUI_Universals* universalsToUse, const juce::String& baseName, bool setListen)
        : BaseSlider(universalsToUse, baseName), numbers(this), listen(&numbers)
	{
		setInterceptsMouseClicks(true, true);

		if (parent != nullptr)
		{
			parent->addAndMakeVisible(this);
		}

		addAndMakeVisible(numbers);

		numbers.setEditable(false, false, true);
		//numbers.setAsMainLabel();


		setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

		numbers.addMouseListener(this, false);


		numbers.setBorderSize(juce::BorderSize<int>());

		if (setListen)
		{
			setListener(this);
		}

		numbers.onEditorShow = [this]
		{
			numbers.getCurrentTextEditor()->setPopupMenuEnabled(false);
			numbers.removeMouseListener(this);
		};

		numbers.onEditorHide = [this]
		{
			numbers.addMouseListener(this, false);
			//numbers.setVisible(true);
		};

		onAttach = [=]()
		{
			listen.sliderValueChanged(this);
		};
		addHoverPartner(&numbers);
		setTextBoxIsEditable(true); // gets set to false somewhere else unsure where
	}

	NumberSlider::~NumberSlider()
	{
		numbers.removeMouseListener(this);
		setLookAndFeel(nullptr);

		if (sliderBeingListenedTo != nullptr)
		{
			sliderBeingListenedTo->removeListener(&listen);
		}
	}

	void NumberSlider::showTextEditor()
	{
		numbers.showEditor();
	}





	void NumberSlider::setListener(juce::Slider* sliderToListenTo)
	{
		sliderBeingListenedTo = sliderToListenTo;
		sliderBeingListenedTo->addListener(&listen);
	}

	juce::Slider::Listener* NumberSlider::getListener()
	{
		return &listen;
	}

	void NumberSlider::resized()
	{
		numbers.setBounds(getLocalBounds());
		BaseSlider::resized();
	}

	void NumberSlider::mouseEnter(const juce::MouseEvent& e)
	{
		repaint();
	}

	void NumberSlider::mouseExit(const juce::MouseEvent& e)
	{
		repaint();
	}




	//========================================================================================================================================================================================================================



	NumberSlider::listener::listener(Label* l)
	{
		label = l;
	}

	NumberSlider::listener::~listener()
	{
	}

	void NumberSlider::listener::sliderValueChanged(Slider* slider)
	{
		auto s = dynamic_cast<BaseSlider*> (slider);
		label->setText(s->getTextFromValue(s->getActualValue()), juce::dontSendNotification);
	}

}// namnepace bdsp
