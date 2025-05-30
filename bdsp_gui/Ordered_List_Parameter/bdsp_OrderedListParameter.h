#pragma once
namespace bdsp
{
	template <typename T>
	void reorderArray(juce::Array<T>& arr, juce::Array<int> order)
	{
		jassert(arr.size() == order.size());

		juce::Array<T> temp;
		temp.resize(arr.size());

		for (int i = 0; i < order.size(); ++i)
		{
			temp.set(i, arr[order[i]]);
		}

		temp.swapWith(arr);
	}

	class OrderedListParameter : public juce::AudioParameterInt::Listener
	{

	public:

		OrderedListParameter(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int listSize, juce::ParameterID ID)
		{
			order.resize(listSize);

			baseName = ID.getParamID() + " Item ";
			baseID = baseName.removeCharacters(" ");

			for (int i = 0; i < listSize; ++i)
			{
				order.set(i, i);
				layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID(baseID + juce::String(i) + "IndexID", ID.getVersionHint()), baseName + juce::String(i) + " Index", 0, listSize, i));

			}

		}

		~OrderedListParameter()
		{
			for (auto p : indexParams)
			{
				p->removeListener(this);
			}
		}

		void setAPVTS(juce::AudioProcessorValueTreeState* newAPVTS)
		{
			for (int i = 0; i < order.size(); ++i)
			{
				indexParams.add(dynamic_cast<juce::AudioParameterInt*>(newAPVTS->getParameter(baseID + juce::String(i) + "IndexID")));
				indexParams.getLast()->addListener(this);
			}

		}


		void setNewOrder(const juce::Array<int>& newOrder)
		{
			jassert(newOrder.size() == order.size());

			bool changed = (order != newOrder);
			if (changed)
			{
				isChangingIndexParams = true;
				order = newOrder;
				for (int i = 0; i < order.size(); ++i)
				{
					auto param = indexParams[order[i]];
					param->beginChangeGesture();
					param->setValueNotifyingHost(param->convertTo0to1(i));
					param->endChangeGesture();
				}
				isChangingIndexParams = false;
				listeners.call([&](Listener& l)
				{
					l.orderChanged(order);
				});
			}

		}

		void moveSingleItem(int indexToMove, int indexToMoveTo)
		{
			auto newOrder = order;
			newOrder.move(indexToMove, indexToMoveTo);
			setNewOrder(newOrder);
		}

		const juce::Array<int>& getOrder()
		{
			return order;
		}

		class Listener
		{
		public:
			Listener(OrderedListParameter* p)
			{
				parent = p;
			}
			~Listener()
			{
				if (!parent.wasObjectDeleted())
				{
					parent->removeListener(this);
				}
			}


			virtual void orderChanged(const juce::Array<int> newOrder) = 0;

		private:
			juce::WeakReference<OrderedListParameter> parent;
		};

		void addListener(Listener* newListener)
		{
			listeners.add(newListener);
		}

		void removeListener(Listener* listenerToRemove)
		{
			listeners.remove(listenerToRemove);
		}


		void presetLoaded()
		{
			juce::Array<int> newOrder;
			newOrder.resize(indexParams.size());
			for (int i = 0; i < indexParams.size(); ++i)
			{
				newOrder.set(indexParams[i]->get(), i);
			}
			setNewOrder(newOrder);
		}

		juce::Array<juce::AudioParameterInt*> getIndexParams()
		{
			return indexParams;
		}

		bool isChangingIndexParameters()
		{
			return isChangingIndexParams;
		}

	private:

		juce::Array<int> order;
		juce::ListenerList<Listener> listeners;

		juce::Array<juce::AudioParameterInt*> indexParams;
		juce::String baseName, baseID;
		bool isChangingIndexParams = false;

		JUCE_DECLARE_WEAK_REFERENCEABLE(OrderedListParameter)

			// Inherited via Listener
			void parameterValueChanged(int parameterIndex, float newValue) override
		{
			if (!isChangingIndexParams)
			{
				juce::Array<int> newOrder, indexVals;
				newOrder.resize(indexParams.size());
				for (int i = 0; i < indexParams.size(); ++i)
				{
					if (indexVals.contains(indexParams[i]->get()))
					{
						return;
					}
					jassert(juce::isPositiveAndBelow(indexVals.getLast(), indexParams.size()));
					indexVals.add(indexParams[i]->get());
					newOrder.set(indexVals.getLast(), i);
				}
				jassert(newOrder.size() == indexParams.size());
				{
					setNewOrder(newOrder);
				}



			}
		}
		void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
		{

		}
	};

} //namespace bdsp