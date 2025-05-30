#pragma once


namespace bdsp
{
	class RearrangableComponentManagerBase : public Component
	{
	public:
		RearrangableComponentManagerBase(GUI_Universals* universalsToUse);

		void setDragHandleText(const juce::String& s);
		void setDragHandlePath(const juce::Path& p);
		void setDragHandleColors(const NamedColorsIdentifier& normalColor, const NamedColorsIdentifier& downColor, const NamedColorsIdentifier& slotColor);


		void setSingleDragHandleText(int idx, const juce::String& s);
		void setSingleDragHandlePath(int idx, const juce::Path& p);
		void setSingleDragHandleColors(int idx, const NamedColorsIdentifier& normalColor, const NamedColorsIdentifier& downColor, const NamedColorsIdentifier& slotColor);


		void setDragHandleTexts(juce::StringArray strings);
		void setDragHandlePaths(juce::Array<juce::Path> paths);
		void setDragHandleColors(juce::Array< NamedColorsIdentifier> normalColors, juce::Array< NamedColorsIdentifier> downColors, juce::Array< NamedColorsIdentifier> slotColors);

		int getPositionOfDragBox(int dragBoxIndex);

		void attach(OrderedListParameter* param);
		class DragBox : public BasicContainerComponent
		{
		public:
			DragBox(RearrangableComponentManagerBase* parent, int index);

			void paint(juce::Graphics& g) override;

			int getIndex();
			class Dragger : public juce::ComponentDragger, public Component
			{

			public:
				Dragger(DragBox* parent);


				void mouseEnter(const juce::MouseEvent& e) override;
				void mouseExit(const juce::MouseEvent& e) override;
				void mouseDown(const juce::MouseEvent& e) override;
				void mouseDrag(const juce::MouseEvent& e) override;
				void mouseUp(const juce::MouseEvent& e) override;


				void paint(juce::Graphics& g) override;
				void resized() override;


				bool isDragging = false;


				juce::Path dragHandlePath;
				juce::String dragText;
				NamedColorsIdentifier dragColor, dragColorDown;
			protected:
				DragBox* p = nullptr;
			}dragger;

			RearrangableComponentManagerBase* p = nullptr;
			int idx;
		};

		class Listener
		{
		public:
			virtual void componentOrderChanged(RearrangableComponentManagerBase* rearrangableCompThatChanged) = 0;
		};

		void addListener(Listener* listenerToAdd);
		void removeListener(Listener* listenerToRemove);

		virtual void addComponent(juce::Component* newComp);

		void reorderComponents(const juce::Array<int>& newOrder, bool notifyListeners = true);
		void reorderComponents(DragBox* boxMoved, int indexMovedTo, bool notifyListeners = true);
		virtual void reorderComponents(int indexMoved, int indexMovedTo, bool notifyListeners = true);

		juce::Component* getComp(int idx) const;

		int getNumComps() const;

		juce::Array<int> getOrder();
		void calculateLongestTitle();

		std::function<void(int, int)> onComponentOrderChanged;
		std::function<void(int, juce::Point<int>)> onDrag;
		std::function<void(juce::Graphics&, DragBox*)> dragBoxPaintFunc;

	protected:
		class DragConstrainer : public juce::ComponentBoundsConstrainer
		{
		public:
			DragConstrainer(RearrangableComponentManagerBase* parent)
			{
				p = parent;
			}


			void checkBounds(juce::Rectangle<int>& bounds, const juce::Rectangle<int>& previousBounds, const juce::Rectangle<int>& limits, bool isStretchingTop, bool isStretchingLeft, bool isStretchingBottom, bool isStretchingRight)
			{
				if (p->horizontal)
				{
					auto y = p->dragHandleBounds.getY();

					bounds.setY(y);
					bounds.setX(juce::jlimit((int)p->dragHandleBounds.getX(), (int)p->dragHandleBounds.getRight() - bounds.getWidth(), bounds.getX()));
				}
				else
				{
					auto x = p->dragHandleBounds.getX();

					bounds.setX(x);
					bounds.setY(juce::jlimit((int)p->dragHandleBounds.getY(), (int)p->dragHandleBounds.getBottom() - bounds.getHeight(), bounds.getY()));
				}
			}
		private:
			RearrangableComponentManagerBase* p = nullptr;
		}dragBoundsConstrainer;

	public:
		juce::String longestTitle;
		juce::OwnedArray<DragBox> dragBoxes;
		juce::Array<DragBox*> orderedDragBoxes;
		bool horizontal = true;
		juce::Rectangle<float> dragHandleBounds;
		juce::OwnedArray<juce::Component> comps;
		juce::OwnedArray<Component> draggerSlots;
		bool instantSwap = true;
		juce::Array<int> baselineDragOrder; // which slot each dragger controls before any dragging occurs, only used for non-instant swaps

	private:
		juce::ListenerList<Listener> listeners;

		class RearrangableComponentParameterAttachment : public Listener
		{
		public:
			RearrangableComponentParameterAttachment(OrderedListParameter& parameter, RearrangableComponentManagerBase& comp, juce::UndoManager* undoManager = nullptr);

			void componentOrderChanged(RearrangableComponentManagerBase* rearrangableCompThatChanged) override;

			class OrderedListParameterAttachment : public OrderedListParameter::Listener, public juce::AsyncUpdater
			{
			public:

				OrderedListParameterAttachment(OrderedListParameter& parameter, RearrangableComponentManagerBase& comp, juce::UndoManager* undoManager = nullptr);

				~OrderedListParameterAttachment();

				void sendInitialUpdate();

				void setValue(juce::Array<int> newOrder);
			private:
				RearrangableComponentManagerBase& rearrangeComp;
				OrderedListParameter& storedParameter;
				juce::UndoManager* um;

				// Inherited via Listener
				void orderChanged(const juce::Array<int> newOrder) override;

				// Inherited via AsyncUpdater
				void handleAsyncUpdate() override;

			} parameterAttachment;
			OrderedListParameter& listParameter;
			RearrangableComponentManagerBase& listComp;
		};

		std::unique_ptr<RearrangableComponentParameterAttachment> attachment;
	};

	// rearrangeable comps that dont move themselves but swap children and contents
	class StationaryRearrangableComponentManager : public RearrangableComponentManagerBase
	{
	public:
		StationaryRearrangableComponentManager(GUI_Universals* universalsToUse);

		virtual ~StationaryRearrangableComponentManager() override;

		void resized() override;
		void addComponent(juce::Component* newComp) override;



		void setDragHandleBoundsRelativeToComp(juce::Rectangle<float> newBounds);

		void reorderComponents(int indexMoved, int indexMovedTo, bool notifyListeners) override;

		float borderRatio = 0.0125;
	protected:

		juce::Rectangle<float> dragHandleBoundsRelativeToComp;





		juce::Viewport vp;
		juce::Component vpComp;
	};




} // namespace bdsp