#include "bdsp_AnimationTimer.h"

namespace bdsp
{

	AnimationTimer::AnimationTimer()
	{
		frameRate.setValue(30);
		setFrameRate(30);
	}

	AnimationTimer::~AnimationTimer()
	{
		stopTimer();
	}


	unsigned long long int AnimationTimer::getFrameCount()
	{
		return totalUpdates;
	}

	int AnimationTimer::getMillisecondsSinceLastUpdate() const noexcept
	{
		return (int)(juce::Time::getCurrentTime() - lastUpdateTime).inMilliseconds();
	}

	void AnimationTimer::setFrameRate(int newRate)
	{
		lastUpdateTime = juce::Time::getCurrentTime();
		frameRate.setValue(newRate);
		startTimerHz(newRate);
	}

	double AnimationTimer::getFrameRate()
	{
		return frameRate.getValue().operator double();
	}

	void AnimationTimer::timerCallback()
	{
		++totalUpdates;
		sendSynchronousChangeMessage();
		lastUpdateTime = juce::Time::getCurrentTime();
	}


} // namespace bdsp