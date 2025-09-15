/*
  ==============================================================================

    ReaderBase.h
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LFO;

class ReaderBase : public juce::ChangeBroadcaster
{
public:
    ReaderBase() = default;
    virtual ~ReaderBase() = default;

    virtual void prepareToPlay (double sampleRate) = 0;
    virtual void processBlock (const juce::Image& image, juce::AudioBuffer<float>& buffer, int startSample, int numSamples) = 0;

    void setFrequency (float freq);
    float getFrequency() const;
    void setVolume (float newVolume);
    float getVolume() const;

    void setLFO (LFO* newLfo);

protected:
    float frequency = 440.0f;
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float phaseLow = 0.0f;
    float phaseHigh = 0.0f;
    float volume = 1.0f;
    juce::LinearSmoothedValue<float> volumeSmoother;
    LFO* lfo = nullptr;
    std::atomic<float> lastLfoValue { 0.0f };

    // For CircleReader trigonometric optimization
    float cos_val = 1.0f,      sin_val = 0.0f;
    float cos_val_low = 1.0f,  sin_val_low = 0.0f;
    float cos_val_high = 1.0f, sin_val_high = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReaderBase)
};