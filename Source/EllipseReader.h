/*
  ==============================================================================

    EllipseReader.h
    Created: 21 Sep 2025 10:00:00am
    Author:  Gemini Code Assist

  ==============================================================================
*/

#pragma once

#include "ReaderBase.h"
#include "ParameterStructs.h"

class EllipseReader : public ReaderBase
{
public:
    EllipseReader();
    ~EllipseReader() override;

    void processBlock (const juce::Image& image, juce::AudioBuffer<float>& buffer, int startSample, int numSamples, const juce::AudioBuffer<float>& modulatorBuffer) override;

    void setCentre (float newCx, float newCy);
    void setRadii (float newR1, float newR2);
    void setAngle (float newAngle);

    void updateParameters (const EllipseReaderParameters& params);
    Type getType() const override { return Type::Ellipse; }
    void prepareToPlay (double sampleRate) override;

private:

    std::atomic<float> cx { 0.5f }, cy { 0.5f };
    std::atomic<float> r1 { 0.4f }, r2 { 0.2f }, angle { 0.0f };

    juce::LinearSmoothedValue<float> cxs, cys, r1s, r2s, angles;

public:
    std::atomic<float> modCxAmount { 0.0f }, modCyAmount { 0.0f }, modR1Amount { 0.0f }, modR2Amount { 0.0f }, modAngleAmount { 0.0f }, modVolumeAmount { 0.0f };

    std::atomic<int> modCxSelect { 0 }, modCySelect { 0 }, modR1Select { 0 }, modR2Select { 0 }, modAngleSelect { 0 }, modVolumeSelect { 0 };

    std::atomic<float> modFreqAmount { 0.0f };
    std::atomic<int> modFreqSelect { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EllipseReader)
};