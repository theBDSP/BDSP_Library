#pragma once



#define BDSP_COLOR_PURE_BLACK "BDSP_Pure_Black"
#define BDSP_COLOR_BLACK "BDSP_Black"
#define BDSP_COLOR_DARK "BDSP_Dark"
#define BDSP_COLOR_MID "BDSP_Mid"
#define BDSP_COLOR_LIGHT "BDSP_Light"
#define BDSP_COLOR_WHITE "BDSP_White"
#define BDSP_COLOR_KNOB "BDSP_Knob"
#define BDSP_COLOR_COLOR "BDSP_Color"


#define BDSP_OVERLAY_ALPHA 0.75f


namespace bdsp
{
	//Associates a juce::Colour with an ID so that components can update their set colors automatically when they are changed globaly
	struct NamedColorsIdentifier
	{
		NamedColorsIdentifier()
		{

		}
		NamedColorsIdentifier(const juce::Identifier& newID)
		{
			ID = newID;
		}

		NamedColorsIdentifier(const NamedColorsIdentifier& other, const std::function<juce::Colour(const juce::Colour&)>& adjustFunc)
		{
			ID = other.ID;
			adjustmentFunction = adjustFunc;
		}

		NamedColorsIdentifier(const NamedColorsIdentifier& other)
			:NamedColorsIdentifier(other, other.adjustmentFunction)
		{

		}

		NamedColorsIdentifier(NamedColorsIdentifier* other)
			:NamedColorsIdentifier(*other, other->adjustmentFunction)
		{

		}

		NamedColorsIdentifier(const NamedColorsIdentifier& other, const NamedColorsIdentifier& mix, float amt)
		{
			ID = other.ID;
			adjustmentFunction = other.adjustmentFunction;
			mixColor = std::make_unique<NamedColorsIdentifier>(mix);
			mixAmt = amt;
		}

		NamedColorsIdentifier(const juce::Identifier& newID, const std::function<juce::Colour(const juce::Colour&)>& adjustFunc)
		{
			ID = newID;
			adjustmentFunction = adjustFunc;
		}
		NamedColorsIdentifier(const juce::String& newID, const std::function<juce::Colour(const juce::Colour&)>& adjustFunc)
			:NamedColorsIdentifier(juce::Identifier(newID), adjustFunc)
		{
		}
		NamedColorsIdentifier(const char* newID, const std::function<juce::Colour(const juce::Colour&)>& adjustFunc)
			:NamedColorsIdentifier(juce::Identifier(newID), adjustFunc)
		{

		}





		NamedColorsIdentifier withMultipliedAlpha(float mult) const
		{
			auto prev = adjustmentFunction;
			std::function<juce::Colour(const juce::Colour&)> func = [=](const juce::Colour& c)
			{
				if (prev.operator bool())
				{
					return prev(c).withMultipliedAlpha(mult);
				}
				else
				{
					return c.withMultipliedAlpha(mult);
				}
			};
			return NamedColorsIdentifier(*this, func);
		}
		NamedColorsIdentifier withRotatedHue(float amt) const
		{
			auto prev = adjustmentFunction;
			std::function<juce::Colour(const juce::Colour&)> func = [=](const juce::Colour& c)
			{
				if (prev.operator bool())
				{
					return prev(c).withRotatedHue(amt);
				}
				else
				{
					return c.withRotatedHue(amt);
				}
			};
			return NamedColorsIdentifier(*this, func);

		}

		NamedColorsIdentifier withMultipliedSaturation(float mult) const
		{
			auto prev = adjustmentFunction;
			std::function<juce::Colour(const juce::Colour&)> func = [=](const juce::Colour& c)
			{
				if (prev.operator bool())
				{
					return prev(c).withMultipliedSaturation(mult);
				}
				else
				{
					return c.withMultipliedSaturation(mult);
				}
			};
			return NamedColorsIdentifier(*this, func);

		}

		NamedColorsIdentifier withMultipliedBrightness(float mult) const
		{
			auto prev = adjustmentFunction;
			std::function<juce::Colour(const juce::Colour&)> func = [=](const juce::Colour& c)
			{
				if (prev.operator bool())
				{
					return prev(c).withMultipliedBrightness(mult);
				}
				else
				{
					return c.withMultipliedBrightness(mult);
				}
			};
			return NamedColorsIdentifier(*this, func);

		}

		NamedColorsIdentifier mixedWith(const NamedColorsIdentifier* other, float amt) const
		{
			return NamedColorsIdentifier(*this, *other, amt);
		}

		NamedColorsIdentifier mixedWith(const NamedColorsIdentifier& other, float amt) const
		{
			return NamedColorsIdentifier(*this, other, amt);
		}


		NamedColorsIdentifier(const char newID[])
			:NamedColorsIdentifier(juce::String(newID))
		{

		}

		NamedColorsIdentifier(const juce::String& newID)
			:NamedColorsIdentifier(juce::Identifier(newID))
		{
		}


		NamedColorsIdentifier& operator=(const NamedColorsIdentifier& other)
		{
			ID = other.ID;
			adjustmentFunction = other.adjustmentFunction;
			if (other.mixColor != nullptr)
			{
				mixColor = std::make_unique<NamedColorsIdentifier>(other.mixColor.get());
			}

			mixAmt = other.mixAmt;
			return *this;
		}

		operator juce::Identifier()
		{
			return ID;
		}



		bool isEmpty() const
		{
			return ID.isNull();
		}

		juce::Identifier ID;
		std::function<juce::Colour(const juce::Colour&)> adjustmentFunction = std::function<juce::Colour(const juce::Colour&)>();
		std::unique_ptr<NamedColorsIdentifier> mixColor;
		float mixAmt = 0;
	};


	// Struct to provide hashing function to use juce::Identifier in standard library maps
	struct IDHash
	{
		std::size_t operator() (const juce::Identifier& id) const noexcept
		{
			return std::hash<std::string>{}(id.toString().toStdString());
		}
	};

	struct ColorScheme
	{

		ColorScheme()
		{
			addColor(BDSP_COLOR_BLACK);
			addColor(BDSP_COLOR_DARK);
			addColor(BDSP_COLOR_MID);
			addColor(BDSP_COLOR_LIGHT);
			addColor(BDSP_COLOR_WHITE);
			addColor(BDSP_COLOR_KNOB);
			addColor(BDSP_COLOR_COLOR);
		}

		void addColor(const juce::Identifier& name, const juce::Colour& c = juce::Colour())
		{
			map[name] = c;
		}

		juce::Colour getColor(const NamedColorsIdentifier& name)
		{

			if (map.find(name.ID) == map.end())
			{
				return juce::Colour();
			}
			else
			{
				if (name.adjustmentFunction.operator bool())
				{
					return name.adjustmentFunction(map[name.ID]);
				}
				else
				{
					return map[name.ID];
				}
			}

		}

		void setColor(const juce::Identifier& name, const juce::Colour& c)
		{
			map[name] = c;
		}

		const std::unordered_map<juce::Identifier, juce::Colour, IDHash>& getMap()
		{
			return map;
		}

	private:

		std::unordered_map<juce::Identifier, juce::Colour, IDHash> map;
	};

}// namnepace bdsp
