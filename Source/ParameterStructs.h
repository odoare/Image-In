/*
  ==============================================================================

    ParameterStructs.h
    Created: 17 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum class FilterType
{
    Lowpass,
    Highpass
};

static const juce::StringArray filterTypeChoices { "Lowpass", "Highpass" };
static const juce::StringArray tempoSyncRateChoices {
    "1/32", "1/16T", "1/16", "1/16D", "1/8T", "1/8", "1/8D", "1/4T", "1/4", "1/4D", "1/2T", "1/2", "1/2D", "1 Bar"
};

static const juce::StringArray lfoWaveformChoices {
    "Sine", "SQ", "TRI", "SAW+", "SAW-"
};

static const juce::StringArray modulatorChoices {
    "LFO 1", "LFO 2", "LFO 3", "LFO 4",
    "ADSR 1", "ADSR 2",
    "LFO1*ADSR1", "LFO1*ADSR2",
    "LFO2*ADSR1", "LFO2*ADSR2",
    "LFO3*ADSR1", "LFO3*ADSR2",
    "LFO4*ADSR1", "LFO4*ADSR2"
};

namespace ModulatorSources
{
    enum
    {
        LFO1 = 0, LFO2, LFO3, LFO4,
        ADSR1, ADSR2,
        LFO1_ADSR1, LFO1_ADSR2,
        LFO2_ADSR1, LFO2_ADSR2,
        LFO3_ADSR1, LFO3_ADSR2,
        LFO4_ADSR1, LFO4_ADSR2,
        NumModulators
    };
}

struct FilterParameters
{
    int type = (int)FilterType::Lowpass;
    float frequency = 20000.0f;
    float quality = 1.0f;
    float modFreqAmount = 0.0f;
    int modFreqSelect = 0;
    float modQualityAmount = 0.0f;
    int modQualitySelect = 0;
};

struct LineReaderParameters
{
    float cx = 0.5f;
    float cy = 0.5f;
    float length = 0.6f;
    float angle = 0.0f;
    float volume = 1.0f;
    float pan = 0.0f;

    float modCxAmount = 0.0f;
    int   modCxSelect = 0;
    float modCyAmount = 0.0f;
    int   modCySelect = 0;
    float modAngleAmount = 0.0f;
    int   modAngleSelect = 0;
    float modLengthAmount = 0.0f;
    int   modLengthSelect = 0;
    float modVolumeAmount = 0.0f;
    int   modVolumeSelect = 0;
    float modPanAmount = 0.0f;
    int   modPanSelect = 0;
    float modFreqAmount = 0.0f;
    int   modFreqSelect = 0;

    FilterParameters filter;
};

struct EllipseReaderParameters
{
    float cx = 0.5f;
    float cy = 0.5f;
    float r1 = 0.4f;
    float r2 = 0.2f;
    float angle = 0.0f;
    float volume = 1.0f;
    float pan = 0.0f;

    float modCxAmount = 0.0f;
    int   modCxSelect = 0;
    float modCyAmount = 0.0f;
    int   modCySelect = 0;
    float modR1Amount = 0.0f;
    int   modR1Select = 0;
    float modR2Amount = 0.0f;
    int   modR2Select = 0;
    float modAngleAmount = 0.0f;
    int   modAngleSelect = 0;
    float modVolumeAmount = 0.0f;
    int   modVolumeSelect = 0;
    float modPanAmount = 0.0f;
    int   modPanSelect = 0;
    float modFreqAmount = 0.0f;
    int   modFreqSelect = 0;

    FilterParameters filter;
};

struct ADSRParameters
{
    float attack = 0.1f;
    float decay = 0.1f;
    float sustain = 1.0f;
    float release = 0.4f;
};

struct GlobalParameters
{
    std::array<EllipseReaderParameters, 3> ellipses;
    ADSRParameters adsr;
    ADSRParameters adsr2;
};