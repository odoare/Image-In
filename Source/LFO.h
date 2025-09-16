/*
  ==============================================================================

    LFO.h
    Created: 14 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "Modulator.h"

class LFO : public Modulator
{
public:
    LFO() = default;
    ~LFO() = default;

    void prepareToPlay (double sr) override
    {
        sampleRate = sr;
        phase = 0.0f;
    }

    void setFrequency (float freq)
    {
        frequency = freq;
    }

    // Returns a sine value between 0.0 and 1.0
    float process() override
    {
        const float value = (juce::dsp::FastMathApproximations::sin (phase) + 1.0f) * 0.5f;

        latestValue.store (value, std::memory_order_relaxed);

        phase += (frequency * juce::MathConstants<float>::twoPi) / (float)sampleRate;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
        
        return value;
    }

private:
    float frequency = 1.0f;
    float phase = 0.0f;
};