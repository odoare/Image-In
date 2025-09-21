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
static const juce::StringArray modulatorChoices { "LFO 1", "LFO 2", "LFO 3", "LFO 4", "ADSR 1", "ADSR 2" };

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

    FilterParameters filter;
};

struct CircleReaderParameters
{
    float cx = 0.5f;
    float cy = 0.5f;
    float radius = 0.25f;
    float volume = 1.0f;
    float pan = 0.0f;

    float modCxAmount = 0.0f;
    int   modCxSelect = 0;
    float modCyAmount = 0.0f;
    int   modCySelect = 0;
    float modRadiusAmount = 0.0f;
    int   modRadiusSelect = 0;
    float modVolumeAmount = 0.0f;
    int   modVolumeSelect = 0;
    float modPanAmount = 0.0f;
    int   modPanSelect = 0;

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
    LineReaderParameters line;
    CircleReaderParameters circle;
    ADSRParameters adsr;
    ADSRParameters adsr2;
};