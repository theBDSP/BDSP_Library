#pragma once

namespace bdsp
{
	class AnimationTimer : public juce::Timer, public juce::ChangeBroadcaster
	{
	public:
		AnimationTimer();
		virtual ~AnimationTimer();
		juce::Value frameRate;

		juce::Time lastUpdateTime;
		unsigned long long int totalUpdates = 0;

		unsigned long long int getFrameCount();

		int getMillisecondsSinceLastUpdate() const noexcept;

		void setFrameRate(int newRate);

		double getFrameRate();



	private:
		// Inherited via Timer
		void timerCallback() override;

	};


} // namespace bdsp
