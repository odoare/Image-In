/*
  ==============================================================================

    CircleReader.h
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderBase.h"
#include "ParameterStructs.h"
#include "colours.h"

class CircleReader  : public ReaderBase
{
public:
    CircleReader();
    ~CircleReader() override;

    void processBlock (const juce::Image::BitmapData& bitmapData, juce::AudioBuffer<float>& buffer, int startSample, int numSamples, const juce::AudioBuffer<float>& modulatorBuffer) override;

    void setCentre (float newCx, float newCy);
    void setRadius (float newRadius);

    void updateParameters (const CircleReaderParameters& params);
    Type getType() const override { return Type::Circle; }

public:
    std::atomic<float> modCxAmount { 0.0f };
    std::atomic<float> modCyAmount { 0.0f };
    std::atomic<float> modRadiusAmount { 0.0f };
    std::atomic<float> modVolumeAmount { 0.0f };

    std::atomic<int> modCxSelect { 0 };
    std::atomic<int> modCySelect { 0 };
    std::atomic<int> modRadiusSelect { 0 };
    std::atomic<int> modVolumeSelect { 0 };

    std::atomic<float> modFreqAmount { 0.0f };
    std::atomic<int> modFreqSelect { 0 };

private:
    void prepareToPlay (double sampleRate) override;

    std::atomic<float> cx { 0.5f }, cy { 0.5f };
    std::atomic<float> radius { 0.25f };
    juce::LinearSmoothedValue<float> cxs, cys, rs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircleReader)
};
