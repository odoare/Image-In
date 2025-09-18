/*
  ==============================================================================

    ParameterStructs.h
    Created: 18 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>

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

struct CircleReaderParameters
{
    float cx, cy;
    float radius;
    float volume;

    float lfoCxAmount;
    bool  lfoCxSelect;
    float lfoCyAmount;
    bool  lfoCySelect;
    float lfoRadiusAmount;
    bool  lfoRadiusSelect;
};

struct ADSRParameters
{
    float attack;
    float decay;
    float sustain;
    float release;
};

struct GlobalParameters
{
    LineReaderParameters line;
    CircleReaderParameters circle;
    ADSRParameters adsr;
};