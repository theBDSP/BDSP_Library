#pragma once 


namespace bdsp
{
	class PresetMenu;

	class PresetMenuItem : public juce::Component, public ComponentCore, public GUI_Universals::Listener
	{
	public:



		PresetMenuItem(PresetMenu* parent, int idx, const juce::File& f);


		~PresetMenuItem() = default;

		PresetMenuItem(const PresetMenuItem& oldItem);




		bool operator == (const PresetMenuItem& other);


		void resized() override;







		void mouseDown(const juce::MouseEvent& e) override;

		void mouseDoubleClick(const juce::MouseEvent& e)override;

		juce::String getLabelText();

		void setHighlighted(bool state);
		bool isHighlighted();

		void setFavorite(bool state);
		bool isFavorite();


		void paint(juce::Graphics& g)override;

		void updateFile(const juce::File& newFile);

		void readTags();



		void setFile(const juce::File& newFile);
		juce::File getFile();

		juce::StringArray getTags();
		juce::OwnedArray<Label>* getSuperTagLabels();

		bool testTags(const juce::StringArray& tagsToMatch);





		juce::StringArray getSuperTags(); // returns this items superTag values
		juce::String getSuperTag(int tagIDX); // returns this items superTag values for a specific superTag

		bool testSuperTags(const juce::StringArray& superTagsToMatch);

		class PassthoughLabel : public MousePassthrough<Label>
		{
		public:
			PassthoughLabel(PresetMenuItem* p, GUI_Universals* universalsToUse);
			~PassthoughLabel() = default;

		}label;


		MultiShapeButton favIcon;


		int index;



		class Listener
		{
		public:
			Listener()
			{
			}

			virtual ~Listener() = default;

			void setItem(PresetMenuItem* MI)
			{
				itemListeneingTo = MI;
			}

			virtual void presetUpdated() = 0;



		private:

			PresetMenuItem* itemListeneingTo = nullptr;
		};


		void addListener(Listener* listenerToAdd);
		void removeListener(Listener* listenerToRemove);

	private:

		PresetMenu* p;
		float b = 0.0f;

		bool Highlighted = false;



		juce::StringArray Tags; // normal tag indecies
		juce::OwnedArray<Label> superTagLabels;

		juce::File file;

		juce::Array<Listener*> listeners;



		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;

	};
} // namespace bdsp
