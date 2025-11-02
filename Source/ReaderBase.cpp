/*
  ==============================================================================

    ReaderBase.cpp
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#include "ReaderBase.h"
#include "LFO.h"

void ReaderBase::prepareToPlay(double sr)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32)512, 1 }; // Max block size and 1 channel
    filter.prepare(spec);
    filter.reset();
    panSmoother.reset(sr, 0.05);
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

void ReaderBase::setPan (float newPan)
{
    pan = juce::jlimit (-1.0f, 1.0f, newPan);
    panSmoother.setTargetValue (pan);
}

void ReaderBase::updateFilterParameters(const FilterParameters& params)
{
    filterType = params.type;
    filterFreq = params.frequency;
    filterQuality = params.quality;
    modFilterFreqAmount = params.modFreqAmount;
    modFilterFreqSelect = params.modFreqSelect;
    modFilterQualityAmount = params.modQualityAmount;
    modFilterQualitySelect = params.modQualitySelect;
}

void ReaderBase::resetPhase()
{
    phase = 0.0f;
    phaseLow = 0.0f;
    phaseHigh = 0.0f;
}

float ReaderBase::applyFilter(float inputSample, float modFreqSignal, float modQualitySignal)
{
    auto& f = filter; // for brevity

    auto type = (FilterType)filterType.load();
    if (type == FilterType::Lowpass)
        f.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    else if (type == FilterType::Highpass)
        f.setType(juce::dsp::StateVariableTPTFilterType::highpass);

    // Frequency modulation (exponential)
    const float baseFreq = filterFreq.load();
    const float freqModAmount = modFilterFreqAmount.load();
    const float bipolarFreqMod = modFreqSignal * 2.0f - 1.0f;
    const float numOctaves = 7.0f; // Modulate over a +/- 7 octave range
    const float modulatedFreq = baseFreq * std::pow(2.0f, freqModAmount * bipolarFreqMod * numOctaves);

    // Quality modulation (linear bipolar)
    const float baseQ = filterQuality.load();
    const float qualityModAmount = modFilterQualityAmount.load();
    const float bipolarQualityMod = modQualitySignal * 2.0f - 1.0f;
    const float modulatedQ = baseQ * (1.0f + qualityModAmount * bipolarQualityMod);

    f.setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, modulatedFreq));
    f.setResonance(juce::jlimit(0.1f, 18.0f, modulatedQ));

    return f.processSample(0, inputSample);
}