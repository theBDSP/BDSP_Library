//#include "bdsp_ColorSelector.h"
//namespace bdsp
//{
//	juce::Colour ColorSpaceChangeBroadcaster::getHSBColor()
//	{
//		return juce::Colour(hue, sat, bri, 1.0f);
//	}
//	float ColorSpaceChangeBroadcaster::getHue()
//	{
//		return hue;
//	}
//	float ColorSpaceChangeBroadcaster::getSat()
//	{
//		return sat;
//	}
//	float ColorSpaceChangeBroadcaster::getBri()
//	{
//		return bri;
//	}
//
//
//	//================================================================================================================================================================================================
//
//
//	ColorSpaceViewer::ColorSpaceViewer(GUI_Universals* universalsToUse)
//		:Component(universalsToUse)
//	{
//		//setBufferedToImage(true);
//		;
//		crossHair = std::make_unique<CrossHair>(this);
//		addAndMakeVisible(crossHair.get());
//	}
//
//	void ColorSpaceViewer::setColor(float h, float s, float b)
//	{
//		hue = h;
//		sat = s;
//		bri = b;
//
//
//		repaint();
//		sendChangeMessage();
//	}
//
//	void ColorSpaceViewer::resized()
//	{
//		forceAspectRatio(1);
//		int s = 2 * universals->roundedRectangleCurve;
//		crossHair->setSize(s, s);
//		crossHair->update(getHSBColor(), sat, 1 - bri);
//	}
//	void ColorSpaceViewer::paint(juce::Graphics& g)
//	{
//		g.saveState();
//		juce::Path clip;
//		clip.addRoundedRectangle(getLocalBounds(), universals->roundedRectangleCurve);
//		g.reduceClipRegion(clip);
//
//		juce::Image space(juce::Image::RGB, getWidth(), getHeight(), true);
//
//
//		juce::Image::BitmapData pixels(space, juce::Image::BitmapData::writeOnly);
//
//		for (int x = 0; x < getWidth(); x++)
//		{
//			float s = x / (getWidth() - 1.0f);
//			for (int y = 0; y < getHeight(); y++)
//			{
//				float b = 1.0f - y / (getHeight() - 1.0f);
//				pixels.setPixelColour(x, y, juce::Colour(hue, s, b, 1.0f));
//			}
//		}
//		g.drawImageAt(space, 0, 0);
//
//		g.restoreState();
//
//		g.setColour(juce::Colours::black);
//		g.strokePath(clip, juce::PathStrokeType(1));
//	}
//	void ColorSpaceViewer::mouseDown(const juce::MouseEvent& e)
//	{
//		auto relX = juce::jlimit(0.0f, 1.0f, e.x / (float)getWidth());
//		auto relY = juce::jlimit(0.0f, 1.0f, e.y / (float)getHeight());
//		setColor(hue, relX, 1 - relY);
//	}
//	void ColorSpaceViewer::mouseDrag(const juce::MouseEvent& e)
//	{
//		mouseDown(e);
//	}
//	void ColorSpaceViewer::changeListenerCallback(juce::ChangeBroadcaster* source)
//	{
//		auto cast = dynamic_cast<ColorSpaceChangeBroadcaster*>(source);
//		setColor(cast->getHue(), cast->getSat(), cast->getBri());
//	}
//
//
//	//================================================================================================================================================================================================
//
//
//	ColorSpaceViewer::CrossHair::CrossHair(ColorSpaceViewer* parent)
//		:Component(parent->universals)
//	{
//		p = parent;
//		p->addChangeListener(this);
//	}
//
//	void ColorSpaceViewer::CrossHair::update(const juce::Colour& c, float x, float y)
//	{
//		color = c.contrasting();
//		auto xLimited = juce::jlimit(getWidth() / 2.0, p->getWidth() - getWidth() / 2.0, (double)p->getWidth() * x);
//		auto yLimited = juce::jlimit(getHeight() / 2.0, p->getHeight() - getHeight() / 2.0, (double)p->getHeight() * y);
//		setCentrePosition(xLimited, yLimited);
//	}
//
//	void ColorSpaceViewer::CrossHair::paint(juce::Graphics& g)
//	{
//		g.setColour(color);
//		g.drawEllipse(getLocalBounds().toFloat().reduced(1), 1);
//	}
//
//	void ColorSpaceViewer::CrossHair::changeListenerCallback(ChangeBroadcaster* source)
//	{
//		update(p->getHSBColor(), p->getSat(), 1.0f - p->getBri());
//
//	}
//
//
//	//================================================================================================================================================================================================
//
//
//	HueSlider::HueSlider(ColorSpaceViewer* viewer)
//		:Component(viewer->universals)
//	{
//		linkedViewer = viewer;
//		slider.setSliderStyle(juce::Slider::LinearBarVertical);
//		slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
//
//		slider.onValueChange = [=]()
//		{
//			linkedViewer->setColor(slider.getValue(), linkedViewer->getSat(), linkedViewer->getBri());
//			repaint();
//		};
//
//
//
//		slider.setRange(0, 1);
//		addAndMakeVisible(slider);
//		linkedViewer->addChangeListener(this);
//	}
//
//
//
//	void HueSlider::paint(juce::Graphics& g)
//	{
//		auto reduced = slider.getBounds().toFloat();
//		juce::ColourGradient cg(juce::Colour::fromHSV(0, 1.0, 1.0, 1.0), reduced.getBottomLeft(), juce::Colour::fromHSV(1, 1.0, 1.0, 1.0), reduced.getTopLeft(), false);
//		cg.addColour(0.25, juce::Colour::fromHSV(0.25, 1.0, 1.0, 1.0));
//		cg.addColour(0.5, juce::Colour::fromHSV(0.5, 1.0, 1.0, 1.0));
//		cg.addColour(0.75, juce::Colour::fromHSV(0.75, 1.0, 1.0, 1.0));
//
//		g.setGradientFill(cg);
//		g.fillRoundedRectangle(reduced, universals->roundedRectangleCurve);
//		g.setColour(juce::Colours::black);
//		g.drawRoundedRectangle(reduced, universals->roundedRectangleCurve, 1);
//
//		auto s = juce::jmin(reduced.getX(), reduced.getY());
//		auto y = reduced.getBottom() - slider.getNormalisableRange().convertTo0to1(slider.getValue()) * reduced.getHeight();
//		g.setColour(juce::Colours::black);
//		g.fillPath(createEquilateralTriangle(juce::Rectangle<float>(s, s).withCentre(juce::Point<float>(reduced.getX() - s / 2, y)), PI / 2));
//		g.fillPath(createEquilateralTriangle(juce::Rectangle<float>(s, s).withCentre(juce::Point<float>(reduced.getRight() + s / 2, y)), -PI / 2));
//	}
//
//	void HueSlider::resized()
//	{
//		slider.setBounds(getWidth() * 0.1, linkedViewer->getY(), getWidth() * 0.8, linkedViewer->getHeight());
//	}
//
//	juce::Slider* HueSlider::getSlider()
//	{
//		return &slider;
//	}
//
//	void HueSlider::changeListenerCallback(juce::ChangeBroadcaster* source)
//	{
//		slider.setValue(linkedViewer->getHue(), juce::dontSendNotification);
//		repaint();
//	}
//
//
//
//
//	//================================================================================================================================================================================================
//
//
//	ColorValueSlider::ColorValueSlider(ColorSpaceViewer* viewer, colorChannel type)
//		:Component(viewer->universals),
//		slider(viewer->universals)
//	{
//		linkedViewer = viewer;
//		linkedViewer->addChangeListener(this);
//
//		channel = type;
//		switch (channel)
//		{
//		case bdsp::Hue:
//			setName("H");
//			slider.setNormalisableRange(juce::NormalisableRange<double>(0, 360));
//			break;
//		case bdsp::Saturation:
//			setName("S");
//			slider.setNormalisableRange(juce::NormalisableRange<double>(0, 100));
//			break;
//		case bdsp::Brightness:
//			setName("V");
//			slider.setNormalisableRange(juce::NormalisableRange<double>(0, 100));
//			break;
//		case bdsp::Red:
//			setName("R");
//			slider.setNormalisableRange(juce::NormalisableRange<double>(0, 255));
//			break;
//		case bdsp::Green:
//			setName("G");
//			slider.setNormalisableRange(juce::NormalisableRange<double>(0, 255));
//			break;
//		case bdsp::Blue:
//			setName("B");
//			slider.setNormalisableRange(juce::NormalisableRange<double>(0, 255));
//			break;
//		default:
//			break;
//		}
//
//		slider.onValueChange = [=]()
//		{
//			auto v = slider.getNormalisableRange().convertTo0to1(slider.getValue());
//			auto c = linkedViewer->getHSBColor();
//
//			switch (channel)
//			{
//			case bdsp::Hue:
//				hue = v;
//				sat = linkedViewer->getSat();
//				bri = linkedViewer->getBri();
//				break;
//			case bdsp::Saturation:
//				hue = linkedViewer->getHue();
//				sat = v;
//				bri = linkedViewer->getBri();
//				break;
//			case bdsp::Brightness:
//				hue = linkedViewer->getHue();
//				sat = linkedViewer->getSat();
//				bri = v;
//				break;
//			case bdsp::Red:
//				c = juce::Colour::fromFloatRGBA(v, c.getFloatGreen(), c.getFloatBlue(), 1.0f);
//				hue = c.getHue();
//				sat = c.getSaturation();
//				bri = c.getBrightness();
//
//				if (sat == 0 || bri == 0 || bri == 1) // edge case where values can snap randomly b/c there's more than one way to represent color in HSB
//				{
//					hue = linkedViewer->getHue();
//				}
//
//				break;
//			case bdsp::Green:
//				c = juce::Colour::fromFloatRGBA(c.getFloatRed(), v, c.getFloatBlue(), 1.0f);
//				hue = c.getHue();
//				sat = c.getSaturation();
//				bri = c.getBrightness();
//				if (sat == 0 || bri == 0 || bri == 1) // edge case where values can snap randomly b/c there's more than one way to represent color in HSB
//				{
//					hue = linkedViewer->getHue();
//
//				}
//				break;
//			case bdsp::Blue:
//				c = juce::Colour::fromFloatRGBA(c.getFloatRed(), c.getFloatGreen(), v, 1.0f);
//				hue = c.getHue();
//				sat = c.getSaturation();
//				bri = c.getBrightness();
//				if (sat == 0 || bri == 0 || bri == 1) // edge case where values can snap randomly b/c there's more than one way to represent color in HSB
//				{
//					hue = linkedViewer->getHue();
//				}
//				break;
//			default:
//				break;
//			}
//			sendChangeMessage();
//		};
//
////		setLookAndFeel(&linkedViewer->universals->MasterCircleSliderLNF);
////		slider.numbers.setLookAndFeel(&linkedViewer->universals->MasterCircleSliderLNF);
//
//		linkedViewer->addChangeListener(this);
//		addChangeListener(linkedViewer);
//
//
//		slider.textFromValueFunction = [=](double v)
//		{
//			juce::String out = juce::String(slider.getNormalisableRange().convertFrom0to1(v));
//
//			int idx = out.lastIndexOf(".");
//			if (idx > 0)
//			{
//				out = out.substring(0, idx);
//			}
//
//			return out;
//		};
//
//		slider.valueFromTextFunction = [=](const juce::String& s)
//		{
//			auto text = s.retainCharacters("0123456789.");
//			return slider.getNormalisableRange().convertTo0to1(text.getDoubleValue());
//		};
//
//		slider.getListener()->sliderValueChanged(&slider);
//
//		slider.numbers.setColour(juce::Label::textColourId, juce::Colours::black);
//
//		addAndMakeVisible(slider);
//	}
//
//	juce::Colour ColorValueSlider::getHSBColor()
//	{
//		auto c = linkedViewer->getHSBColor();
//		auto v = slider.getNormalisableRange().convertTo0to1(slider.getValue());
//		switch (channel)
//		{
//		case bdsp::Hue:
//			return juce::Colour(v, linkedViewer->getSat(), linkedViewer->getBri());
//			break;
//		case bdsp::Saturation:
//			return juce::Colour(linkedViewer->getHue(), v, linkedViewer->getBri());
//			break;
//		case bdsp::Brightness:
//			return juce::Colour(linkedViewer->getHue(), linkedViewer->getSat(), v);
//			break;
//		case bdsp::Red:
//			return juce::Colour::fromFloatRGBA(v, c.getFloatGreen(), c.getFloatBlue(), 1.0f);
//			break;
//		case bdsp::Green:
//			return juce::Colour::fromFloatRGBA(c.getFloatRed(), v, c.getFloatBlue(), 1.0f);
//			break;
//		case bdsp::Blue:
//			return juce::Colour::fromFloatRGBA(c.getFloatRed(), c.getFloatBlue(), v, 1.0f);
//			break;
//		default:
//			break;
//		}
//	}
//
//	void ColorValueSlider::paint(juce::Graphics& g)
//	{
//		g.setColour(juce::Colours::white);
//		g.fillRoundedRectangle(getLocalBounds().toFloat(), linkedViewer->universals->roundedRectangleCurve);
//
//		auto rect = getLocalBounds().withRight(slider.getX());
//		juce::Path p;
//		p.addRoundedRectangle(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, true, false, true, false);
//		g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f));
//		g.fillPath(p);
//
//		g.setColour(juce::Colours::black);
//		g.setFont(universals->Fonts[0].getFont().withHeight(getHeight() * 0.8));
//		g.drawText(getName(), rect, juce::Justification::centred);
//
//		g.drawRoundedRectangle(getLocalBounds().toFloat(), linkedViewer->universals->roundedRectangleCurve, 1);
//		g.drawVerticalLine(slider.getX(), 0, getHeight());
//
//		/*if (!te.isVisible())
//		{
//
//			auto text = getTextFromValue(getValue());
//			int idx = text.lastIndexOf(".");
//
//			text = text.substring(0, idx);
//
//			drawTextVerticallyCentered(g, linkedViewer->universals->Fonts[0].getFont().withHeight(getHeight() * 0.9), text, getLocalBounds().toFloat());
//		}*/
//	}
//
//	void ColorValueSlider::resized()
//	{
//		slider.setBoundsRelative(0.3, 0, 0.7, 1);
//	}
//
//
//
//
//	ColorValueSlider::NoDefaultNumberSlider::NoDefaultNumberSlider(GUI_Universals* universalsToUse)
//		:NumberSlider(universalsToUse, "")
//	{
//	}
//
//	void ColorValueSlider::NoDefaultNumberSlider::mouseDoubleClick(const juce::MouseEvent& e)
//	{
//		showTextEditor();
//	}
//
//
//	void ColorValueSlider::changeListenerCallback(juce::ChangeBroadcaster* source)
//	{
//		double v = 0;
//		auto c = linkedViewer->getHSBColor();
//		switch (channel)
//		{
//		case bdsp::Hue:
//			v = linkedViewer->getHue();
//			break;
//		case bdsp::Saturation:
//			v = linkedViewer->getSat();
//			break;
//		case bdsp::Brightness:
//			v = linkedViewer->getBri();
//			break;
//		case bdsp::Red:
//			v = c.getFloatRed();
//			break;
//		case bdsp::Green:
//			v = c.getFloatGreen();
//			break;
//		case bdsp::Blue:
//			v = c.getFloatBlue();
//			break;
//		default:
//			break;
//		}
//
//		slider.setValue(slider.getNormalisableRange().convertFrom0to1(v), juce::dontSendNotification);
//		slider.numbers.setText(slider.textFromValueFunction(v), juce::dontSendNotification);
//	}
//
//	//================================================================================================================================================================================================
//
//	HexLabel::HexLabel(ColorSpaceViewer* viewer)
//		:Component(viewer->universals),
//		label(viewer->universals)
//	{
//		linkedViewer = viewer;
//		linkedViewer->addChangeListener(this);
////		setLookAndFeel(&viewer->universals->MasterCircleSliderLNF);
//
//		label.onEditorShow = [=]()
//		{
//			label.getCurrentTextEditor()->setInputRestrictions(6, "0123456789aAbBcCdDeEfF");
//		};
//		label.onTextChange = [=]()
//		{
//			auto text = label.getText();
//
//			auto v = text.getHexValue32();
//
//			auto c = juce::Colour(v).withAlpha(1.0f);
//
//			if (c.getBrightness() == 0 || c.getBrightness() == 1 || c.getSaturation() == 0)
//			{
//				linkedViewer->setColor(linkedViewer->getHue(), c.getSaturation(), c.getBrightness());
//			}
//			else
//			{
//				linkedViewer->setColor(c.getHue(), c.getSaturation(), c.getBrightness());
//			}
//		};
//		changeListenerCallback(linkedViewer);
//
//		addAndMakeVisible(label);
//	}
//
//	HexLabel::~HexLabel()
//	{
//		setLookAndFeel(nullptr);
//	}
//
//	void HexLabel::paint(juce::Graphics& g)
//	{
//		g.setColour(juce::Colours::white);
//		g.fillRoundedRectangle(getLocalBounds().toFloat(), linkedViewer->universals->roundedRectangleCurve);
//
//		auto rect = getLocalBounds().withRight(label.getX());
//		juce::Path p;
//		p.addRoundedRectangle(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, true, false, true, false);
//		g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f));
//		g.fillPath(p);
//
//		g.setColour(juce::Colours::black);
//		g.setFont(universals->Fonts[0].getFont().withHeight(getHeight() * 0.8));
//		g.drawText("Hex", rect, juce::Justification::centred);
//
//		g.drawRoundedRectangle(getLocalBounds().toFloat(), linkedViewer->universals->roundedRectangleCurve, 1);
//		g.drawVerticalLine(label.getX(), 0, getHeight());
//	}
//
//	void HexLabel::resized()
//	{
//		label.setBoundsRelative(0.2, 0, 0.8, 1);
//	}
//
//
//	void HexLabel::changeListenerCallback(juce::ChangeBroadcaster* source)
//	{
//		auto c = linkedViewer->getHSBColor().toDisplayString(false);
//		label.setText(c, juce::dontSendNotification);
//	}
//
//	//================================================================================================================================================================================================
//
//
//
//	ColorSelector::ColorSelector(GUI_Universals* universalsToUse)
//		:Component(universalsToUse)
//	{
//		colorSpace = std::make_unique<ColorSpaceViewer>(universalsToUse);
//		addAndMakeVisible(colorSpace.get());
//
//		//================================================================================================================================================================================================
//
//		hueSlider = std::make_unique<HueSlider>(colorSpace.get());
//		addAndMakeVisible(hueSlider.get());
//
//		//================================================================================================================================================================================================
//
//		R = std::make_unique<ColorValueSlider>(colorSpace.get(), Red);
//		addAndMakeVisible(R.get());
//
//		G = std::make_unique<ColorValueSlider>(colorSpace.get(), Green);
//		addAndMakeVisible(G.get());
//
//		B = std::make_unique<ColorValueSlider>(colorSpace.get(), Blue);
//		addAndMakeVisible(B.get());
//
//
//		H = std::make_unique<ColorValueSlider>(colorSpace.get(), Hue);
//		addAndMakeVisible(H.get());
//
//		S = std::make_unique<ColorValueSlider>(colorSpace.get(), Saturation);
//		addAndMakeVisible(S.get());
//
//		V = std::make_unique<ColorValueSlider>(colorSpace.get(), Brightness);
//		addAndMakeVisible(V.get());
//
//		//================================================================================================================================================================================================
//		Hex = std::make_unique<HexLabel>(colorSpace.get());
//		addAndMakeVisible(Hex.get());
//	}
//
//	void ColorSelector::resized()
//	{
//		forceAspectRatio(5.0 / 6.0);
//
//		auto l = getWidth() * 0.8;
//
//		colorSpace->setBounds(shrinkRectangleToInt(juce::Rectangle<float>(l, l).reduced(l * 0.05)));
//		auto w = colorSpace->getWidth() / 4.0;
//		hueSlider->setBounds((colorSpace->getRight() + getWidth() - w) / 2.0, 0, w, colorSpace->getBottom() + colorSpace->getY());
//
//		auto valueArea = juce::Rectangle<float>().leftTopRightBottom(colorSpace->getX(), colorSpace->getBottom() + colorSpace->getY(), colorSpace->getRight(), getHeight() - colorSpace->getY());
//
//		float s = 0.25;
//		float b = (1.0 - 3 * s) / 2.0;
//		R->setBounds(shrinkRectangleToInt(valueArea.getProportion(juce::Rectangle<float>(0, 0, s, 0.25))));
//		G->setBounds(shrinkRectangleToInt(valueArea.getProportion(juce::Rectangle<float>(s + b, 0, s, 0.25))));
//		B->setBounds(shrinkRectangleToInt(valueArea.getProportion(juce::Rectangle<float>(2 * s + 2 * b, 0, s, 0.25))));
//
//		H->setBounds(shrinkRectangleToInt(valueArea.getProportion(juce::Rectangle<float>(0, 1.0 / 3.0, s, 0.25))));
//		S->setBounds(shrinkRectangleToInt(valueArea.getProportion(juce::Rectangle<float>(s + b, 1.0 / 3.0, s, 0.25))));
//		V->setBounds(shrinkRectangleToInt(valueArea.getProportion(juce::Rectangle<float>(2 * s + 2 * b, 1.0 / 3.0, s, 0.25))));
//
//		Hex->setBounds(shrinkRectangleToInt(valueArea.getProportion(juce::Rectangle<float>(0, 2.0 / 3.0, 1, 0.25))));
//	}
//
//	void ColorSelector::paint(juce::Graphics& g)
//	{
//
//		g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f));
//		g.fillAll();
//
//
//	}
//
//	ColorSpaceViewer* ColorSelector::getColorSpace()
//	{
//		return colorSpace.get();
//	}
//
//
//
//
//} // namespace bdsp