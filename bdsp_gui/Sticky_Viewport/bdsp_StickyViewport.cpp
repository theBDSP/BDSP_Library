#include "bdsp_StickyViewport.h"

namespace bdsp
{

	StickyViewport::StickyViewport(GUI_Universals* universalsToUse, bool hasSeperator)
		:ComponentCore(this, universalsToUse),
		vp(universalsToUse)
	{
		seperator = hasSeperator;

		vp.setScrollBarsShown(true, false);
		vp.setViewedComponent(&viewComp);
		addAndMakeVisible(vp);

		;
		titleColor = BDSP_COLOR_WHITE;

	}



	StickyViewport::~StickyViewport()
	{
		vp.setLookAndFeel(nullptr);
		setLookAndFeel(nullptr);
	}
	void StickyViewport::addSection(const juce::String& title)
	{
		titles.add(title);

		universalsComponents.add(new juce::Component());
		viewComp.addAndMakeVisible(universalsComponents.getLast());
	}

	void StickyViewport::addSections(const juce::StringArray& newTitles)
	{
		for (auto& s : newTitles)
		{
			addSection(s);
		}
	}
	void StickyViewport::addAndMakeVisibleToSection(juce::Component& childToAdd, int viewportIndex, int zOrder)
	{
		universalsComponents[viewportIndex]->addAndMakeVisible(childToAdd, zOrder);
		resized();
	}
	void StickyViewport::addAndMakeVisibleToSection(juce::Component* childToAdd, int viewportIndex, int zOrder)
	{
		addAndMakeVisibleToSection(*childToAdd, viewportIndex, zOrder);
	}
	void StickyViewport::resized()
	{
		titleH = getWidth() * titleHRatio;
		border = getWidth() * 0.05;

		vp.setBounds(getLocalBounds().withTrimmedTop(titleH));

		int bottom = -border - titleH;

		for (int i = 0; i < titles.size(); ++i)
		{
			int maxBot = 0;
			for (auto c : universalsComponents[i]->getChildren()) // children should already have been resized outside of this comp
			{
				if (c->getBottom() > maxBot)
				{
					maxBot = c->getBottom();
				}
			}

			universalsComponents[i]->setBounds(0, bottom + border + titleH, getWidth(), maxBot);

			bottom = universalsComponents[i]->getBottom();
		}

		viewComp.setBounds(vp.getLocalBounds().withBottom(universalsComponents.getLast()->getBottom()));
	}


	int StickyViewport::getViewportIndex(const juce::String& title)
	{
		return titles.indexOf(title);
	}

	void StickyViewport::paintOverChildren(juce::Graphics& g)
	{
		auto font = universals->Fonts[getFontIndex()].getFont().withHeight(titleH * 0.9);

		g.setColour(getColor(titleColor));
		g.setFont(font);

		for (int i = 0; i < universalsComponents.size(); ++i)
		{
			auto h = universalsComponents[i];
			if (!(h->getY() > vp.getHeight() || h->getBottom() < 0) && titleHRatio > 0) // section is at least partially visible
			{
				//g.drawText(titles[i], 0, juce::jmax(0, h->getY()) + vp.getY() - titleH, getWidth(), titleH, juce::Justification::centred);
				const auto& p = drawText(g, g.getCurrentFont(), titles[i], juce::Rectangle<int>(0, juce::jmax(0, h->getY()) + vp.getY() - titleH, getWidth(), titleH), juce::Justification::centredTop);

				if (seperator)
				{
					g.drawHorizontalLine(h->getY() + vp.getY() - titleH + p.getBounds().getHeight() * 1.5, titleH, getWidth() - titleH);
				}
			}
		}

	}


	void StickyViewport::lookAndFeelChanged()
	{
		vp.setLookAndFeel(&getLookAndFeel());
	}

	void StickyViewport::clear()
	{
		titles.clear();
		universalsComponents.clear();
	}



} // namespace bdsp
