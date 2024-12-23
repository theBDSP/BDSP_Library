#pragma once 
namespace bdsp
{
	enum FontGuideLines { Baseline, Ascender, Descender, xHeight, CapHeight };

	/**
	 * Wrapper for juce::Font to provide access to font guideline values and various string width functions
	 */
	struct Font
	{
		/**
		 * Creates an empty font
		 */
		Font()
		{
			widthOrder.clear();

			for (char c = 32; c < 127; ++c) // from ' ' to '~', i.e. all printing ascii characters
			{
				widthOrder.add(juce::juce_wchar(c));
			}




		}

		/**
		 * Creates a font initialized to a certain juce::Font
		 */
		Font(const juce::Font& f)
			:Font()
		{
			setFont(f);
		}


		/**
		 * @return True if provided character can be represented by this font
		 */
		bool canRepresentChar(char c) const
		{
			return font.getStringWidthFloat(juce::String(c)) > 0;
		}


		/**
		 * Sets this object to wrap a new juce::Font object and calculates all its attributes
		 */
		void setFont(const juce::Font& f)
		{
			font = f;


			//================================================================================================================================================================================================
			// Create a new width comparator for the new font and sort the widthOrder with it
			WidthComparator comp(font);
			widthOrder.sort(comp, false);
			//================================================================================================================================================================================================
			// Calculate the new font's guideline values

			ascenderRatio = font.getAscent() / font.getHeight();
			descenderRatio = font.getDescent() / font.getHeight();

			juce::GlyphArrangement glyph_x, glyph_Caps;
			glyph_x.addLineOfText(f, "x", 0, 0);
			glyph_Caps.addLineOfText(f, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 0);

			juce::Path path_x, path_Caps;

			glyph_x.createPath(path_x);
			glyph_Caps.createPath(path_Caps);

			xHeightRatio = -path_x.getBounds().getY() / f.getHeight();
			capHeightRatio = -path_Caps.getBounds().getY() / f.getHeight();
			//================================================================================================================================================================================================

		}


		const juce::Font& getFont() const
		{
			return font;
		}

		/**
		 * Returns the string from a set of strings that when rendered with this font will be the widest
		 * @param strings The strings to test
		 * @return The widest of the provided strings
		 */
		juce::String getWidestString(const juce::StringArray& strings) const
		{
			float max = 0;
			juce::String out;
			for (auto& s : strings)
			{
				float w = font.getStringWidthFloat(s);
				if (w > max)
				{
					max = w;
					out = s;
				}
			}
			return out;
		}

		/**
		 * Returns the widest possible string when rendered with this font given a maximum number of characters and a set of allowed characters
		 * @param maxNumberOfCharacters The maximum length of the string
		 * @param allowedCharacters The set of characters allowed to be in the result - defaults to empty string (all characters allowed)
		 * @return The widest string
		 */
		juce::String getWidestString(int maxNumberOfCharacters, const juce::String& allowedCharacters = juce::String()) const
		{

			juce::juce_wchar widest;
			if (allowedCharacters.isEmpty())
			{
				widest = widthOrder.getLast();
			}
			else
			{
				for (int i = widthOrder.size() - 1; ; --i) // loop through the width order array in reverse
				{
					auto curr = widthOrder.getUnchecked(i);
					if (allowedCharacters.contains(juce::String::charToString(curr))) // if the current character is in the allowed set we're done
					{
						widest = curr;
						break;
					}
				}
			}
			return  juce::String::repeatedString(juce::String::charToString(widest), maxNumberOfCharacters); // retrun the widest character repeated maxNumberofCharacters number of times

		}


		float ascenderRatio = 0, descenderRatio = 0; // ratio of asc./desc. relative to total font height
		float xHeightRatio = 0, capHeightRatio = 0; // ratio of xHeight/capHeight relative to total font height

	private:
		/**
		 * Class used to sort characters by their width when rendered with this font
		 */
		class WidthComparator
		{
		public:
			WidthComparator(const juce::Font& f)
			{
				font = f;
			}
			const int compareElements(juce::juce_wchar first, juce::juce_wchar second)
			{
				float w1 = font.getStringWidthFloat(juce::String::charToString(first));
				float w2 = font.getStringWidthFloat(juce::String::charToString(second));

				return (w1 < w2) ? -1 : ((w2 < w1) ? 1 : 0);
			}

			juce::Font font;
		};
		juce::Font font;
		juce::Array<juce::juce_wchar> widthOrder;// narrowest to widest
	};

	/**
	 * Resizes a font to fit a certain string within a bounding box
	 * @param fontToUse The font to resize
	 * @param maxWidth The maximum allowed width of the string
	 * @param maxHeight The maximum allowed height of the resulting font
	 * @param text The string to resize for
	 * @return The resized font
	 */
	inline juce::Font resizeFontToFit(const juce::Font& fontToUse, const float& maxWidth, const float& maxHeight, const juce::String& text)
	{
		auto currentWidth = fontToUse.getStringWidthFloat(text);
		float newHeight = maxWidth / currentWidth * fontToUse.getHeight() * BDSP_FONT_HEIGHT_SCALING;

		return fontToUse.withHeight(juce::jmin(maxHeight, newHeight));
	}


	/**
	 * Resized a font to fit all possible strings within a bounding box, given a maximum number of characters and an optional set of allowed characters
	 * @param fontToUse The font to resize
	 * @param maxWidth The maximum allowed width of the string
	 * @param maxHeight The maximum allowed height of the resulting font
	 * @param maxNumberOfCharacters The maximum number of characters allowed in the test string
	 * @param allowedCharacters The set of characters allowed in the test string - defaults to empty string (all characters allowed)
	 * @return The resized font
	 */
	inline juce::Font resizeFontToFit(const Font& fontToUse, const float& maxWidth, const float& maxHeight, const int& maxNumberOfCharacters, const juce::String& allowedCharacters = juce::String())
	{
		return resizeFontToFit(fontToUse.getFont(), maxWidth, maxHeight, fontToUse.getWidestString(maxNumberOfCharacters, allowedCharacters));
	}



} // namespace bdsp