/*
  ==============================================================================

    ADSR.cpp
    Created: 17 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "ADSR.h"

ADSR::ADSR() {}
ADSR::~ADSR() {}

void ADSR::prepareToPlay (double sr)
{
    sampleRate = sr;
    adsr.setSampleRate (sampleRate);
}

float ADSR::process()
{
    const float value = adsr.getNextSample();
    latestValue.store (value, std::memory_order_relaxed);
    return value;
}

void ADSR::setParameters (const juce::ADSR::Parameters& params)
{
    adsr.setParameters (params);
}

void ADSR::applyEnvelopeToBuffer (juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    adsr.applyEnvelopeToBuffer (buffer, startSample, numSamples);
}

void ADSR::noteOn() { adsr.noteOn(); }
void ADSR::noteOff() { adsr.noteOff(); }

bool ADSR::isActive() const
{
    return adsr.isActive();
}