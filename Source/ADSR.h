/*
  ==============================================================================

    ADSR.h
    Created: 17 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "Modulator.h"
#include "ParameterStructs.h"

class ADSR : public Modulator
{
public:
    ADSR();
    ~ADSR() override;

    void prepareToPlay (double sampleRate) override;
    float process() override;

    void setParameters (const ADSRParameters& params);

    void applyEnvelopeToBuffer (juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    void noteOn();
    void noteOff();
    bool isActive() const;

private:
    juce::ADSR adsr;
};