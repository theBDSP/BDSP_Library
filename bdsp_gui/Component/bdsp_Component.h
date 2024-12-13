#pragma once


namespace bdsp
{

	class BaseSlider;
	class LNFuniversals;

	template <class CompType>
	class MousePassthrough : public CompType
	{
	public:
		template<class ... Types>
		MousePassthrough(juce::Component* parent, Types...args)
			:CompType(args...)
		{
			jassert(parent != this);
			p = parent;
		}

		void mouseMove(const juce::MouseEvent& e) override
		{
			p->mouseMove(e.getEventRelativeTo(p));
		}
		void mouseEnter(const juce::MouseEvent& e) override
		{
			p->mouseEnter(e.getEventRelativeTo(p));
		}
		void mouseExit(const juce::MouseEvent& e) override
		{
			p->mouseExit(e.getEventRelativeTo(p));
		}
		void mouseDown(const juce::MouseEvent& e) override
		{
			p->mouseDown(e.getEventRelativeTo(p));
		}
		void mouseUp(const juce::MouseEvent& e) override
		{
			p->mouseUp(e.getEventRelativeTo(p));
		}
		void mouseDoubleClick(const juce::MouseEvent& e) override
		{
			p->mouseDoubleClick(e.getEventRelativeTo(p));
		}
		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override
		{
			p->mouseWheelMove(e.getEventRelativeTo(p), mw);
		}
		void mouseDrag(const juce::MouseEvent& e) override
		{
			p->mouseDrag(e.getEventRelativeTo(p));
		}
		void mouseMagnify(const juce::MouseEvent& e, float scaleFactor) override
		{
			p->mouseMagnify(e.getEventRelativeTo(p), scaleFactor);
		}

	private:
		juce::Component* p;

	};

	//================================================================================================================================================================================================

	/***
		Provides logic for handling users hovering over components and sending hover notifications to other components.
		You will probably never use this class alone without it being the base class of ComponentCore
	*/
	class HoverableComponent
	{
	public:
		HoverableComponent(juce::Component* compToExtend);
		virtual ~HoverableComponent() = default;


		juce::Component* getComponent();

		void setHoverFunc(std::function<void(bool)>& func);
		std::function<void(bool)>& getHoverFunc();

		void setHoverTime(int ms);
		int getHoverTime();

		bool isHovering();
		void setHovering(bool state, bool notifyListeners = true, bool notifyPartners = true, HoverableComponent* excludedPartner = nullptr);
		bool isHoverPartner(HoverableComponent* compToCheck);

		bool isMouseWithin();

		class Listener
		{
		public:
			Listener() = default;
			~Listener() = default;

			std::function<void(HoverableComponent*, bool)> onHoveringStateChanged; // like a virtual funtion except it is settable while running (add hover partner)
		private:
			JUCE_DECLARE_WEAK_REFERENCEABLE(Listener)
		};


		void addHoverListener(Listener* listenerToAdd);
		void removeHoverListener(Listener* listenerToRemove);
		void removeAllHoverListeners();

		void addHoverPartner(HoverableComponent* otherComp, bool initialAdd = true); // sets both comps as hover listeners of each other that set their own hover in response to the other
		void addHoverPartners(const juce::Array<juce::WeakReference<HoverableComponent>>& otherComps, bool initialAdd = true);

		void removeHoverPartner(HoverableComponent* otherComp, bool removeFromAllOtherPartners = true);
		void removeAllHoverPartners();
	private:
		class HoverTimer : public juce::Timer, public juce::MouseListener
		{
		public:
			HoverTimer(HoverableComponent* parent);
			virtual ~HoverTimer() = default;


			void timerCallback() override;

			void setTime(int ms);
			int getTime();

			bool isHovering();
			void setHovering(bool state, bool notifyListeners = true, bool notifyPartners = true, HoverableComponent* excludedPartner = nullptr);
			void notifyParentsListeners();
			void notifyParentsPartners(HoverableComponent* excludedPartner = nullptr);

			void mouseEnter(const juce::MouseEvent& e) override;
			void mouseExit(const juce::MouseEvent& e) override;

			bool isMouseWithin();


			std::function<void(bool)> hoverFunc;

		private:
			int onDelayMS = 250, offDelayMS = 100;
			bool hovering = false;
			HoverableComponent* p;

			bool mouseWithin = false;

			JUCE_LEAK_DETECTOR(HoverTimer)

		};

		HoverTimer t;
		juce::Array<juce::WeakReference<Listener>> listeners;
		juce::Array<juce::WeakReference<HoverableComponent>> hoverPartners;
	protected:
		juce::Component* extendedComponent;
		JUCE_DECLARE_WEAK_REFERENCEABLE(HoverableComponent)
	};

	class ComponentCore : public HoverableComponent, public HoverableComponent::Listener
	{
	public:

		enum RangeOverflow { Contained, Underflow, Overflow };
		ComponentCore(juce::Component* componentToExtend, GUI_Universals* universalsToUse);
		virtual ~ComponentCore();



		juce::Rectangle<int> getBoundsRelativeToDesktopManager();
		void forceAspectRatio(double aspectRatio, juce::RectanglePlacement position = juce::RectanglePlacement::centred);

		void paintLinearSlider(juce::Graphics& g, BaseSlider* s, const juce::Rectangle<int>& bounds, bool paintValue = true, bool paintKnob = true, bool paintValuePtr = true, bool paintTrack = true);
		void paintCircularSlider(juce::Graphics& g, BaseSlider* s, const juce::Rectangle<int>& bounds, bool paintValue = true, bool paintKnob = true, bool paintActualValue = true, bool paintTrack = true);

		void repaintThreadChecked(); // call when there's a possibility this will get called outside of message thread




		const juce::AttributedString& getHintText();
		void setHintBarText(const juce::String& text);

		virtual void setFontIndex(int idx);
		int getFontIndex();

		juce::Colour getColor(const NamedColorsIdentifier& name);
		juce::Font getFont(int index = -1);

		virtual void clearLookAndFeels();
		virtual void resetLookAndFeels(GUI_Universals* universalsToUse);






	private:



		std::unique_ptr<HintBar::Listener> hintListener;


	protected:
		int fontIndex = 0;


	public:
		GUI_Universals* universals;


	};

	class Component : public juce::Component, public ComponentCore
	{
	public:
		Component(GUI_Universals* universalsToUse)
			:ComponentCore(this, universalsToUse)
		{

		}
		virtual ~Component() = default;

		void enablementChanged() override
		{
			repaint();
		}
	};


} // namespace bdsp