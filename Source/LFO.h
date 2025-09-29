/*
  ==============================================================================

    LFO.h
    Created: 14 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include "Modulator.h"

class LFO : public Modulator
{
public:
    enum class Waveform
    {
        Sine,
        Square,
        Triangle,
        SawUp,
        SawDown
    };

    LFO() = default;
    ~LFO() = default;

    void prepareToPlay (double sr) override
    {
        sampleRate = sr;
        phase = 0.0f;
        phaseOffsetSmoother.reset(sr, 0.05);
    }

    void setFrequency (float freq)
    {
        frequency = freq;
    }

    void setPhaseOffset(float offset)
    {
        phaseOffsetSmoother.setTargetValue(offset * juce::MathConstants<float>::twoPi);
    }

    void setWaveform(Waveform newWaveform)
    {
        waveform = newWaveform;
    }

    // Returns a value between 0.0 and 1.0
    float process() override
    {
        const float currentPhaseOffset = phaseOffsetSmoother.getNextValue();
        float currentPhase = phase + currentPhaseOffset;
        while (currentPhase >= juce::MathConstants<float>::twoPi)
            currentPhase -= juce::MathConstants<float>::twoPi;
        while (currentPhase < 0.0f)
            currentPhase += juce::MathConstants<float>::twoPi;

        float value = 0.0f;

        switch (waveform)
        {
            case Waveform::Sine:
                value = (std::sin(currentPhase) + 1.0f) * 0.5f;
                break;
            case Waveform::Square:
                value = (currentPhase < juce::MathConstants<float>::pi) ? 1.0f : 0.0f;
                break;
            case Waveform::Triangle:
                value = (currentPhase < juce::MathConstants<float>::pi)
                        ? currentPhase / juce::MathConstants<float>::pi
                        : 1.0f - ((currentPhase - juce::MathConstants<float>::pi) / juce::MathConstants<float>::pi);
                break;
            case Waveform::SawUp:
                value = currentPhase / juce::MathConstants<float>::twoPi;
                break;
            case Waveform::SawDown:
                value = 1.0f - (currentPhase / juce::MathConstants<float>::twoPi);
                break;
        }

        latestValue.store (value, std::memory_order_relaxed);

        phase += (frequency * juce::MathConstants<float>::twoPi) / (float)sampleRate;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        return value;
    }

private:
    float frequency = 1.0f;
    float phase = 0.0f;
    juce::LinearSmoothedValue<float> phaseOffsetSmoother;
    Waveform waveform = Waveform::Sine;
};