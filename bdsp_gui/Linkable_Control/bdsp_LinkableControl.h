#pragma once



namespace bdsp
{
	class LinkableControl
	{
	public:
		class SampleListener;

		LinkableControl() = default;
		virtual ~LinkableControl() = default;

		virtual float update() = 0;
		void pushToListeners();

		const juce::Identifier& getID();
		void setID(const juce::Identifier& newID)
		{
			ID = newID;
		}

		int getIndex()
		{
			return index;
		}


		float value = 0;

		bool getPolarity() // true == bipolar
		{
			return polarity;
		}

		void addSampleListener(SampleListener* listenerToAdd);
		void removeSampleListener(SampleListener* listenerToRemove);

		class SampleListener
		{
		public:
			SampleListener(LinkableControl* p)
			{
				parent = p;
				parent->addSampleListener(this);
			}
			virtual ~SampleListener()
			{
				parent->removeSampleListener(this);
			}
			virtual void pushNewSample(float smp) // cannot be pure virtual b/c of opengl stuff
			{
			}

		private:
			LinkableControl* parent;
		};
		juce::Array<SampleListener*>& getListeners();

	protected:


		juce::Array<SampleListener*> listeners;
		bool polarity = false; // true == bipolar
		juce::Identifier ID;

		int index;

	private:

		JUCE_LEAK_DETECTOR(LinkableControl)
			JUCE_DECLARE_WEAK_REFERENCEABLE(LinkableControl)
	};

	class LinkableControlComponent : public juce::DragAndDropContainer // outputs values between -1 and 1
	{
	public:
		LinkableControlComponent(GUI_Universals* universalsToUse, int idx)
			:dragComp(universalsToUse, this),
			transferComp(universalsToUse, this)
		{
			index = idx;

		}
		virtual ~LinkableControlComponent() = default;

		virtual void setControlObject(LinkableControl* newControlObject);

		virtual void setName(const juce::String& title) = 0;

		juce::String getDisplayName();

		void setColor(const NamedColorsIdentifier& color, const NamedColorsIdentifier& caretColor);
		virtual void colorSet() = 0;
		NamedColorsIdentifier getColorID();

		const juce::Identifier& getID();
		void setID(const juce::Identifier& newID)
		{
			ID = newID;
		}

		int getIndex()
		{
			return index;
		}


		void addLink(juce::RangedAudioParameter* param)
		{
			links.addIfNotAlreadyThere(param);
			transferComp.setEnabled(true);
		}

		void removeLink(juce::RangedAudioParameter* param)
		{
			links.removeAllInstancesOf(param);
			transferComp.setEnabled(!links.isEmpty());
		}

		virtual juce::Array<juce::WeakReference<HoverableComponent>> getComponentsThatShareHover() = 0;

		juce::String name, defaultName;
		LinkableControl* control = nullptr;
		juce::Array<juce::RangedAudioParameter*> links;

		class colorListener
		{
		public:
			colorListener()
			{

			}
			virtual ~colorListener()
			{
			}

			virtual void colorChanged(LinkableControlComponent* controlThatChanged) = 0;
		};

		void addColorListener(colorListener* listenerToAdd)
		{
			colorListeners.addIfNotAlreadyThere(listenerToAdd);
			listenerToAdd->colorChanged(this);
		}

		void removeColorListener(colorListener* listenerToRemove)
		{
			colorListeners.removeAllInstancesOf(listenerToRemove);
		}
	protected:
		juce::String displayName; // what to write for inside of slider influence and drag comps

		NamedColorsIdentifier c, caret;

		juce::Identifier ID;

		int index;
		juce::Array<colorListener*> colorListeners;


	public:
		class DragComp : public Component, public juce::DragAndDropContainer
		{
		public:
			DragComp(GUI_Universals* universalsToUse, LinkableControlComponent* p);
			~DragComp() = default;

			void mouseEnter(const juce::MouseEvent& e) override;
			void mouseExit(const juce::MouseEvent& e) override;
			void mouseDrag(const juce::MouseEvent& e) override;

			void resized() override;


			/** Subclasses can override this to be told when a drag starts. */
			void dragOperationStarted(const juce::DragAndDropTarget::SourceDetails&) override;

			/** Subclasses can override this to be told when a drag finishes. */
			void dragOperationEnded(const juce::DragAndDropTarget::SourceDetails&) override;

			void paint(juce::Graphics& g) override;

			LinkableControlComponent* parent = nullptr;
		private:
			;

		} dragComp;

		class TransferComp : public Button
		{
		public:
			TransferComp(GUI_Universals* universalsToUse, LinkableControlComponent* p);

			// Inherited via Button
			void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
			void enablementChanged() override;
		private:
			LinkableControlComponent* parent = nullptr;
		}transferComp;

	};

	class ControlTransferPopupMenu : public PopupMenu
	{
	public:
		ControlTransferPopupMenu(GUI_Universals* universalsToUse);

		void createPopup(LinkableControlComponent* source);



	private:

		LinkableControlComponent* sourceComponent = nullptr;

	};



}// namnepace bdsp
