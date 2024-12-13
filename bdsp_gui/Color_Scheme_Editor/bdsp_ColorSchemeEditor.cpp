//#include "bdsp_ColorSchemeEditor.h"
//
//namespace bdsp
//{
//	ColorSchemeEditor::ColorSchemeEditor(GUI_Universals* universalsToUse)
//		:DesktopComponent(universalsToUse),
//		name(universalsToUse),
//		saveButton(universalsToUse),
//		cancelButton(universalsToUse),
//		selector(universalsToUse)
//	{
//
//		setLossOfFocusClosesWindow(false);
//
//		addAndMakeVisible(name);
//		addAndMakeVisible(saveButton);
//		addAndMakeVisible(cancelButton);
//		addAndMakeVisible(selector);
//
//		selector.getColorSpace()->addChangeListener(this);
//
//
//		saveButton.onClick = [=]()
//		{
//			writeSchemeToFile();
//			hide();
//		};
//
//		cancelButton.onClick = [=]()
//		{
//			hide();
//		};
//
//		int numColors = universals->colors.colors.size();
//
//		for (int i = 0; i < numColors; ++i)
//		{
//			colorItems.add(new ColorItem(this, universals->colors.names[i]));
//			addAndMakeVisible(colorItems.getLast());
//		}
//		selectItem(colorItems.getFirst());
//
//	}
//	void ColorSchemeEditor::paint(juce::Graphics& g)
//	{
//		g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f));
//		g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(universals->roundedRectangleCurve), universals->roundedRectangleCurve);
//	}
//	void ColorSchemeEditor::paintOverChildren(juce::Graphics& g)
//	{
//		if (selectedItem != nullptr)
//		{
//			g.setColour(juce::Colours::black);
//			g.drawRoundedRectangle(selectedItem->getBounds().toFloat(), universals->roundedRectangleCurve, 1);
//		}
//	}
//	void ColorSchemeEditor::resized()
//	{
//		forceAspectRatio(9.0 / 6.0); // 5/6 of selector + 4/6 of rest
//
//		selector.setBoundsRelative(0, 0, 5.0 / 9.0, 1);
//
//		itemArea = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(5.0 / 9.0, 0, 4.0 / 9.0, 2.0 / 3.0));
//
//		placeItems();
//
//		auto remaining = juce::Rectangle<float>().leftTopRightBottom(itemArea.getX(), itemArea.getBottom(), itemArea.getRight(), getHeight());
//		auto b = 0.1;
//		name.setBounds(shrinkRectangleToInt(remaining.getProportion(juce::Rectangle<float>(b, b, 1 - 2 * b, 0.5 - 1.5 * b))));
//		cancelButton.setBounds(shrinkRectangleToInt(remaining.getProportion(juce::Rectangle<float>(b, 0.5 + b / 2, 0.5 - 1.5 * b, 0.5 - 1.5 * b))));
//		saveButton.setBounds(shrinkRectangleToInt(remaining.getProportion(juce::Rectangle<float>(0.5 + b / 2, 0.5 + b / 2, 0.5 - 1.5 * b, 0.5 - 1.5 * b))));
//	}
//
//	void ColorSchemeEditor::placeItems()
//	{
//		bool lastNeedsCentering = false;
//		int cols = 4;
//		int rows = ceil((float)colorItems.size() / cols);
//
//		int n = 0;
//
//		float b = 0.1;
//
//		float w = 1 - (cols + 1) * b;
//		float h = 1 - (rows + 1) * b;
//
//		for (int i = 0; i < rows; ++i)
//		{
//			for (int j = 0; j < cols; ++j)
//			{
//				if (n >= colorItems.size())
//				{
//					lastNeedsCentering = true;
//					break;
//				}
//				else
//				{
//					colorItems[n]->setBounds(shrinkRectangleToInt(itemArea.getProportion(juce::Rectangle<float>((j + 1) * b + j * w, (i + 1) * b + i * h, w, h))));
//				}
//				++n;
//			}
//		}
//
//		if (lastNeedsCentering)
//		{
//			int num = colorItems.size() % cols;
//			float totalW = num * w + (n + 1) * b;
//			float x = 0.5 - totalW / 2;
//
//			for (int i = 0; i < num; ++i)
//			{
//				colorItems[colorItems.size() - num + i]->setBounds(shrinkRectangleToInt(itemArea.getProportion(juce::Rectangle<float>(x + (i + 1) * b + i * w, rows * b + (rows - 1) * h, w, h))));
//			}
//
//		}
//	}
//	void ColorSchemeEditor::writeSchemeToFile()
//	{
//
//		juce::XmlElement xml("Main");
//		auto infoNode = xml.createNewChildElement("Info");
//		infoNode->setAttribute("Name", BDSP_APP_NAME);
//		infoNode->setAttribute("Version", JucePlugin_VersionString);
//
//		auto colorNode = xml.createNewChildElement("Colors");
//
//		for (auto c : colorItems)
//		{
//			colorNode->setAttribute(c->getColorID(), c->getColor().toString());
//		}
//
//		//================================================================================================================================================================================================
//
//
//		PropertiesFolder folder;
//		auto dir = folder.getFolder().getChildFile("Data").getChildFile("ColorSchemes");
//		bool result = true;
//		if (!dir.exists())
//		{
//			result = dir.createDirectory();
//		}
//
//		if (result)
//		{
//			auto file = dir.getChildFile(name.getText()).withFileExtension("bdspallete");
//			xml.writeTo(file);
//		}
//	}
//	void ColorSchemeEditor::readSchemeFromFile(juce::File f)
//	{
//		auto xml = juce::parseXML(f);
//
//		if (xml != nullptr && xml->getChildByName("Info")->getAttributeValue(0).compare(BDSP_APP_NAME) == 0) // for the right plugin
//		{
//			auto colors = xml->getChildByName("Colors");
//
//			jassert(colors->getNumAttributes() == colorItems.size());
//
//			for (int i = 0; i < colors->getNumAttributes(); ++i)
//			{
//				auto c = juce::Colour::fromString(colors->getAttributeValue(i));
//				jassert(colorItems[i]->getColorID().compare(colors->getAttributeName(i)) == 0);
//				colorItems[i]->setColor(c);
//
//				if (selectedItem == colorItems[i])
//				{
//					selector.getColorSpace()->setColor(c.getHue(), c.getSaturation(), c.getBrightness());
//				}
//			}
//		}
//	}
//	void ColorSchemeEditor::readSchemeFromFile(const juce::String& fileName)
//	{
//		PropertiesFolder folder;
//		auto dir = folder.getFolder().getChildFile("Data").getChildFile("ColorSchemes");
//		readSchemeFromFile(dir.getChildFile(fileName + ".bdspallete"));
//	}
//	void ColorSchemeEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
//	{
//		if (selectedItem != nullptr)
//		{
//		
//			selectedItem->setColor(selector.getColorSpace()->getHSBColor());
//		}
//	}
//	void ColorSchemeEditor::selectItem(ColorItem* item)
//	{
//		selectedItem = item;
//		auto c = item->getColor();
//
//		selector.getColorSpace()->setColor(c.getHue(), c.getSaturation(), c.getBrightness());
//		repaint();
//	}
//
//
//	//================================================================================================================================================================================================
//
//
//	ColorSchemeEditor::ColorItem::ColorItem(ColorSchemeEditor* parent, const juce::Identifier& id)
//		:Component(parent->universals)
//	{
//		colorID = id.toString();
//
//		universalsIndex = universals->colors.names.indexOf(colorID);
//
//	}
//	void ColorSchemeEditor::ColorItem::paint(juce::Graphics& g)
//	{
//		auto s = getWidth();
//
//		g.setColour(color);
//		g.fillRoundedRectangle(0, 0, s, s, universals->roundedRectangleCurve);
//
//		g.setColour(juce::Colours::black);
//		drawTextVerticallyCentered(g, universals->Fonts[0].getFont(), colorID, juce::Rectangle<float>().leftTopRightBottom(0, s, s, getHeight()));
//
//	}
//	void ColorSchemeEditor::ColorItem::resized()
//	{
//		forceAspectRatio(1 / 1.25);
//	}
//	void ColorSchemeEditor::ColorItem::mouseDown(const juce::MouseEvent& e)
//	{
//		p->selectItem(this);
//	}
//	void ColorSchemeEditor::ColorItem::setColor(juce::Colour c)
//	{
//		color = c;
//		repaint();
//		universals->colors.colors.set(universalsIndex, color);
//		universals->schemeChanged();
//
//	}
//	juce::Colour ColorSchemeEditor::ColorItem::getColor()
//	{
//		return color;
//	}
//	juce::String ColorSchemeEditor::ColorItem::getColorID()
//	{
//		return colorID;
//	}
//} // namespace bdsp