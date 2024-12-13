#pragma once


namespace bdsp
{
	class Viewport : public juce::Viewport, public ComponentCore
	{

	public:
		Viewport(GUI_Universals* universalsToUse)
			:ComponentCore(this, universalsToUse)
		{

		}
		virtual ~Viewport() = default;

		void setScrollDistance(float d)
		{
			scrollDistance = d;
		}
		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override
		{
			bool up;
			if (!mw.isReversed)
			{
				if (mw.deltaY < 0)
				{
					up = false;
				}
				else if (mw.deltaY > 0)
				{
					up = true;
				}
			}
			else
			{
				if (mw.deltaY < 0)
				{
					up = true;
				}
				else if (mw.deltaY > 0)
				{
					up = false;
				}
			}
			auto h = scrollDistance; // plus borderColor of 0.25h

			setViewPosition(0, getViewPositionY() + (up ? -1 : 1) * h);

		}
	private:
		float scrollDistance = 1;
	};

} // namespace bdsp