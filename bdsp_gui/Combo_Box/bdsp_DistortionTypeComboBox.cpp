#include "bdsp_DistortionTypeComboBox.h"
namespace bdsp
{

	DistortionTypeComboBox::DistortionTypeComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const std::function<void(int)>& changFunc)
		:ComboBoxBase(param, universalsToUse, juce::StringArray(), changFunc),
		DistortionList(this)
	{
		Listuniversals = std::make_unique<DesktopComponent>(universals);
		List = &DistortionList;

		Listuniversals->addAndMakeVisible(DistortionList);

		initMenuComponents();
	}

	void DistortionTypeComboBox::setCurveColor(const NamedColorsIdentifier& c)
	{
		curve = c;
		List->repaint();
	}

	void DistortionTypeComboBox::setRowsAndCols(int rows, int cols)
	{
		List->setRowsAndCols(rows, cols);
		List->heightRatio = 1.0 / cols;
	}


	//================================================================================================================================================================================================


	DistortionTypeComboBox::DistortionList::DistortionList(DistortionTypeComboBox* p)
		:ListComponent(p)
	{
		for (int i = 0; i < p->parameter->choices.size(); i++)
		{
			auto name = p->parameter->choices[i];
			auto type = dynamic_cast<Processor*>(&universals->APVTS->processor)->lookups.distortionLookups->stringToDistortionType(name);
			auto path = universals->commonPaths.distortionPaths.getPath(type);
			auto fill = universals->commonPaths.distortionPaths.getPath(type, true);

			List.add(new DistortionListItem(this, name, path, fill, i));

			pushColorsToNewItem();
		}
	}

	DistortionTypeComboBox::DistortionList::~DistortionList()
	{
	}






	//================================================================================================================================================================================================


	DistortionTypeComboBox::DistortionList::DistortionListItem::DistortionListItem(ListComponent* parentList, const juce::String& name, juce::Path path, juce::Path fillPath, int idx, int retValue)
		:ListItem(parentList, idx, retValue)
	{
		setText(name);
		p = path;
		fill = fillPath;
	}

	void DistortionTypeComboBox::DistortionList::DistortionListItem::resized()
	{
		auto s = juce::jmin((double)getWidth(), getHeight() * 0.75);
		pathBounds = juce::Rectangle<float>(getWidth() / 2.0 - s / 2, 0, s, s).reduced(universals->roundedRectangleCurve);
		scaleToFit(p, pathBounds.reduced(universals->roundedRectangleCurve));
		scaleToFit(fill, pathBounds.reduced(universals->roundedRectangleCurve));
	}

	void DistortionTypeComboBox::DistortionList::DistortionListItem::paint(juce::Graphics& g)
	{
		if (isHighlighted())
		{
			g.setColour(getColor(highlightColor));
			g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
		}

		g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		g.fillRoundedRectangle(pathBounds, universals->roundedRectangleCurve);

		auto h = (getHeight() - pathBounds.getBottom()) * BDSP_FONT_HEIGHT_SCALING * 0.9;
		drawText(g, universals->Fonts[getFontIndex()].getFont().withHeight(h), getText(), getLocalBounds().toFloat().withTop(pathBounds.getBottom()));


		auto c = getColor(dynamic_cast<DistortionTypeComboBox*>(parent->parent)->curve);
		juce::ColourGradient cg(c.withAlpha(universals->midOpacity), pathBounds.getTopLeft(), c.withAlpha(universals->midOpacity), pathBounds.getBottomLeft(), false);
		cg.addColour(0.5, c.withAlpha(universals->lowOpacity));



		g.setGradientFill(cg);
		g.fillPath(fill);


		g.setColour(c);
		g.strokePath(p, juce::PathStrokeType(2 * universals->visualizerLineThickness));



	}


	//================================================================================================================================================================================================
	//================================================================================================================================================================================================


	DistortionTypeSelectorGrid::DistortionTypeSelectorGrid(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, Component* newGridSibling)
		:IncrementalComboBox<>(param, universalsToUse)
	{

		icon = Shape(juce::Path(), NamedColorsIdentifier(), juce::Rectangle<float>(0.1, 0.1, 0.8, 0.8), textDown, 0.1);
		icon.fillOff = textUp;

		setInterceptsMouseClicks(true, true);
		setGridSibling(newGridSibling);
		grid = std::make_unique<Grid>(this);


		Listuniversals = std::make_unique<DesktopComponent>(universals);
		Listuniversals->addAndMakeVisible(grid.get());

		Listuniversals->onShow = [=]()
		{
			resized();
		};

		List = grid.get();

		initMenuComponents();

	}

	DistortionTypeSelectorGrid::~DistortionTypeSelectorGrid()
	{
		if (gridSibling != nullptr)
		{
			gridSibling->removeComponentListener(this);
		}

	}

	void DistortionTypeSelectorGrid::colorsChanged()
	{
		IncrementalComboBox<>::colorsChanged();
		icon.outline = textDown;
		repaint();
	}


	void DistortionTypeSelectorGrid::setRowsAndCols(int rows, int cols)
	{
		grid->setRowsAndCols(rows, cols);
	}

	void DistortionTypeSelectorGrid::setGridSibling(Component* newGridSibling)
	{
		if (gridSibling != nullptr)
		{
			gridSibling->removeComponentListener(this);
		}

		gridSibling = newGridSibling;
		if (newGridSibling != nullptr)
		{
			newGridSibling->addComponentListener(this);
			newGridSibling->getParentComponent()->addAndMakeVisible(grid.get());

		}
	}

	void DistortionTypeSelectorGrid::resized()
	{
		IncrementalComboBox<>::resized();


		auto h = getHeight();

		if (gridSibling == nullptr)
		{


			auto y = Listuniversals->getY();
			auto h = Listuniversals->getWidth() / BDSP_DISTORTION_TYPE_GRID_ASPECT_RATIO;

			if (y + h > universals->desktopManager.getHeight()) // would be cut off
			{
				y = y - getHeight() * 0.1 - h;
			}

			Listuniversals->setBounds(Listuniversals->getX(), y, Listuniversals->getWidth(), h);
			List->setBounds(Listuniversals->getLocalBounds());
			List->resized();
		}
	}

	void DistortionTypeSelectorGrid::paint(juce::Graphics& g)
	{
		IncrementalComboBox<>::paint(g);
		icon.draw(g, getLocalBounds().toFloat().withSizeKeepingCentre(getHeight(), getHeight()), !isListVisible(), isMouseOver(), universals, isEnabled() ? 1.0f : universals->disabledAlpha);
	}

	void DistortionTypeSelectorGrid::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
	{
		Listuniversals->setBounds(gridSibling->getBoundsRelativeToDesktopManager());
		grid->setBoundsRelative(0, 0, 1, 1);
	}


	void DistortionTypeSelectorGrid::parameterChanged(int idx)
	{


		auto name = parameter->choices[idx];

		auto type = dynamic_cast<Processor*>(&universals->APVTS->processor)->lookups.distortionLookups->stringToDistortionType(name);
		auto path = universals->commonPaths.distortionPaths.getPath(type);

		icon.path = path;

		grid->label = name;
		IncrementalComboBox<>::parameterChanged(idx);
	}


	//================================================================================================================================================================================================


	DistortionTypeSelectorGrid::Grid::Grid(DistortionTypeSelectorGrid* p)
		:ListComponent(p)
	{
		for (int i = 0; i < p->parameter->choices.size(); i++)
		{
			auto name = p->parameter->choices[i];
			auto type = dynamic_cast<Processor*>(&universals->APVTS->processor)->lookups.distortionLookups->stringToDistortionType(name);
			auto path = universals->commonPaths.distortionPaths.getPath(type);

			List.add(new Item(this, name, path, i));
			List.getLast()->highlightColor = BDSP_COLOR_PURE_BLACK;
			List.getLast()->textColor = NamedColorsIdentifier(BDSP_COLOR_PURE_BLACK).withMultipliedAlpha(universals->midOpacity);
		}


	}

	void DistortionTypeSelectorGrid::Grid::paint(juce::Graphics& g)
	{
		g.setColour(getColor(BDSP_COLOR_KNOB));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);

		auto c = getColor(List[getHighlightedIndex()]->textColor);

		g.setColour(c);

		auto textRect = getLocalBounds().withTop(List.getLast()->getBottom() + insideBorder.getY());
		juce::Path rect;
		rect.addRoundedRectangle(textRect.getX(), textRect.getY(), textRect.getWidth(), textRect.getHeight(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, false, false, true, true);
		g.fillPath(rect);

		g.setColour(getColor(List[getHighlightedIndex()]->highlightColor));
		drawText(g, universals->Fonts[getFontIndex()].getFont().withHeight(textRect.getHeight() * 0.8), label, textRect);

	}

	void DistortionTypeSelectorGrid::Grid::resized()
	{
		if (!getBounds().isEmpty())
		{
			auto reduced = confineToAspectRatio(getLocalBounds().toFloat(), BDSP_DISTORTION_TYPE_GRID_ASPECT_RATIO);
			insideBorder = reduced.withHeight(reduced.getWidth()).reduced(reduced.getWidth() * BDSP_POPUP_MENU_OUTLINE_RATIO);// .getProportion(juce::Rectangle<float>(0.05, 0.05, 0.9, 0.9));

			postionItemsWithinRectangle(insideBorder);
		}
	}


	//================================================================================================================================================================================================


	DistortionTypeSelectorGrid::Grid::Item::Item(ListComponent* parentList, const juce::String& name, juce::Path path, int idx, int retValue)
		:ListItem(parentList, idx)
	{
		p = path;
		setText(name);
	}

	void DistortionTypeSelectorGrid::Grid::Item::resized()
	{
		forceAspectRatio(1);
		pathBounds = getLocalBounds().toFloat().reduced(universals->roundedRectangleCurve);
		scaleToFit(p, pathBounds);
	}

	void DistortionTypeSelectorGrid::Grid::Item::paint(juce::Graphics& g)
	{

		g.setColour(getColor(highlightColor));



		auto c = getColor(isHighlighted() ? highlightColor : textColor);

		g.setColour(c);
		g.strokePath(p, juce::PathStrokeType(getWidth() * 0.05));


	}

	void DistortionTypeSelectorGrid::Grid::Item::mouseEnter(const juce::MouseEvent& e)
	{
		dynamic_cast<DistortionTypeSelectorGrid::Grid*>(parent)->label = getText();
	}

	void DistortionTypeSelectorGrid::Grid::Item::mouseExit(const juce::MouseEvent& e)
	{
		auto grandparent = dynamic_cast<DistortionTypeSelectorGrid*>(parent->parent);
		dynamic_cast<DistortionTypeSelectorGrid::Grid*>(parent)->label = grandparent->parameter->choices[grandparent->getIndex()];

	}





} // namespace bdsp