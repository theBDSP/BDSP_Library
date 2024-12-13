#pragma once

namespace bdsp
{
	class TagMenu : public juce::Component
	{
	public:
		TagMenu(PresetManager* parent);
		~TagMenu();

		juce::StringArray getCurrentTags();
		juce::StringArray getCurrentSuperTags();

		void reFilter();

		void rebuildMenu();

		bool isFiltering();

		void calculateNewSize(juce::Rectangle<int> newBounds); // called outsisde of resized to avoid repeated calls
		void resized() override;

		void paint(juce::Graphics& g) override;

		void mouseDown(const juce::MouseEvent& e) override;

		void highlightTags(const juce::StringArray& superTagsToHighlight, const juce::StringArray& tagsToHighlight); // highlight around tags whne a single preset is seleceted
		void clearTagHighlights();
		void clearTags();

		void initMenuOpen();

		bool isExpanded();


		class TagButton : public TextButton
		{
		public:
			TagButton(GUI_Universals* universalsToUse, const juce::String& s, PresetManagerColors* pmc, bool isAdd = false);
			TagButton(const TagButton& other);
			TagButton(TagButton&& other) noexcept;
			TagButton& operator = (const TagButton& other);

			~TagButton() = default;

			float getIdealWidth(int h, const juce::String& s = juce::String());
			void resized() override;

			void paintOverChildren(juce::Graphics& g) override;


			//void paintButton(juce::Graphics& g, bool highlighted, bool down) override
			//{
			//	bool isDown = down || getToggleState();
			//	bool isOver = highlighted || isHovering();

			//	auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;


			//	auto brightness = isOver ? universals->hoverBrightness : 1.0f;

			//	//	g.setColour((isDown ? backgroundDown : backgroundUp).withMultipliedBrightness(brightness).withMultipliedAlpha(alpha));
			//	g.setColour(getHoverColor(backgroundDown, backgroundUp, isDown, isOver));
			//	g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
			//	//g.fillAll();

			//	if (!(add && te->isVisible()))
			//	{


			//		g.setColour((isDown ? textDown : textUp).withMultipliedBrightness(brightness).withMultipliedAlpha(alpha));



			//		auto textUp = getText();

			//		g.setFont(f);

			//		g.drawText(textUp, getLocalBounds(), juce::Justification::centred);
			//	}
			//}

			std::unique_ptr<TextEditor> te;
			bool highlighted = false;
		private:
			juce::Font f;
			;
			PresetManagerColors* colors;
			bool add;

		};





	private:
		juce::OwnedArray<TagButton> tags;
		juce::OwnedArray<juce::OwnedArray<TagButton>> SuperTags;
		juce::StringArray superTagTitles, currentSuperTags, currentTags, highlightedSuperTags, highlightedTags;
		PresetManager* p;

		juce::OwnedArray<Viewport> vps;
		juce::OwnedArray<juce::Component> vpViewedComps;
		juce::OwnedArray<MultiShapeButton> menuExpanderTriangles;
		juce::OwnedArray<TextButton> menuExpanders;

		int border = 0, expanderH = 0;

		int closedHeight = 0, maxOpenHeight = 0;

	};

} // namespace bdsp
