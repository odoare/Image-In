/*
  ==============================================================================

    ADSR.h
    Created: 17 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "Modulator.h"

class ADSR : public Modulator
{
public:
    ADSR();
    ~ADSR() override;

    void prepareToPlay (double sampleRate) override;
    float process() override;

    void setParameters (const juce::ADSR::Parameters& params);

    void noteOn();
    void noteOff();
    bool isActive() const;

private:
    juce::ADSR adsr;
};