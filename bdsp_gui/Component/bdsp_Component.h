#pragma once


namespace bdsp
{

	class BaseSlider;

	/**
	 * A class that wrapsa compnent class and forwards all of its mouse events to its parent.
	 * @tparam CompType The component class to wrap
	 */
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
		You will probably never use this class alone without it being the base class of ComponentCore.
	*/
	class HoverableComponent
	{
	public:
		HoverableComponent(juce::Component* compToExtend);
		virtual ~HoverableComponent();

		/**
		 * @return The juce component this object extends
		 */
		juce::Component* getComponent();

		/**
		 * Sets a new lambda function to be called when this component's hovering state is changed 
		 */
		void setHoverFunc(std::function<void(bool)>& func);

		/**
		 * @return The current lambda function that gets called when this component's hovering state is changed
		 */
		std::function<void(bool)>& getHoverFunc();

		/**
		 * Sets how long the mouse must remain inside this component to trigger a hovering state change
		 * @param ms The time in milliseconds
		 */
		void setHoverTime(int ms);
		
		/**
		 * @return  How long the mouse must remain inside this component to trigger a hovering state change
		 */
		int getHoverTime();


		/**
		 * @return True if this component (or one of its hover partners) is currently being hovered over
		 */
		bool isHovering();

		/**
		 * Directly sets the hovering state of this component
		 * @param state The hovering state to set
		 * @param notifyListeners True if this component's hover listeners should be notified of this change - defaults to true
		 * @param notifyPartners True if this component's hover partners should be notified of this change - defaults to true
		 * @param excludedPartner A hover partner that should not be notified of this change - defaults to nullptr
		 */
		void setHovering(bool state, bool notifyListeners = true, bool notifyPartners = true, HoverableComponent* excludedPartner = nullptr);
	
		/**
		 * Checks if a different hoverable component is a hover partner of this one 
		 */
		bool isHoverPartner(HoverableComponent* compToCheck);

		/**
		 * Listens for changes to one or more HoverableComponent's hovering state and calls a lambda when it hears a change 
		 */
		class Listener
		{
		public:
			Listener() = default;
			~Listener() = default;

			std::function<void(HoverableComponent*, bool)> onHoveringStateChanged;
		private:
			JUCE_DECLARE_WEAK_REFERENCEABLE(Listener)
		};

		//================================================================================================================================================================================================
		void addHoverListener(Listener* listenerToAdd);
		void removeHoverListener(Listener* listenerToRemove);
		void removeAllHoverListeners();
		//================================================================================================================================================================================================

		/**
		 * Adds another HoverableComponent to this component's list of hover partners. After this both components will be notified when the other's hovering state has changed (unless excluded in the setHovering call)
		 * @param otherComp The component to add as a hover partner
		 * @param initialAdd If true, this component will also be added to the other component's list of hover partners - defaults to true
		 */
		void addHoverPartner(HoverableComponent* otherComp, bool initialAdd = true);


		/**
		 * Adds a set of HoverableComponents to this component's list of hover partners. After this all provided components will be notified when the any other provided component's hovering state has changed (unless excluded in the setHovering call)
		 * @param otherComp The components to add as hover partners
		 * @param initialAdd If true, this component will also be added to the other components' list of hover partners - defaults to true
		 */
		void addHoverPartners(const juce::Array<juce::WeakReference<HoverableComponent>>& otherComps, bool initialAdd = true);


		/**
		 * Removes another HoverableComponent from this component's list of hover partners
		 * @param otherComp The component to remove as a hover partner
		 * @param removeFromAllOtherPartners If true, removes the provided component from all the remaining hover partners' list of hover partners
		 */
		void removeHoverPartner(HoverableComponent* otherComp, bool removeFromAllOtherPartners = true);
		
		/**
		 * Removes this component from all its hover partners list of hover partners, and clears it's own list of hover partners 
		 */
		void removeAllHoverPartners();
	private:

		/**
		 *  Internal class that manages the hovering state of a HoverableComponent. Also manages the timing of triggering a hovering state change.
		 */
		class HoverTimer : public juce::Timer, public juce::MouseListener
		{
		public:
			HoverTimer(HoverableComponent* parent);
			virtual ~HoverTimer();


			void timerCallback() override;


			/**
			 * Sets how long the mouse must remain inside the parent component to trigger a hovering state change
			 * @param ms The time in milliseconds
			 */
			void setTime(int ms);
			int getTime();


			/**
			 * @return True if the parent component is currently being hovered over
			 */
			bool isHovering();

			/**
			 * Directly sets the hovering state of the parent component
			 * @param state The hovering state to set
			 * @param notifyListeners True if the parent component's hover listeners should be notified of this change - defaults to true
			 * @param notifyPartners True if the parent component's hover partners should be notified of this change - defaults to true
			 * @param excludedPartner A hover partner that should not be notified of this change - defaults to nullptr
			 */
			void setHovering(bool state, bool notifyListeners = true, bool notifyPartners = true, HoverableComponent* excludedPartner = nullptr);

			void mouseEnter(const juce::MouseEvent& e) override;
			void mouseExit(const juce::MouseEvent& e) override;



			std::function<void(bool)> hoverFunc;

		private:

			/**
			 * Notifys the parent component's hover listeners of its new hover state.
			 */
			void notifyParentsListeners();

			/**
			 * Notifys the parent component's hover partners of its new hover state. Optionally excluding a partner from recieving this notification.
			 */
			void notifyParentsPartners(HoverableComponent* excludedPartner = nullptr);

			int onDelayMS = 250, offDelayMS = 100;
			bool hovering = false;
			HoverableComponent* p; // the parent component

			JUCE_LEAK_DETECTOR(HoverTimer)

		};

		HoverTimer t;
		juce::Array<juce::WeakReference<Listener>> listeners;
		juce::Array<juce::WeakReference<HoverableComponent>> hoverPartners;
	protected:
		juce::Component* extendedComponent;
		JUCE_DECLARE_WEAK_REFERENCEABLE(HoverableComponent)
	};


	/**
	 *  Extends any juce::Component (or derived class of juce::Component) with handy functions to manage BDSP specific things
	 */
	class ComponentCore : public HoverableComponent, public HoverableComponent::Listener
	{
	public:

		ComponentCore(juce::Component* componentToExtend, GUI_Universals* universalsToUse);
		virtual ~ComponentCore();


		/**
		 * @return The extended component's bounds relative to the plugin's global bounds (the desktop manager will always be the exact size of the plugin)
		 */
		juce::Rectangle<int> getBoundsRelativeToDesktopManager();

		/**
		 * Restricts the extended component's bounds to a provided aspect ratio
		 * @param aspectRatio The aspect ratio to restrict the bounds to 
		 * @param position Where to place the restricted bounds in the original extended ocmponent's bounds - defaults to the center
		 */
		void forceAspectRatio(double aspectRatio, juce::RectanglePlacement position = juce::RectanglePlacement::centred);


		/**
		 * Calls for the extended component to repaint itself in a thread safe manner.
		 * Call this instead of repaint() when there's a possibility this will get called outside of message thread.
		 */
		void repaintThreadChecked();



		/**
		 * @return The attributed string that will be shown when the extended component gets hovered over
		 */
		const juce::AttributedString& getHintText();

		/**
		 * Sets the text that will be shown when the extended component gets hovered over and automatically applies any global formatting (colors, fonts) to applicable spans of text
		 */
		void setHintBarText(const juce::String& text);

		virtual void setFontIndex(int idx);
		int getFontIndex();


		/**
		 * @return An actual color value from a named color
		 */
		juce::Colour getColor(const NamedColorsIdentifier& name);

		/**
		 * @param index The index of the font to get -  defaults to -1 (gets the currently set font index)
		 * @return An actual font from a given font index
		 */
		juce::Font getFont(int index = -1);


		GUI_Universals* universals; // a global pool of values, colors, fonts, etc.

	protected:
		int fontIndex = 0;

	private:
		std::unique_ptr<HintBar::Listener> hintListener; 
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