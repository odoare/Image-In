/*
  ==============================================================================

    Modulator.h
    Created: 17 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Modulator
{
public:
    virtual ~Modulator() = default;

    virtual void prepareToPlay (double sampleRate) = 0;
    virtual float process() = 0;

    float getLatestValue() const { return latestValue.load (std::memory_order_relaxed); }

protected:
    std::atomic<float> latestValue { 0.0f };
    double sampleRate = 44100.0;
};