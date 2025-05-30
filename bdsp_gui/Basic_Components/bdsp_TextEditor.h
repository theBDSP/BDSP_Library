#pragma once
namespace bdsp
{

	class TextEditor : public juce::TextEditor, public ComponentCore, public GUI_Universals::Listener
	{


	public:
		template <class ... Types>
		TextEditor(GUI_Universals* universalsToUse, Types...args)
			:juce::TextEditor(args...),
			ComponentCore(this, universalsToUse),
			GUI_Universals::Listener(universalsToUse)
		{
			setPopupMenuEnabled(false);
		}
		~TextEditor()
		{
			setLookAndFeel(nullptr);
		}

		void setMaxText(const juce::String& newMax)
		{
			maxText = newMax;
		}

		juce::String getMaxText()
		{
			return maxText;
		}

		void focusGained(juce::Component::FocusChangeType cause) override
		{
			if (onFocusGained.operator bool())
			{
				onFocusGained();
			}
		}


		bool keyPressed(const juce::KeyPress& key) override
		{
			if (key.isKeyCode(key.escapeKey))
			{
				giveAwayKeyboardFocus();
				return true;
			}
			else
			{
				return juce::TextEditor::keyPressed(key);
			}
		}

		void drawContent(juce::Graphics& g)
		{
			auto font = universals->Fonts[getFontIndex()].getFont().withHeight(getHeight() * 0.9);
			auto selectedRange = getHighlightedRegion();
			juce::AttributedString text;
			text.append(getTextInRange(juce::Range<int>(0, selectedRange.getStart())), font, findColour(juce::TextEditor::textColourId));
			text.append(getTextInRange(selectedRange), font, findColour(juce::TextEditor::highlightedTextColourId));
			text.append(getTextInRange(juce::Range<int>(selectedRange.getEnd(), getTotalNumChars() - 1)), font, findColour(juce::TextEditor::textColourId));

			g.setColour(findColour(juce::TextEditor::highlightColourId));
			g.fillRect(getTextBounds(selectedRange).getBounds());

			auto textBounds = juce::Rectangle<float>(getWidth(), getHeight() * 0.9);
			drawAttributedText(g, font, text, textBounds.withCentre({ textBounds.getCentreX(),getHeight() / 2.0f }));
		}

		void setColors(const NamedColorsIdentifier& newBackground, const NamedColorsIdentifier& newText, const NamedColorsIdentifier& newTextHighlight, const NamedColorsIdentifier& newHighlight)
		{
			background = newBackground;
			text = newText;
			textHighlight = newTextHighlight;
			highlight = newHighlight;
			GUI_UniversalsChanged();
		}
		std::function<void()> onFocusGained;
	private:
		juce::String maxText;

		NamedColorsIdentifier background, text, textHighlight, highlight;


		// Inherited via Listener
		void GUI_UniversalsChanged() override
		{
			setColour(juce::TextEditor::backgroundColourId, getColor(background));
			setColour(juce::TextEditor::textColourId, getColor(text));
			setColour(juce::TextEditor::highlightedTextColourId, getColor(textHighlight));
			setColour(juce::TextEditor::highlightColourId, getColor(highlight));
			setColour(juce::CaretComponent::caretColourId, getColor(text));
			repaint();
		}

	};


	// needed so the textUp doesnt shift when editing a influenceLabel
	// this is a center justified influenceLabel that needs to be a child of a influenceLabel
	class CenteredTextEditor : public TextEditor, public juce::ComponentListener, public juce::TextEditor::Listener
	{
	public:
		CenteredTextEditor(Label* parent)
			:TextEditor(parent->universals)
		{
			addComponentListener(this);
			parentLabel = parent;

			setBorder(juce::BorderSize<int>(0));
			setIndents(0, 0);

			setHasFocusOutline(false);

			setLookAndFeel(&parent->getLookAndFeel());
			parent->copyAllExplicitColoursTo(*this);
		}
		~CenteredTextEditor() = default;

		void resize()
		{
			auto newBounds = juce::Rectangle<int>(getTextWidth(), parentLabel->getHeight() * 0.99).withCentre(parentLabel->getLocalBounds().getCentre());
			setBounds(newBounds);
		}
		void textEditorTextChanged(juce::TextEditor& te) override
		{
			resize();
		}
		void componentMovedOrResized(juce::Component& c, bool moved, bool resized) override
		{
			if (resized)
			{
				resize();
			}
		}

	private:
		Label* parentLabel;
	};

	class CenteredEditableLabel : public Label
	{
	public:
		CenteredEditableLabel(GUI_Universals* universalsToUse)
			:Label(universalsToUse)
		{
			setEditable(true, true);
			setJustificationType(juce::Justification::centred);
		}
		juce::TextEditor* createEditorComponent() override
		{
			auto* ed = new CenteredTextEditor(this);
			return ed;
		}

	};

	class TextEditorVerticallyAligned : public TextEditor
	{
	public:

		TextEditorVerticallyAligned(GUI_Universals* universalsToUse, FontGuideLines top = FontGuideLines::Ascender, FontGuideLines bot = FontGuideLines::Baseline)
			:TextEditor(universalsToUse)
		{
			topAlignment = top;
			botAlignment = bot;
		}

		void resized() override
		{
			TextEditor::resized();

			float top, bot;
			auto font = universals->Fonts[getFontIndex()];
			auto h = getHeight() * 0.9;

			switch (topAlignment)
			{
			case Ascender:
				top = (1 - font.ascenderRatio) * h;
				break;
			case xHeight:
				top = (1 - font.xHeightRatio) * h;
				break;
			case CapHeight:
				top = (1 - font.capHeightRatio) * h;
				break;
			default:
				top = 0;
				break;

			}

			switch (botAlignment)
			{
			case Baseline:
				bot = (1 - font.descenderRatio) * h;
				break;
			case Descender:
				bot = h;
				break;

			default:
				bot = h;
				break;

			}

			yOffset = getHeight() / 2.0f - (top + bot) / 2.0f;
		}


	private:
		FontGuideLines topAlignment, botAlignment;
		float yOffset = 0;
	};
} // namespace bdsp
