/*
  ==============================================================================

    ReaderBase.cpp
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "ReaderBase.h"
#include "LFO.h"

void ReaderBase::setLFO (LFO* newLfo)
{
    lfo = newLfo;
}

void ReaderBase::setFrequency (float freq)
{
    frequency = freq;
}

float ReaderBase::getFrequency() const
{
    return frequency;
}

void ReaderBase::setVolume (float newVolume)
{
    volume = juce::jlimit (0.0f, 1.0f, newVolume);
    volumeSmoother.setTargetValue (volume);
}

float ReaderBase::getVolume() const
{
    return volume;
}