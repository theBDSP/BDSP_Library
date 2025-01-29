#pragma once



namespace bdsp
{
#define vSuffix "Val"



	class RangedSlider : public AnimatedComponent, public RangedComponentCore, public juce::AsyncUpdater, public juce::DragAndDropTarget, public juce::AudioProcessorParameter::Listener
	{
	public:
		RangedSlider(GUI_Universals* universalsToUse);
		~RangedSlider();
		void swap(RangedSlider* other);

		void setParent(BaseSlider* p, bool setLookAndFeel = true);


		juce::String getID();



		BaseSlider* getParent();

		ControlParameter* getControlParamter();






		void setActiveControl(int i) override;





		double getConstrainedProportion(double val);



		bool isLinked() override;

		bool isLinkedToControl(int idx) override;

		bool isLinkedToAnimatedControl() override;



		//void visibilityChanged() override;

		bool hitTest(int x, int y) override;

		void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;


		virtual ComponentCore* getHoverMenuAnchor();
		int ActiveIndex = -1;

		BaseSlider* parent = nullptr;

		juce::OwnedArray<RangedSliderInfluence> influences;

	protected:

		;

	public:


		class InfluenceDisplay : public juce::Component, public ComponentCore
		{
		public:
			InfluenceDisplay(RangedSlider* parent, int idx);
			~InfluenceDisplay() = default;

			void paint(juce::Graphics& g) override;

			void paintContainerDisplay(juce::Graphics& g, double influenceValue);
			void paintLinearDisplay(juce::Graphics& g, double influenceValue);
			void paintCircularDisplay(juce::Graphics& g, double influenceValue);

			void visibilityChanged() override;

		private:
			RangedSlider* p;
			RangedSliderInfluence* inf;
		};
		juce::OwnedArray<InfluenceDisplay> displays;


		class InfluenceHoverMenu : public DesktopComponent
		{
		public:
			InfluenceHoverMenu(RangedSlider* sliderToAttachTo);
			~InfluenceHoverMenu() = default;

			void addInfluence(RangedSliderInfluence* influenceToAdd);
			void removeInfluence(RangedSliderInfluence* influenceToRemove);
			void resized() override;

			void resizeMenu();

			void paint(juce::Graphics& g) override;

			void disableAllInfluences();
			void enableAllInfluences();

			bool canShow();
			bool canHide();
			bool isEmpty();




			bdsp::Component container;


			juce::Rectangle<float> bounds;

			std::function<bool()> canShowLambda;
			std::function<bool()> canHideLambda;
		private:

			juce::Point<float> pointerLocation;
			int numInfluences = 0;

			RangedSlider* parent;
			juce::Array<RangedSliderInfluence*> influences;
		};
		std::unique_ptr<InfluenceHoverMenu> hoverMenu;

		ControlParameter* controlParam = nullptr;
	private:


		bool parentSet = false;











		// Inherited via AsyncUpdater
		void handleAsyncUpdate() override;








		// Inherited via DragAndDropTarget
		bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;

		void itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;

		void itemDragEnter(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;

		void itemDragExit(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;





		// Inherited via Listener (Macro Parameters)
		void parameterValueChanged(int parameterIndex, float newValue) override;

		void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;


		// Inherited via AnimatedComponent
		void update() override;


		private:

			// Inherited via AnimatedComponent
			void valueChanged(juce::Value& value) override;

			// Inherited via RangedComponentCore
			ComponentCore* getComponent() override;
};



}// namnepace bdsp
