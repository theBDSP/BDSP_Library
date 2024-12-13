#pragma once 

#define BDSP_CONTAINER_SHADOW_FACTOR 3 // currently unused
#define BDSP_BLUR_PADDING_FACTOR 4 // factor to multiply blur size by to ensure the image the blur is rendered to is large enough to avoid clipping the blur


namespace bdsp
{
	/**
	 * Calls the path function scale to fit with a rectangle as bounds instead of individual arguments for x, y, width, and height
	 */
	template<typename T>
	inline void scaleToFit(juce::Path& p, const juce::Rectangle<T>& bounds, bool preserveProportions = true)
	{
		p.scaleToFit(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), preserveProportions);
	}



	/**
	 * Calculates the exact bounding box for a path. The bounding box you get from getBounds() can be inaccurate for complex paths since it includes control points in the bounding box.
	 * @param p The path to get the bounding box of
	 */
	inline juce::Rectangle<double> getPrecisePathBounds(const juce::Path& p)
	{
		juce::PathFlatteningIterator it(p);
		it.next(); // get first point of the path

		double xMin = juce::jmin(it.x1, it.x2);
		double xMax = juce::jmax(it.x1, it.x2);

		double yMin = juce::jmin(it.y1, it.y2);
		double yMax = juce::jmax(it.y1, it.y2);

		while (it.next()) // loop through each point in flattened path
		{

			if (it.x2 < xMin)
			{
				xMin = it.x2;
			}
			else if (it.x2 > xMax)
			{
				xMax = it.x2;
			}

			if (it.y2 < yMin)
			{
				yMin = it.y2;
			}
			else if (it.y2 > yMax)
			{
				yMax = it.y2;
			}

		}

		return juce::Rectangle<double>().leftTopRightBottom(xMin, yMin, xMax, yMax);
	}



	/**
	 * Scales a path to fit a certain bounding box using a provided precise bounding box
	 * @param precisePathBounds The true bounding box of the provided path (the bounding box you get from getBounds() can be inaccurate for complex paths)
	 * @param overridePath If true this function will scale the provided path to its new size, if false the provided path will remain unchanged - defaults to true
	 */
	template<typename T>
	inline juce::Path scaleToFitPrecise(juce::Path& p, const juce::Rectangle<T>& bounds, const juce::Rectangle<double>& precisePathBounds, bool preserveProportions = true, bool overridePath = true)
	{

		T w = bounds.getWidth();
		T h = bounds.getHeight();

		auto doubleBounds = bounds.toDouble();
		if (preserveProportions) // if we need to perseve proportion we have to scale doubleBounds to keep the path's aspect ratio and fit within the desired bounding box
		{
			if (!(bounds.isEmpty() || precisePathBounds.isEmpty())) // check that we aren't dealing with any empty bounding boxes
			{


				float newW, newH;
				auto srcRatio = precisePathBounds.getHeight() / precisePathBounds.getWidth(); // aspect ratio of the path

				if (srcRatio > h / w) // if the path's aspect ratio is greater than the aspect ratio of the desired bounding box
				{
					// shrink width of new path
					newW = h / srcRatio;
					newH = h;
				}
				else
				{
					// shrink height of new path
					newW = w;
					newH = w * srcRatio;
				}

				auto newXCentre = doubleBounds.getCentreX();
				auto newYCentre = doubleBounds.getCentreY();


				doubleBounds = juce::Rectangle<double>(newW, newH).withCentre(juce::Point<double>(newXCentre, newYCentre));
			}
		}

		// create an affine transform that maps the path's current bounding box to the desired bounding box (doubleBounds)
		auto transform = juce::AffineTransform().fromTargetPoints(precisePathBounds.getTopLeft(), doubleBounds.getTopLeft(), precisePathBounds.getTopRight(), doubleBounds.getTopRight(), precisePathBounds.getBottomLeft(), doubleBounds.getBottomLeft());

		if (overridePath)
		{
			p.applyTransform(transform);
			return p;
		}
		else
		{
			auto out = p;
			out.applyTransform(transform);
			return out;
		}
	}

	/**
	 * Gets the path's precise bounds and scales it to fit a certain bounding box
	 * @param overridePath If true this function will scale the provided path to its new size, if false the provided path will remain unchanged - defaults to true
	 */
	template<typename T>
	inline juce::Path scaleToFitPrecise(juce::Path& p, const juce::Rectangle<T>& bounds, bool preserveProportions = true, bool overridePath = true)
	{

		const auto& boundsRect = getPrecisePathBounds(p);
		scaleToFitPrecise(p, bounds, boundsRect, preserveProportions, overridePath);
		return p;
	} // uses precise method of getting path bounds


	enum CornerCurves :int { topLeft = 1, topRight = 2, bottomLeft = 4, bottomRight = 8 };

	/**
	 * Creates a rounded rectangle path
	 * @param bounds The bounding box of the resulting path
	 * @param curveBools A combination of flags indicating which corners should be curved
	 * @param cornerRadius The radius of the curved corners
	 */
	inline juce::Path getRoundedRectangleFromCurveBools(juce::Rectangle<float> bounds, CornerCurves curveBools, float cornerRadius)
	{
		juce::Path out;
		out.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), cornerRadius, cornerRadius, curveBools & topLeft, curveBools & topRight, curveBools & bottomLeft, curveBools & bottomRight);
		return out;
	}

	/**
	 * Renders text into a path
	 * @param f The font to use
	 * @param text The text to render
	 * @param bounds The bounding box of the resulting path
	 * @param justification The justification used to place the text in the bounding box - defaults to centred
	 * @param scaleFactor How much of the bounding box the path should fill. Only values greater than 0 will have effect. Values of 0 will result in the size of the path being determined the height of the font provided - defaults to 0
	 */
	template <typename T>
	inline juce::Path createTextPath(const juce::Font& f, const juce::String& text, const juce::Rectangle<T>& bounds, juce::Justification justification = juce::Justification::centred, float scaleFactor = 0)
	{
		// create a GlyphArrangement representation of the text
		juce::GlyphArrangement ga;
		ga.addLineOfText(f, text, 0, 0);

		// render the GlyphArrangement representation to a path
		juce::Path p;
		ga.createPath(p);
		if (scaleFactor > 0)
		{
			scaleToFitPrecise(p, bounds.getProportion(juce::Rectangle<float>(0, (1 - scaleFactor) / 2, 1, scaleFactor)));
		}

		int leadingWhitespaces = text.length() - text.trimStart().length();
		int trailingWhitespaces = text.length() - text.trimEnd().length();

		float whitespaceWidth = f.getStringWidthFloat(" ");

		auto pBounds = getPrecisePathBounds(p).withTrimmedLeft(-whitespaceWidth * leadingWhitespaces).withTrimmedRight(-whitespaceWidth * trailingWhitespaces); // bounding box of the path including whitespaces omitted when rendering the path with GlyphArrangement

		float xTrans;
		float yTrans;

		if (justification.testFlags(juce::Justification::left))
		{
			xTrans = bounds.getX() - pBounds.getX();
		}
		else if (justification.testFlags(juce::Justification::right))
		{
			xTrans = bounds.getRight() - pBounds.getRight();
		}
		else
		{
			xTrans = bounds.getCentreX() - pBounds.getCentreX();
		}


		//================================================================================================================================================================================================


		if (justification.testFlags(juce::Justification::top))
		{
			yTrans = bounds.getY() - pBounds.getY();
		}
		else if (justification.testFlags(juce::Justification::bottom))
		{
			yTrans = bounds.getBottom() - pBounds.getBottom();
		}
		else
		{
			yTrans = bounds.getCentreY() - pBounds.getCentreY();
		}

		p.applyTransform(juce::AffineTransform().translated(xTrans, yTrans));

		return p;
	}

	/**
	 * Renders text into a path, then draws it to the provided graphics context
	 * @param g The graphics context to draw to
	 * @param f The font to use
	 * @param text The text to render
	 * @param bounds The bounding box of the resulting path
	 * @param fillBounds If true, scales the path to fill as much of the provided bounding box as possible
	 * @param justification The justification used to place the text in the bounding box - defaults to centred
	 * @param scaleFactor How much of the bounding box the path should fill. Only values greater than 0 will have effect. Values of 0 will result in the size of the path being determined the height of the font provided - defaults to 0
	 */
	template <typename T>
	inline juce::Path drawText(juce::Graphics& g, const juce::Font& f, const juce::String& text, const juce::Rectangle<T>& bounds, bool fillBounds = false, juce::Justification justification = juce::Justification::centred, float scaleFactor = 0)
	{

		juce::Path p(createTextPath(f, text, bounds, justification, fillBounds ? 0 : scaleFactor));
		if (fillBounds)
		{
			auto aspect = (float)p.getBounds().getWidth() / p.getBounds().getHeight();

			scaleFactor = (scaleFactor == 0) ? 1 : scaleFactor;

			// get the maximum bounding box for the given scale factor
			auto scaleBounds = bounds.getProportion(juce::Rectangle<float>(0, 0, 1, scaleFactor));
			scaleBounds = scaleBounds.withWidth(scaleBounds.getHeight() * aspect);

			if (scaleBounds.getWidth() > bounds.getWidth())
			{
				//if the ideal scaled bounding box is too wide, shrink it while maintaining its aspect ratio
				auto diff = scaleBounds.getWidth() - bounds.getWidth();
				scaleBounds = scaleBounds.reduced(diff / 2, diff / (2 * aspect));
			}

			if (scaleBounds.getHeight() > bounds.getHeight())
			{
				//if the ideal scaled bounding box is too tall, shrink it while maintaining its aspect ratio
				auto diff = scaleBounds.getHeight() - bounds.getHeight();
				scaleBounds = scaleBounds.reduced(aspect * diff / 2, diff / 2);
			}

			if (justification.testFlags(juce::Justification::left))
			{
				scaleBounds = scaleBounds.withX(bounds.getX());
			}
			else if (justification.testFlags(juce::Justification::horizontallyCentred))
			{
				scaleBounds = scaleBounds.withCentre({ bounds.getCentreX(), scaleBounds.getCentreY() });
			}
			else
			{
				scaleBounds = scaleBounds.withRightX(bounds.getRight());
			}

			if (justification.testFlags(juce::Justification::top))
			{
				scaleBounds = scaleBounds.withY(bounds.getY());
			}
			else if (justification.testFlags(juce::Justification::verticallyCentred))
			{
				scaleBounds = scaleBounds.withCentre({ scaleBounds.getCentreX(),bounds.getCentreY() });
			}
			else
			{
				scaleBounds = scaleBounds.withBottomY(bounds.getBottom());
			}

			auto pathBounds = p.getBounds();

			// create an affine transform that maps the path's current bounding box to the desired bounding box (scaleBounds)
			auto transform = juce::AffineTransform().fromTargetPoints(pathBounds.getTopLeft(), scaleBounds.toFloat().getTopLeft(), pathBounds.toFloat().getTopRight(), scaleBounds.toFloat().getTopRight(), pathBounds.getBottomLeft(), scaleBounds.toFloat().getBottomLeft());
			p.applyTransform(transform);

		}
		g.fillPath(p);

		return p;
	}


	/**
	 * Draws an attributed string to the provided graphics context
	 * @param g The graphics context to draw to
	 * @param f The font to use if a font is not specified in the attributes of the string provided
	 * @param text The text to render
	 * @param bounds The bounding box to draw into
	 * @param heightRatio The ratio of the bounding box's height to set the font's height to - defaults to 0.9
	 * @param justification The justification used to place the text in the bounding box - defaults to centred
	 */
	inline void drawAttributedText(juce::Graphics& g, const juce::Font& f, const juce::AttributedString& text, const juce::Rectangle<float>& bounds, float heightRatio = 0.9f, juce::Justification justification = juce::Justification::centred)
	{
		juce::OwnedArray<juce::GlyphArrangement> ga; // GlyphArrangments for each span of attributes
		juce::Array<juce::Colour> colors; // colors for each span of attributes
		float totalW = 0;


		for (int i = 0; i < text.getNumAttributes(); ++i)
		{
			ga.add(new juce::GlyphArrangement());
			auto& att = text.getAttribute(i);
			juce::Font font = (att.font == juce::Font()) ? f : att.font; // the font set by the attribute or the default one provided
			font = font.withHeight(bounds.getHeight() * heightRatio); // scale the font 
			colors.add(att.colour);

			ga.getLast()->addLineOfText(font, text.getText().substring(att.range.getStart(), att.range.getEnd()), totalW, bounds.getCentreY() + font.getAscent() / 2); // add the text effected by this attribute to the GlyphArrangement array
			totalW += ga.getLast()->getBoundingBox(att.range.getStart(), att.range.getLength(), true).getWidth(); // add the width of this text to running total
		}


		float deltaX = bounds.getX();

		// set deltaX to correct value based on provided justification
		if (justification.testFlags(juce::Justification::horizontallyCentred))
		{
			deltaX = bounds.getCentreX() - totalW / 2;
		}
		else if (justification.testFlags(juce::Justification::right))
		{
			deltaX = bounds.getRight() - totalW;
		}





		for (int i = 0; i < ga.size(); i++)
		{
			g.setColour(colors[i]);
			ga[i]->moveRangeOfGlyphs(0, -1, deltaX, 0); // shift the GlyphaArrangement to the correct x-location
			ga[i]->draw(g);
		}
	}



	/**
	 * Renders text into a path, centering it vertically relative to certain font guidelindes
	 * @param f The font to use - make sure this is a bdsp::Font object and not a juce::Font object
	 * @param text The text to render
	 * @param bounds The bounding box of the resulting path
	 * @param topLine The font guidline for the top edge when centering vertically - defaults to Ascender
	 * @param bottomLine The font guidline for the bottom edge when centering vertically - defaults to Descender
	 * @param elipsesIfTooBig If true, truncates the text if it is wider than the bounding box and replaces last few characters with ... - defaults to false
	 */
	inline juce::Path createTextPathVerticallyCentered(const Font& f, const juce::String& text, const juce::Rectangle<float>& bounds, FontGuideLines topLine = Ascender, FontGuideLines bottomLine = Descender, bool elipsesIfTooBig = false)
	{
		juce::GlyphArrangement ga;
		ga.addLineOfText(f.getFont(), text, 0, 0);
		auto textBounds = ga.getBoundingBox(0, -1, true);
		if (elipsesIfTooBig && textBounds.getWidth() > bounds.getWidth()) // if the text is too wide and needs to be truncated
		{
			int end = ga.getNumGlyphs();
			auto elipsesW = f.getFont().getStringWidthFloat("...");
			if (elipsesW <= bounds.getWidth()) // check to make sure we don't enter an infinite loop trying to find truncated text that fits in the bounding box
			{
				while (textBounds.getWidth() > bounds.getWidth() - elipsesW && end > 0) // loop through each character in text until the remaining text + the elipses fits within the bounding box
				{
					end--;
					textBounds = ga.getBoundingBox(0, end, true);
				}
			}

			// set ga to truncated text + elipses
			ga.clear();
			ga.addLineOfText(f.getFont(), text.substring(0, end) + "...", 0, 0);

		}

		juce::Path p;
		ga.createPath(p);

		//================================================================================================================================================================================================
		// calculate all possible font guideline values
		auto h = f.getFont().getHeight();
		auto descender = f.descenderRatio * h;
		auto ascender = f.ascenderRatio * h;
		auto x_Height = f.xHeightRatio * h;
		auto capHeight = f.capHeightRatio * h;
		//================================================================================================================================================================================================


		//================================================================================================================================================================================================
		// get the appropiate values for the top and bottom edge
		float top, bot;

		switch (topLine)
		{
		case Baseline:
			top = 0;
			break;
		case Ascender:
			top = ascender;
			break;
		case Descender:
			jassertfalse;
			break;
		case xHeight:
			top = x_Height;
			break;
		case CapHeight:
			top = capHeight;
			break;
		}
		switch (bottomLine)
		{
		case Baseline:
			bot = 0;
			break;
		case Ascender:
			jassertfalse;
			break;
		case Descender:
			bot = descender;
			break;
		case xHeight:
			bot = x_Height;
			break;
		case CapHeight:
			bot = capHeight;
			break;
		}
		//================================================================================================================================================================================================





		p.applyTransform(juce::AffineTransform().translated(bounds.getCentreX() - p.getBounds().getCentreX(), bounds.getCentreY() + (top + bot) / 2));

		return p;
	}
	inline juce::Path drawTextVerticallyCentered(juce::Graphics& g, const Font& f, const juce::String& text, const juce::Rectangle<float>& bounds, FontGuideLines topLine = Ascender, FontGuideLines bottomLine = Descender, bool elipsesIfTooBig = false)
	{
		auto p = createTextPathVerticallyCentered(f, text, bounds, topLine, bottomLine, elipsesIfTooBig);
		g.fillPath(p);


		return p;

	}


	// only use if you need clipping or non solid colors

	/**
	 * Renders a path, then blurs it, and optionally clips the result to another path. Draws the result to the provided graphics context.
	 * Only use this function if you need clipping or a non solid fill. For all other blurring needs you should directly call the melatonin methods for creating drop shadows.
	 * @param g The context to draw to
	 * @param p The path to blur
	 * @param fill The fill to render the path with before blurring it
	 * @param blurRadius The radius of the blur in pixels
	 * @param clipPath The path to clip the blurred path to - defualts to an empty path (no clipping)
	 */
	inline void blurPath(juce::Graphics& g, const juce::Path& p, juce::FillType fill, size_t blurRadius, const juce::Path& clipPath = juce::Path())
	{
		juce::Image blurImg(juce::Image::ARGB, p.getBounds().getWidth() + BDSP_BLUR_PADDING_FACTOR * blurRadius, p.getBounds().getHeight() + BDSP_BLUR_PADDING_FACTOR * blurRadius, true); // create an empty image to render the blur into
		juce::Graphics blurG(blurImg); // create a graphics context to draw onto the image

		const auto& transform = juce::AffineTransform().translated(-p.getBounds().getPosition()).translated(BDSP_BLUR_PADDING_FACTOR / 2.0 * blurRadius, BDSP_BLUR_PADDING_FACTOR / 2.0 * blurRadius); // transform to translate the path back to its intended position accounting for added size from padding
		auto translated = p;
		translated.applyTransform(transform);

		// render the path to the image context
		fill.transform = fill.transform.followedBy(transform);
		blurG.setFillType(fill);
		blurG.fillPath(translated);


		// blur the image
		melatonin::CachedBlur blur(blurRadius);
		blur.render(blurImg);



		//================================================================================================================================================================================================
		// render the blurred path with optional clipping
		g.saveState();
		if (!clipPath.isEmpty())
		{
			g.reduceClipRegion(clipPath);
		}
		g.drawImage(blurImg, blurImg.getBounds().toFloat().withCentre(p.getBounds().getCentre()));
		g.restoreState();
		//================================================================================================================================================================================================
	}

	/**
	 * Draws a basic divider with a faux-3D effect
	 * @param g The context to draw to
	 * @param l The line to draw the divider on - placement of highlight is dependent on the direction of the line ( flipping the line will flip where the highlight gets placed)
	 * @param highlight The color of the highlight
	 * @param dividerW The width of the divider
	 */
	inline void drawDivider(juce::Graphics& g, const juce::Line<float>& l, const juce::Colour& highlight, const float& dividerW)
	{
		auto highlightW = dividerW * 0.33;
		auto shift = ceil((dividerW + highlightW / 2));

		juce::Path divider, dividerHighlight;
		divider.addLineSegment(l, dividerW);

		//================================================================================================================================================================================================
		// copy the line and translate the copy
		auto newL = l;
		newL.applyTransform(juce::AffineTransform().translated(juce::Point<float>(0, 0).getPointOnCircumference(shift, fmod(l.getAngle(), PI) + PI / 2)));
		//================================================================================================================================================================================================
		// draw the highlight first
		dividerHighlight.addLineSegment(newL, highlightW);
		g.setColour(highlight);
		g.fillPath(dividerHighlight);
		//================================================================================================================================================================================================
		// draw the shadow on top
		g.setColour(juce::Colour(0, 0, 0));
		g.fillPath(divider);

	}


	// currently unused
	//inline void drawContainer(juce::Graphics& g, const juce::Path& p, const juce::Colour& backgroundColor, const juce::Colour& highlightColor, const juce::Colour& borderColor, float highlightThickness, float borderThickness, bool bumpOut = true)
	//{
	//	juce::ColourGradient shadow(juce::Colour(), 0, p.getBounds().getY(), juce::Colour(0, 0, 0), 0, p.getBounds().getBottom(), false);

	//	juce::Path temp;
	//	juce::PathStrokeType stroke(highlightThickness * 1.5);
	//	stroke.createStrokedPath(temp, p);
	//	blurPath(g, temp, shadow, highlightThickness * BDSP_CONTAINER_SHADOW_FACTOR);
	//	g.setColour(backgroundColor);
	//	g.fillPath(p);


	//	juce::ColourGradient highlight(highlightColor, 0, p.getBounds().getY(), juce::Colour(0, 0, 0), 0, p.getBounds().getBottom(), false);
	//	if (!bumpOut)
	//	{
	//		highlight.point1 = p.getBounds().getBottomLeft();
	//		highlight.point2 = p.getBounds().getTopLeft();
	//	}

	//	stroke.setStrokeThickness(highlightThickness);
	//	stroke.createStrokedPath(temp, p);
	//	blurPath(g, temp, highlight, highlightThickness * 2, p);

	//	g.setColour(borderColor);
	//	stroke.setStrokeThickness(borderThickness);
	//	g.strokePath(p, stroke);

	//}


} // namespace bdsp

