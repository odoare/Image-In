/*
  ==============================================================================

    CircleReader.h
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderBase.h"

struct CircleReaderParameters
{
    float cx, cy;
    float radius;
    float volume;

    float lfoCxAmount;
    bool  lfoCxSelect;
    float lfoCyAmount;
    bool  lfoCySelect;
    float lfoRadiusAmount;
    bool  lfoRadiusSelect;
};

class CircleReader  : public ReaderBase
{
public:
    CircleReader();
    ~CircleReader() override;

    void processBlock (const juce::Image& image, juce::AudioBuffer<float>& buffer, int startSample, int numSamples, const juce::AudioBuffer<float>& lfoBuffer) override;

    void setCentre (float newCx, float newCy);
    void setRadius (float newRadius);

    float getCX() const;
    float getCY() const;
    float getRadius() const;

    void updateParameters (const CircleReaderParameters& params);
    Type getType() const override { return Type::Circle; }

public:
    std::atomic<float> lfoCxAmount { 0.5f };
    std::atomic<float> lfoCyAmount { 0.5f };
    std::atomic<float> lfoRadiusAmount { 0.5f };

    std::atomic<bool> lfoCxSelect { false };
    std::atomic<bool> lfoCySelect { false };
    std::atomic<bool> lfoRadiusSelect { false };

private:
    void prepareToPlay (double sampleRate) override;

    std::atomic<float> cx { 0.5f }, cy { 0.5f };
    std::atomic<float> radius { 0.25f };
    juce::LinearSmoothedValue<float> cxs, cys, rs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircleReader)
};
