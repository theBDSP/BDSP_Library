#include "bdsp_DistortionTypeComboBox.h"
namespace bdsp
{

	DistortionTypeComboBox::DistortionTypeComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const std::function<void(int)>& changFunc)
		:ComboBoxBase(param, universalsToUse, juce::StringArray(), changFunc),
		DistortionList(this)
	{
		ListHolder = std::make_unique<DesktopComponent>(universals);
		List = &DistortionList;

		ListHolder->addAndMakeVisible(DistortionList);

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
			auto type = dynamic_cast<Processor*>(&universals->APVTS->processor)->lookups.distortionLookups->nameToDistortionType(name);
			auto path = type->getIcon(false);
			auto fill = type->getIcon(true);

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


	DistortionTypeSelectorGrid::DistortionTypeSelectorGrid(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, Component* newGridSibling, const juce::Array<juce::Path>& paths, const juce::StringArray& dispNames)
		:IncrementalComboBox<AdvancedComboBox>(param, universalsToUse, paths, dispNames)
	{

		//icon = Shape(juce::Path(), NamedColorsIdentifier(), juce::Rectangle<float>(0.1, 0.1, 0.8, 0.8), textDown, 0.1);
		//icon.fillOff = textUp;

		setInterceptsMouseClicks(true, true);
		setGridSibling(newGridSibling);


		ListHolder = std::make_unique<DesktopComponent>(universals);
		advancedList = std::make_unique<Grid>(this);
		List = advancedList.get();

		ListHolder->addAndMakeVisible(advancedList.get());
		initMenuComponents();

	}

	DistortionTypeSelectorGrid::~DistortionTypeSelectorGrid()
	{
		if (gridSibling != nullptr)
		{
			gridSibling->removeComponentListener(this);
		}

	}

	void DistortionTypeSelectorGrid::attach(juce::RangedAudioParameter* param, juce::UndoManager* undo)
	{
		IncrementalComboBox<AdvancedComboBox>::attach(param, undo);

		icons.ensureStorageAllocated(parameter->choices.size());
		for (int i = 0; i < parameter->choices.size(); ++i)
		{
			auto name = parameter->choices[i];

			auto type = dynamic_cast<Processor*>(&universals->APVTS->processor)->lookups.distortionLookups->nameToDistortionType(name);
			auto path = type->getIcon(false);

			juce::PathStrokeType(path.getBounds().getWidth() * 0.05).createStrokedPath(path, path);
			icons.set(i, new juce::Path(path));
		}
	}

	void DistortionTypeSelectorGrid::colorsChanged()
	{
		IncrementalComboBox<AdvancedComboBox>::colorsChanged();
		//icon.outline = textDown;
		repaint();
	}


	void DistortionTypeSelectorGrid::setRowsAndCols(int rows, int cols)
	{
		advancedList->setRowsAndCols(rows, cols);
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
			newGridSibling->getParentComponent()->addAndMakeVisible(advancedList.get());

		}
	}

	void DistortionTypeSelectorGrid::resized()
	{
		IncrementalComboBox<AdvancedComboBox>::resized();

		if (gridSibling == nullptr)
		{


			auto y = ListHolder->getY();
			auto h = ListHolder->getWidth() / BDSP_DISTORTION_TYPE_GRID_ASPECT_RATIO;

			if (y + h > universals->desktopManager.getHeight()) // would be cut off
			{
				y = y - getHeight() * 0.1 - h;
			}

			ListHolder->setBounds(ListHolder->getX(), y, ListHolder->getWidth(), h);
			List->setBounds(ListHolder->getLocalBounds());
			List->resized();
		}
	}

	void DistortionTypeSelectorGrid::paint(juce::Graphics& g)
	{
		IncrementalComboBox<AdvancedComboBox>::paint(g);
		//icon.draw(g, getLocalBounds().toFloat().withSizeKeepingCentre(getHeight(), getHeight()), !isListVisible(), isMouseOver(), universals, isEnabled() ? 1.0f : universals->disabledAlpha);
	}

	void DistortionTypeSelectorGrid::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
	{
		ListHolder->setBounds(gridSibling->getBoundsRelativeToDesktopManager());
		advancedList->setBoundsRelative(0, 0, 1, 1);
	}


	void DistortionTypeSelectorGrid::parameterChanged(int idx)
	{


		auto name = parameter->choices[idx];

		auto grid = dynamic_cast<Grid*>(advancedList.get());

		grid->label = name;
		grid->repaint();
		IncrementalComboBox<AdvancedComboBox>::parameterChanged(idx);
	}


	//================================================================================================================================================================================================


	DistortionTypeSelectorGrid::Grid::Grid(DistortionTypeSelectorGrid* p)
		:AdvancedList(p)
	{
		for (int i = 0; i < p->parameter->choices.size(); i++)
		{
			auto name = p->parameter->choices[i];
			auto type = dynamic_cast<Processor*>(&universals->APVTS->processor)->lookups.distortionLookups->nameToDistortionType(name);
			auto path = type->getIcon(false);

			List.add(new AdvancedListItem(this, name, path, i));
			addAndMakeVisible(List.getLast());
		}


	}

	void DistortionTypeSelectorGrid::Grid::paint(juce::Graphics& g)
	{
		juce::Path p = getRoundedRectangleFromCurveBools(getLocalBounds().toFloat().reduced(insideBorder.getX()), CornerCurves::all, universals->roundedRectangleCurve);


		dropShadow.render(g, p);


		g.setColour(getColor(background));
		g.fillPath(p);

		auto c = getColor(List[getHighlightedIndex()]->textColor);

		g.setColour(c);

		auto textRect = juce::Rectangle<float>().leftTopRightBottom(insideBorder.getX(), List.getLast()->getBottom() + insideBorder.getX(), insideBorder.getRight(), getHeight() - insideBorder.getX());
		juce::Path rect = getRoundedRectangleFromCurveBools(textRect, CornerCurves::bottom, universals->roundedRectangleCurve);
		//g.fillPath(rect);
		g.drawHorizontalLine(textRect.getY(), textRect.getX(), textRect.getRight());


		//g.setColour(getColor(background));
		drawText(g, getFont().withHeight(textRect.getHeight() * 0.8), label, textRect);

	}

	void DistortionTypeSelectorGrid::Grid::resized()
	{
		if (!getBounds().isEmpty())
		{
			auto reduced = confineToAspectRatio(getLocalBounds().toFloat(), BDSP_DISTORTION_TYPE_GRID_ASPECT_RATIO, juce::RectanglePlacement::xMid | juce::RectanglePlacement::yTop);
			insideBorder = reduced.withHeight(reduced.getWidth()).reduced(reduced.getWidth() * BDSP_POPUP_MENU_OUTLINE_RATIO);// .getProportion(juce::Rectangle<float>(0.05, 0.05, 0.9, 0.9));
			dropShadow.setRadius(insideBorder.getY());

			postionItemsWithinRectangle(insideBorder);
		}
	}

	void DistortionTypeSelectorGrid::Grid::mouseEnter(const juce::MouseEvent& e)
	{
		mouseMove(e);
	}

	void DistortionTypeSelectorGrid::Grid::mouseMove(const juce::MouseEvent& e)
	{
		int idx = -1;
		auto pos = e.getEventRelativeTo(this).position;
		for (int i = 0; i < List.size(); ++i)
		{
			if (List[i]->getBounds().translated(insideBorder.getX(), insideBorder.getY()).toFloat().contains(pos))
			{
				idx = i;
				break;
			}
		}


		if (idx > -1)
		{
			label = List[idx]->getText();
			repaint();
		}
		else
		{
			//mouseExit(e);
		}
	}

	void DistortionTypeSelectorGrid::Grid::mouseExit(const juce::MouseEvent& e)
	{
		if (SelectedIndex > -1)
		{
			label = List[SelectedIndex]->getText();
			repaint();
		}
	}




} // namespace bdsp
