/*
  ==============================================================================

    LineReader.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderBase.h"
#include "ParameterStructs.h"

/**
    This class reads audio samples from an image by traversing a line.
    It acts as an oscillator and notifies listeners of changes to its state.
*/
class LineReader : public ReaderBase
{
public:
    LineReader();
    ~LineReader() override;

    void processBlock (const juce::Image& image, juce::AudioBuffer<float>& buffer, int startSample, int numSamples, const juce::AudioBuffer<float>& modulatorBuffer) override;

    void setCentre (float newCx, float newCy);
    void setLength (float newLength);
    void setAngle (float newAngle);

    void updateParameters (const LineReaderParameters& params);
    Type getType() const override { return Type::Line; }

private:
    void prepareToPlay (double sampleRate) override;

    std::atomic<float> cx { 0.5f }, cy { 0.5f };
    std::atomic<float> length { 0.6f }, angle { 0.0f };

    juce::LinearSmoothedValue<float> cxs, cys, lengths, angles;

public:
    std::atomic<float> modCxAmount { 0.0f };
    std::atomic<float> modCyAmount { 0.0f };
    std::atomic<float> modAngleAmount { 0.0f };
    std::atomic<float> modLengthAmount { 0.0f };
    std::atomic<float> modVolumeAmount { 0.0f };

    std::atomic<int> modCxSelect { 0 };
    std::atomic<int> modCySelect { 0 };
    std::atomic<int> modAngleSelect { 0 };
    std::atomic<int> modLengthSelect { 0 };
    std::atomic<int> modVolumeSelect { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LineReader)
};