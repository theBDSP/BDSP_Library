//#pragma once
//
//
//namespace bdsp
//{
//	class ColorSpaceChangeBroadcaster : public juce::ChangeBroadcaster
//	{
//	public:
//
//		float getHue();
//		float getSat();
//		float getBri();
//		virtual juce::Colour getHSBColor();
//
//	protected:
//		float hue = 0, sat = 0, bri = 0; // for when sat can be ambiguous @ 0 brightness
//	};
//
//	class ColorSpaceViewer : public Component, public ColorSpaceChangeBroadcaster, public juce::ChangeListener
//	{
//	public:
//
//		ColorSpaceViewer(GUI_Universals* universalsToUse);
//		~ColorSpaceViewer() = default;
//		void setColor(float h, float s, float b);
//
//		void resized() override;
//		void paint(juce::Graphics& g) override;
//
//
//		void mouseDown(const juce::MouseEvent& e) override;
//		void mouseDrag(const juce::MouseEvent& e) override;
//
//		 = nullptr;
//	private:
//
//		// Inherited via ChangeListener
//		void changeListenerCallback(juce::ChangeBroadcaster* source) override; // change from scheme editor, value editors, etc
//
//		class CrossHair : public Component, public juce::ChangeListener
//		{
//		public:
//
//			CrossHair(ColorSpaceViewer* parent);
//			~CrossHair() = default;
//			void update(const juce::Colour& c, float x, float y); // updates position and color
//
//			void paint(juce::Graphics& g) override;
//
//		private:
//			// Inherited via ChangeListener
//			void changeListenerCallback(juce::ChangeBroadcaster* source) override;
//
//			juce::Colour color;
//			ColorSpaceViewer* p = nullptr;
//		};
//		std::unique_ptr<CrossHair> crossHair;
//
//
//	};
//
//	class HueSlider : public Component, public juce::ChangeListener
//	{
//	public:
//		HueSlider(ColorSpaceViewer* viewer);
//
//		void paint(juce::Graphics& g) override;
//		void resized() override;
//
//		juce::Slider* getSlider();
//
//
//	private:
//		ColorSpaceViewer* linkedViewer = nullptr;
//		juce::Slider slider;
//
//		// Inherited via ChangeListener
//		void changeListenerCallback(juce::ChangeBroadcaster* source) override; // when color space gets changed
//	};
//
//	class ColorValueSlider :  public Component, public ColorSpaceChangeBroadcaster, public juce::ChangeListener
//	{
//	public:
//		ColorValueSlider(ColorSpaceViewer* viewer, colorChannel type);
//		juce::Colour getHSBColor() override;
//		void paint(juce::Graphics& g) override;
//		void resized() override;
//
//
//	private:
//		ColorSpaceViewer* linkedViewer = nullptr;
//		HueSlider* hueSlider = nullptr;
//		colorChannel channel;
//		class NoDefaultNumberSlider : public NumberSlider
//		{
//		public:
//			NoDefaultNumberSlider(GUI_Universals* universalsToUse);
//
//		void mouseDoubleClick(const juce::MouseEvent& e) override;
//		}slider;
//
//		// Inherited via ChangeListener
//		void changeListenerCallback(juce::ChangeBroadcaster* source) override;
//	};
//
//	class HexLabel : public Component, public juce::ChangeListener
//	{
//	public:
//
//		HexLabel(ColorSpaceViewer* viewer);
//		~HexLabel();
//
//		void paint(juce::Graphics& g) override;
//		void resized() override;
//		// Inherited via ChangeListener
//		void changeListenerCallback(juce::ChangeBroadcaster* source) override; // update label on color change
//	private:
//		ColorSpaceViewer* linkedViewer = nullptr;
//		CenteredEditableLabel label;
//
//	};
//
//	class ColorSelector : public Component
//	{
//	public:
//		ColorSelector(GUI_Universals* universalsToUse);
//
//		void resized() override;
//		void paint(juce::Graphics& g) override;
//
//		ColorSpaceViewer* getColorSpace();
//
//	private:
//		std::unique_ptr<ColorSpaceViewer> colorSpace;
//		std::unique_ptr<HueSlider> hueSlider;
//		std::unique_ptr<ColorValueSlider> H, S, V, R, G, B;
//		std::unique_ptr<HexLabel> Hex;
//	};
//
//} // namespace bdsp