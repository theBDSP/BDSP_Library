#pragma once



namespace bdsp
{

	class HintBar : public juce::Component
	{
	public:
		HintBar(GUI_Universals* universalsToUse)
		{
			universals = universalsToUse;
			setColor(BDSP_COLOR_LIGHT);
		}
		~HintBar() = default;

		void setFont(const juce::Font& font)
		{
			f = font;
		}

		juce::Font getFont()
		{
			return f;
		}

		void setText(const juce::AttributedString& description)
		{

			text = description;
			repaint();
		}

		void setTextToReColor(const juce::Array<std::pair<juce::String, NamedColorsIdentifier>>& textToColor)
		{
			textToReColor = textToColor;
		}
		void addTextToRecolor(std::pair<juce::String, NamedColorsIdentifier> textToColor)
		{
			bool found = false;
			for (auto& r : textToReColor)
			{
				if (textToColor.first.compare(r.first) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				textToReColor.add(textToColor);
			}

		}

		void setTextToReFont(const juce::Array<std::pair<juce::String, int>>& textToFont)
		{
			textToReFont = textToFont;
		}
		void addTextToReFont(std::pair<juce::String, int> textToFont)
		{
			bool found = false;
			for (auto& r : textToReFont)
			{
				if (textToFont.first.compare(r.first) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				textToReFont.add(textToFont);
			}
		}

		void setColor(const NamedColorsIdentifier& description)
		{
			DescriptionColor = description;
			repaint();
		}

		void clear()
		{
			setText(juce::AttributedString());
		}



		void paint(juce::Graphics& g)override
		{

			juce::AttributedString padded = juce::AttributedString(" ");

			padded.append(text);

			auto h = getHeight();


			g.setColour(universals->colors.getColor(DescriptionColor));
			padded.setFont(f.withHeight(h * 0.9));
			padded.setJustification(juce::Justification::centred);
			padded.draw(g, getLocalBounds().toFloat());

			//drawAttributedText(g, f, padded, getLocalBounds().toFloat());
		}

		NamedColorsIdentifier& getColor()
		{
			return DescriptionColor;
		}


		class Listener : public juce::MouseListener, public GUI_Universals::Listener
		{
		public:

			Listener(HintBar* barToUse, const juce::String& description)
				:GUI_Universals::Listener(barToUse->universals)
			{
				bar = barToUse;

				text = juce::AttributedString(description);

				bar->addListener(this);
				hintBarChanged();

			}

			~Listener()
			{
				bar->removeListener(this);
			}





			const juce::AttributedString& getText()
			{
				return text;
			}

			void mouseEnter(const juce::MouseEvent& e)
			{
				bar->setText(text);
			}

			void mouseExit(const juce::MouseEvent& e)
			{
				bar->clear();
			}

			void hintBarChanged()
			{
				colors.clear();
				for (auto& r : bar->textToReColor)
				{

					auto stringToReplaceLen = r.first.length();

					int i = 0;
					juce::String result(text.getText());

					while ((i = result.lastIndexOf(r.first)) >= 0)
					{
						result = result.replaceSection(i, stringToReplaceLen, "");

						bool replaced = false;
						for (int j = 0; j < colors.size(); ++j)
						{
							if (colors[j].first.getStart() == i && colors[j].first.getLength() < stringToReplaceLen) // overlap in word to be replaced and this word is longer
							{
								colors.set(j, { juce::Range<int>(i,i + stringToReplaceLen),r.second });
								replaced = true;
							}
						}

						if (!replaced)
						{
							colors.add({ juce::Range<int>(i,i + stringToReplaceLen),r.second });
						}

					}

				}

				fonts.clear();
				for (auto& r : bar->textToReFont)
				{

					auto stringToReplaceLen = r.first.length();

					int i = 0;
					juce::String result(text.getText());

					while ((i = result.lastIndexOf(r.first)) >= 0)
					{
						result = result.replaceSection(i, stringToReplaceLen, "");

						bool replaced = false;
						for (int j = 0; j < fonts.size(); ++j)
						{
							if (fonts[j].first.getStart() == i && fonts[j].first.getLength() < stringToReplaceLen) // overlap in word to be replaced and this word is longer
							{
								fonts.set(j, { juce::Range<int>(i,i + stringToReplaceLen),r.second });
								replaced = true;
							}
						}

						if (!replaced)
						{
							fonts.add({ juce::Range<int>(i,i + stringToReplaceLen),r.second });
						}

					}

				}


				GUI_UniversalsChanged();
			}


		private:


			// Inherited via Listener

			void GUI_UniversalsChanged()
			{
				text.setColour(universalsPTR->colors.getColor(bar->getColor()));
				text.setFont(bar->getFont());
				for (auto& c : colors)
				{
					text.setColour(c.first, universalsPTR->colors.getColor(c.second));
				}

				for (auto& f : fonts)
				{
					text.setFont(f.first, universalsPTR->Fonts[f.second].getFont());
				}

			}


			juce::AttributedString text;
			HintBar* bar;
			juce::Array<std::pair<juce::Range<int>, NamedColorsIdentifier>> colors;
			juce::Array<std::pair<juce::Range<int>, int>> fonts;
		};

		void addListener(Listener* listener)
		{
			listeners.add(listener);
		}
		void removeListener(Listener* listener)
		{
			listeners.removeFirstMatchingValue(listener);
		}

		juce::Array<std::pair<juce::String, NamedColorsIdentifier>> textToReColor;
		juce::Array<std::pair<juce::String, int>>textToReFont;
		GUI_Universals* universals;
	private:
		juce::AttributedString text;
		juce::Font f;
		NamedColorsIdentifier DescriptionColor;

		juce::Array<Listener*> listeners;


		void pushChangeToListeners()
		{
			for (auto l : listeners)
			{
				l->hintBarChanged();
			}
		}
	};



}// namnepace bdsp

