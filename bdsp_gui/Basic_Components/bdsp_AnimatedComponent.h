#pragma once


namespace bdsp
{
	// splits comp into static and animated comps, animated comp gets repainted every frame whereas static only gets repainted from repaint call
	class AnimatedComponent : public Component, public juce::Value::Listener, public juce::ComponentListener, public juce::ChangeListener
	{
	public:


        AnimatedComponent(GUI_Universals* universalsToUse, juce::ChangeBroadcaster* animationTimer)
			:Component(universalsToUse)
		{
			//universalsToUse->animationTimer.compsToUpdate.add(this);
			globalTimer = animationTimer;

		}

        virtual ~AnimatedComponent()
		{
			endAnimation();


			for (auto p : parentHierarchy)
			{
				p->removeComponentListener(this);
			}


		}


		virtual void update() = 0;
        void beginAnimation()
		{
			isAnimating = true;
			globalTimer->addChangeListener(this);
		}

        void endAnimation()
		{
			globalTimer->removeChangeListener(this);
			isAnimating = false;
		}

        void parentHierarchyChanged() override
		{
			//for (auto p : parentHierarchy)
			//{
			//	p->removeComponentListener(this);
			//}

			//parentHierarchy.clear();

			//juce::Component* p = this;
			//while ((p = p->getParentComponent()))
			//{
			//	parentHierarchy.add(p);
			//	p->addComponentListener(this);
			//}
		}

		//void AnimatedComponent::componentVisibilityChanged(juce::Component& component)	// when a higher level components visibility changes check if this needs to stop or start animation
		//{
		//	if (isAnimating && isShowing())
		//	{
		//		beginAnimation();
		//	}
		//	else
		//	{
		//		endAnimation();
		//	}
		//}


		//void AnimatedComponent::valueChanged(juce::Value& value)
		//{
		//	if (value == universals->animationTimer.frameRate)
		//	{
		//		if (onFrameRateChange.operator bool())
		//		{
		//			onFrameRateChange(value.getValue().operator double());
		//		}
		//	}
		//}
		std::function<void(double)> onFrameRateChange;

	protected:



		bool isAnimating = false;

		juce::Array<juce::Component*> parentHierarchy;
		juce::ChangeBroadcaster* globalTimer;



		// Inherited via ChangeListener
		void changeListenerCallback(juce::ChangeBroadcaster* source) override
		{
			update();
		}

	};


} // namespace bdsp
