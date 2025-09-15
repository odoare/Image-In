/*
  ==============================================================================

    CircleReader.h
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderBase.h"

class CircleReader  : public ReaderBase
{
public:
    CircleReader();
    ~CircleReader() override;

    void processBlock (const juce::Image& image, juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;

    void setCentre (float newCx, float newCy);
    void setRadius (float newRadius);

    float getCX() const;
    float getCY() const { return cy.load(); }
    float getRadius() const;

public:
    std::atomic<float> lfoCxAmount { 0.5f };
    std::atomic<float> lfoRadiusAmount { 0.5f };

private:
    void prepareToPlay (double sampleRate) override;

    std::atomic<float> cx { 0.5f }, cy { 0.5f };
    std::atomic<float> radius { 0.25f };
    juce::LinearSmoothedValue<float> cxs, cys, rs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircleReader)
};