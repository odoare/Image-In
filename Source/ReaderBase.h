/*
  ==============================================================================

    ReaderBase.h
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ParameterStructs.h"

class LFO;

class ReaderBase : public juce::ChangeBroadcaster
{
public:
    enum class Type { Ellipse };

    struct DrawingInfo
    {
        Type type = Type::Ellipse;
        bool isActive = false;
        float volume = 0.0f;

        // For Ellipse
        float cx = 0.0f, cy = 0.0f;
        float r1 = 0.0f, r2 = 0.0f, angle = 0.0f;
    };


    ReaderBase() = default;
    virtual ~ReaderBase() = default;

    virtual void prepareToPlay (double sampleRate);
    virtual void processBlock (const juce::Image::BitmapData& bitmapData, juce::AudioBuffer<float>& buffer, int startSample, int numSamples, const juce::AudioBuffer<float>& modulatorBuffer) = 0;

    void setFrequency (float freq);
    float getFrequency() const;
    void setVolume (float newVolume);
    float getVolume() const;
    void setPan (float newPan);
    void updateFilterParameters(const FilterParameters& params);

    virtual Type getType() const = 0;

    DrawingInfo lastDrawingInfo;

protected:
    float frequency = 440.0f;
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float phaseLow = 0.0f;
    float phaseHigh = 0.0f;
    float volume = 1.0f;
    juce::LinearSmoothedValue<float> volumeSmoother;
    std::atomic<float> pan { 0.0f };
    juce::LinearSmoothedValue<float> panSmoother;

    float applyFilter(float inputSample, float modFreqSignal, float modQualitySignal);

    juce::dsp::StateVariableTPTFilter<float> filter;

    std::atomic<int> filterType { (int)FilterType::Lowpass };
    std::atomic<float> filterFreq { 20000.0f };
    std::atomic<float> filterQuality { 1.0f };

    std::atomic<float> modFilterFreqAmount { 0.0f };
    std::atomic<int>   modFilterFreqSelect { 0 };
    std::atomic<float> modFilterQualityAmount { 0.0f };
    std::atomic<int>   modFilterQualitySelect { 0 };
    std::atomic<float> modPanAmount { 0.0f };
    std::atomic<int>   modPanSelect { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReaderBase)
};