#pragma once



namespace bdsp
{
	class PreRenderedComponent : public Component, public GUI_Universals::Listener
	{
	public:
		PreRenderedComponent(GUI_Universals* universalsToUse);
		virtual void init(const juce::Rectangle<int>& maxBounds) = 0;
		void paint(juce::Graphics& g) override;

	protected:
		juce::Image bufferedImage;
	};


	//currently unused
	//class ProtrudingContainerComponent : public PreRenderedComponent
	//{

	//public:
	//	ProtrudingContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& highlightColor, const NamedColorsIdentifier& borderColor, float borderThickness, float highlightThickness, bool isBumpOut = true, bool respondsToUniversalChanges = false);

	//	void init(const juce::Rectangle<int>& maxBounds) override;

	//protected:

	//	// Inherited via PreRenderedComponent
	//	void GUI_UniversalsChanged() override;

	//	
	//	void paint(juce::Graphics& g) override;

	//	bool universalListener, bumpOut;
	//	NamedColorsIdentifier background, highlight, border;
	//	float borderThicc, highlightThicc;
	//};


	class BeveledContainerComponent : public Component
	{
	public:
		BeveledContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& borderColor, float innerBorderRatio, float outerBorderRatio); // ratios are relative to rounded rectangle curve
		void paint(juce::Graphics& g) override;

	private:

		float innerRatio, outterRatio;

		NamedColorsIdentifier background, border;


	};

	class BasicContainerComponent : public Component
	{
	public:
		BasicContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& backgroundColorGradient = NamedColorsIdentifier());
		void paint(juce::Graphics& g) override;

		void setColors(const NamedColorsIdentifier& backgroundColor, const NamedColorsIdentifier& backgroundColorGradient = NamedColorsIdentifier());

	protected:
		NamedColorsIdentifier background, background2;


	};

	class TexturedContainerComponent : public BasicContainerComponent
	{
	public:
		TexturedContainerComponent(GUI_Universals* universalsToUse, const NamedColorsIdentifier& backgroundColor, const  NamedColorsIdentifier& backgroundColorGradient = NamedColorsIdentifier(), bool rotate90 = false);
		void paint(juce::Graphics& g) override;
		void paintTexture(juce::Graphics& g);

	private:
		bool rotate;
	};
} // namespace bdsp
