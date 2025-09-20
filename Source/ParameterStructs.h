/*
  ==============================================================================

    ParameterStructs.h
    Created: 18 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum class ModulatorSource
{
    LFO1,
    LFO2,
    ADSR1,
    ADSR2
};

const juce::StringArray modulatorChoices { "LFO 1", "LFO 2", "ADSR 1", "ADSR 2" };

enum class FilterType
{
    Lowpass,
    Highpass
};

const juce::StringArray filterTypeChoices { "Low Pass", "High Pass" };

struct ADSRParameters
{
    float attack;
    float decay;
    float sustain;
    float release;
};

struct FilterParameters
{
    int   type;
    float frequency;
    float quality;

    float modFreqAmount;
    int   modFreqSelect;
    float modQualityAmount;
    int   modQualitySelect;
};

struct LineReaderParameters
{
    float cx, cy;
    float length;
    float angle;
    float volume;

    float modCxAmount;
    int   modCxSelect;
    float modCyAmount;
    int   modCySelect;
    float modAngleAmount;
    int   modAngleSelect;
    float modLengthAmount;
    int   modLengthSelect;
    float modVolumeAmount;
    int   modVolumeSelect;
    float pan;
    float modPanAmount;
    int   modPanSelect;

    FilterParameters filter;
};

struct CircleReaderParameters
{
    float cx, cy;
    float radius;
    float volume;

    float modCxAmount;
    int   modCxSelect;
    float modCyAmount;
    int   modCySelect;
    float modRadiusAmount;
    int   modRadiusSelect;
    float modVolumeAmount;
    int   modVolumeSelect;
    float pan;
    float modPanAmount;
    int   modPanSelect;

    FilterParameters filter;
};

struct GlobalParameters
{
    LineReaderParameters line;
    CircleReaderParameters circle;
    ADSRParameters adsr;
    ADSRParameters adsr2;
};