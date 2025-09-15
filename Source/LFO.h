/*
  ==============================================================================

    LFO.h
    Created: 14 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LFO
{
public:
    LFO() = default;
    ~LFO() = default;

    void prepareToPlay (double sr)
    {
        sampleRate = sr;
        phase = 0.0f;
    }

    void setFrequency (float freq)
    {
        frequency = freq;
    }

    // Returns a sine value between 0.0 and 1.0
    float process()
    {
        const float value = (juce::dsp::FastMathApproximations::sin (phase) + 1.0f) * 0.5f;

        phase += (frequency * juce::MathConstants<float>::twoPi) / (float)sampleRate;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
        
        return value;
    }

private:
    float frequency = 1.0f;
    double sampleRate = 44100.0;
    float phase = 0.0f;
};