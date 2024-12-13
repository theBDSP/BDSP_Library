#include "bdsp_PreRenderedComponent.h"
namespace bdsp
{

	PreRenderedComponent::PreRenderedComponent(GUI_Universals* universalsToUse)
		:Component(universalsToUse),
		GUI_Universals::Listener(universalsToUse)
	{
		setBufferedToImage(false);
	}

	void PreRenderedComponent::paint(juce::Graphics& g)
	{
		//g.drawImage(bufferedImage, getLocalBounds().toFloat());
		g.drawImageTransformed(bufferedImage, juce::AffineTransform().scaled((float)getWidth() / bufferedImage.getWidth(), (float)getHeight() / bufferedImage.getHeight()));
	}


	//================================================================================================================================================================================================




	// currently unused
	//ProtrudingContainerComponent::ProtrudingContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& highlightColor, const NamedColorsIdentifier& borderColor, float borderThickness, float highlightThickness, bool isBumpOut, bool respondsToUniversalChanges)
	//	:PreRenderedComponent(universalsToUse)
	//{
	//	background = backgroundColor;
	//	highlight = highlightColor;
	//	border = borderColor;
	//	borderThicc = borderThickness;
	//	highlightThicc = highlightThickness;
	//	bumpOut = isBumpOut;
	//	universalListener = respondsToUniversalChanges;
	//	setPaintingIsUnclipped(true);
	//	setInterceptsMouseClicks(false, true);
	//}

	//void ProtrudingContainerComponent::init(const juce::Rectangle<int>& maxBounds)
	//{

	//	auto padding = highlightThicc * BDSP_CONTAINER_SHADOW_FACTOR * BDSP_BLUR_PADDING_FACTOR;
	//	juce::Path p;

	//	p.addRoundedRectangle(maxBounds.withPosition(padding / 2, padding / 2), universals->roundedRectangleCurve);
	//	bufferedImage = juce::Image(juce::Image::ARGB, maxBounds.getWidth() + padding, maxBounds.getHeight() + padding, true);
	//	juce::Graphics g(bufferedImage);
	//	drawContainer(g, p, universals->colors.getColor(background), universals->colors.getColor(highlight), universals->colors.getColor(border), highlightThicc, borderThicc, bumpOut);
	//}

	//void ProtrudingContainerComponent::GUI_UniversalsChanged()
	//{
	//	if (universalListener)
	//	{
	//		init(bufferedImage.getBounds());
	//	}
	//}
	//void ProtrudingContainerComponent::paint(juce::Graphics& g)
	//{
	//	jassert(bufferedImage.isValid());

	//	auto padding = highlightThicc * BDSP_CONTAINER_SHADOW_FACTOR * BDSP_BLUR_PADDING_FACTOR;
	//	g.drawImageTransformed(bufferedImage, juce::AffineTransform().translated(getLocalBounds().getCentre() - bufferedImage.getBounds().getCentre()).scaled(getWidth() / (bufferedImage.getWidth() - padding), getHeight() / (bufferedImage.getHeight() - padding), getWidth() / 2, getHeight() / 2));

	//}


	BeveledContainerComponent::BeveledContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& borderColor, float innerBorderRatio, float outerBorderRatio)
		:Component(universalsToUse)
	{
		background = backgroundColor;
		border = borderColor;
		innerRatio = innerBorderRatio;
		outterRatio = outerBorderRatio;
		setPaintingIsUnclipped(true);
	}


	void BeveledContainerComponent::paint(juce::Graphics& g)
	{

		juce::Path p;
		p.addRoundedRectangle(getLocalBounds(), universals->roundedRectangleCurve);

		g.setColour(universals->colors.getColor(background));
		g.fillPath(p);

		g.setColour(universals->colors.getColor(BDSP_COLOR_PURE_BLACK));
		g.strokePath(p, juce::PathStrokeType(universals->roundedRectangleCurve * outterRatio));

		g.setColour(universals->colors.getColor(border));
		g.strokePath(p, juce::PathStrokeType(universals->roundedRectangleCurve * innerRatio));
	}


	//================================================================================================================================================================================================


	BasicContainerComponent::BasicContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& backgroundColorGradient)
		:Component(universalsToUse)
	{
		setColors(backgroundColor, backgroundColorGradient);
	}


	void BasicContainerComponent::paint(juce::Graphics& g)
	{

		if (background2.isEmpty())
		{
			g.setColour(universals->colors.getColor(background));
		}
		else
		{
			juce::ColourGradient cg(universals->colors.getColor(background), 0, 0, universals->colors.getColor(background2), 0, getHeight(), false);
			g.setGradientFill(cg);
		}
		g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);

	}

	void BasicContainerComponent::setColors(const NamedColorsIdentifier& backgroundColor, const  NamedColorsIdentifier& backgroundColorGradient)
	{
		background = backgroundColor;
		background2 = backgroundColorGradient;
		repaint();
	}

	TexturedContainerComponent::TexturedContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& backgroundColorGradient, bool rotate90)
		:BasicContainerComponent(universalsToUse, backgroundColor, backgroundColorGradient)
	{
		rotate = rotate90;
	}

	void TexturedContainerComponent::paint(juce::Graphics& g)
	{
		BasicContainerComponent::paint(g);

		paintTexture(g);
	}

	void TexturedContainerComponent::paintTexture(juce::Graphics& g)
	{
		juce::Path p;
		p.addRoundedRectangle(getLocalBounds(), universals->roundedRectangleCurve);
		g.saveState();
		g.reduceClipRegion(p);
		g.setColour(universals->textureHighlightAdjustmentFunc(getColor(background)));
		if (rotate)
		{
			g.drawImageTransformed(universals->texture, juce::AffineTransform().rotated(PI / 2).translated(universals->texture.getHeight(), 0).scaled(float(getWidth()) / universals->texture.getHeight(), float(getHeight()) / universals->texture.getWidth()), true);
		}
		else
		{
			g.drawImageWithin(universals->texture, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit, true);
		}


		g.restoreState();
	}

} // namespace bdsp
