#pragma once



namespace bdsp
{
	class DesktopComponent : public Component
	{
	public:
		DesktopComponent(GUI_Universals* universalsToUse,  bool shouldBeDraggable = false, bool needsToBeOriginalComp = true);
		~DesktopComponent() override;
		virtual void show();
		virtual void hide();





		virtual bool isMouseInside(const juce::MouseEvent& e);
		void focusLost(FocusChangeType change) override;

		void setDarken(bool state);

		void paintOverChildren(juce::Graphics& g) override;

		void setLossOfFocusClosesWindow(bool b, const juce::Array<juce::Component*>& excludedComps = juce::Array<juce::Component*>());
		bool lossOfFocusClosesWindow();

		void setDraggable(bool shouldBeDraggable, bool needsToBeOriginalComp = true);

		void setClosesOthersOnShow(bool closeOthersOnShow);



		class DragListener : public juce::MouseListener
		{
		public:
			DragListener(DesktopComponent* parent)
			{
				p = parent;
				p->addMouseListener(this, true);


			}
			void mouseDown(const juce::MouseEvent& e) override
			{
				constrainer.setMinimumOnscreenAmounts(p->getHeight(), p->getWidth(), p->getHeight(), p->getWidth());
				if ((p->needsToBeOriginalCompToDrag && e.originalComponent == p) || !p->needsToBeOriginalCompToDrag)
				{
					p->dragger.startDraggingComponent(p, e);
				}
			}

			void mouseDrag(const juce::MouseEvent& e) override
			{
				if ((p->needsToBeOriginalCompToDrag && e.originalComponent == p) || !p->needsToBeOriginalCompToDrag)
				{
					p->dragger.dragComponent(p, e, &constrainer);
				}
			}


		private:
			DesktopComponent* p;
			juce::ComponentBoundsConstrainer constrainer;
		};
		std::unique_ptr<DragListener> dragListener;
		bool needsToBeOriginalCompToDrag = true;

		std::function<void()> onShow;
		std::function<void()> onHide;
		DesktopComponentManager* manager;

	protected:

		juce::ComponentDragger dragger;
	private:
		bool darken = false;
		bool lofCloses = true;
		juce::Point<int> dragStart;

		juce::Array<juce::Component*> lofExclusion; // comps that won't hide this comp when clicked on

		bool closesOthersOnOpen = true;



		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopComponent)


	};


}// namnepace bdsp
