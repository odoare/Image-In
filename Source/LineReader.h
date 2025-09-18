/*
  ==============================================================================

    LineReader.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderBase.h"

struct LineReaderParameters
{
    float cx, cy;
    float length;
    float angle;
    float volume;

    float lfoCxAmount;
    bool  lfoCxSelect;
    float lfoCyAmount;
    bool  lfoCySelect;
    float lfoAngleAmount;
    bool  lfoAngleSelect;
    float lfoLengthAmount;
    bool  lfoLengthSelect;
};

/**
    This class reads audio samples from an image by traversing a line.
    It acts as an oscillator and notifies listeners of changes to its state.
*/
class LineReader : public ReaderBase
{
public:
    LineReader();
    ~LineReader() override;

    void processBlock (const juce::Image& image, juce::AudioBuffer<float>& buffer, int startSample, int numSamples, const juce::AudioBuffer<float>& lfoBuffer) override;

    void setCentre (float newCx, float newCy);
    void setLength (float newLength);
    void setAngle (float newAngle);

    float getX1() const;
    float getY1() const;
    float getX2() const;
    float getY2() const;

    void updateParameters (const LineReaderParameters& params);
    Type getType() const override { return Type::Line; }

private:
    void prepareToPlay (double sampleRate) override;

    void getModulated(float& outCx, float& outCy, float& outLength, float& outAngle) const;

    std::atomic<float> cx { 0.5f }, cy { 0.5f };
    std::atomic<float> length { 0.6f }, angle { 0.0f };

    juce::LinearSmoothedValue<float> cxs, cys, lengths, angles;

public:
    std::atomic<float> lfoCxAmount { 0.5f };
    std::atomic<float> lfoCyAmount { 0.5f };
    std::atomic<float> lfoAngleAmount { 0.5f };
    std::atomic<float> lfoLengthAmount { 0.5f };

    std::atomic<bool> lfoCxSelect { false };
    std::atomic<bool> lfoCySelect { false };
    std::atomic<bool> lfoAngleSelect { false };
    std::atomic<bool> lfoLengthSelect { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LineReader)
};