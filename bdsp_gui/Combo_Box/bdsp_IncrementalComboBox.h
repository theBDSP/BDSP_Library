#pragma once 



namespace bdsp
{
	template <class ComboBoxType = ComboBox>
	class IncrementalComboBox : public ComboBoxType
	{
	public:
		template <typename... Types>
		IncrementalComboBox(Types... args)
			:ComboBoxType(args...)
		{


			left = std::make_unique<IncrementButton>(this);
			right = std::make_unique<IncrementButton>(this);

			juce::Path arrow = createEquilateralTriangle(juce::Rectangle<float>(1, 1), -PI / 2);

			left->setPath(arrow);
			arrow.applyTransform(juce::AffineTransform().rotated(PI));
			right->setPath(arrow);

            ComboBoxType::addAndMakeVisible(left.get());
            ComboBoxType::addAndMakeVisible(right.get());

			left->onClick = [=]()
			{
                ComboBoxType::scrollItems(true);
			};
			right->onClick = [=]()
			{
                ComboBoxType::scrollItems(false);
			};

            ComboBoxType::setDropDownArrow(false);

		}



		void resized() override
		{
			ComboBoxType::resized();
			float s = ComboBoxType::getHeight() / 3.0;
			left->setBounds(juce::Rectangle<int>(s, s).withCentre(juce::Point<int>(s, ComboBoxType::getHeight() / 2.0)));
			right->setBounds(juce::Rectangle<int>(s, s).withCentre(juce::Point<int>(ComboBoxType::getWidth() - s, ComboBoxType::getHeight() / 2.0)));
            ComboBoxType::button->setBounds(juce::Rectangle<int>().leftTopRightBottom(left->getRight(), 0, right->getX(), ComboBoxType::getHeight()));
		}

		void updateIncrementArrows(int idx)
		{
			bool canGoLeft = false;
			bool canGoRight = false;

			for (int i = idx - 1; i >= 0; i--)
			{
				if (ComboBoxType::List->List[i]->isEnabled())
				{
					canGoLeft = true;
					break;
				}
			}

			for (int i = idx + 1; i < ComboBoxType::numItems; ++i)
			{
				if (ComboBoxType::List->List[i]->isEnabled())
				{
					canGoRight = true;
					break;
				}
			}

			left->setEnabled(canGoLeft);
			right->setEnabled(canGoRight);
		}

		void colorsChanged() override
		{
			ComboBoxType::colorsChanged();

			left->setColor(ComboBoxType::textUp, ComboBoxType::textDown);
			right->setColor(ComboBoxType::textUp, ComboBoxType::textDown);
		}

		void parameterChanged(int idx) override
		{
			ComboBoxType::parameterChanged(idx);
			updateIncrementArrows(idx);
		}
		class IncrementButton : public PathButton
		{
		public:
			IncrementButton(IncrementalComboBox<ComboBoxType>* parent)
				:PathButton(parent->universals)
			{
				p = parent;
			}

			void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override
			{
				p->button->mouseWheelMove(e, mw);
			}
		private:
			IncrementalComboBox<ComboBoxType>* p;
		};
		std::unique_ptr<IncrementButton> left, right;

	};






} // namespace bdsp
