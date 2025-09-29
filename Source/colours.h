/*
  ==============================================================================

    colours.h
    Created: 21 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once
#include <juce_graphics/juce_graphics.h>

const juce::Colour LINECOLOUR = juce::Colours::red;
const juce::Colour LFOCONTROLCOLOUR = juce::Colours::hotpink;
const juce::Colour ADSRCONTROLCOLOUR = juce::Colours::orange;

const juce::Colour CIRCLECOLOUR = juce::Colours::blue;
const juce::Colour ELLIPSECOLOURS[6] =
  {juce::Colours::magenta,
    juce::Colours::limegreen,
    juce::Colours::cyan,
    juce::Colours::magenta,
    juce::Colours::yellow,
    juce::Colours::cyan};
const juce::Colour LFOMODULATEDCOLOUR = juce::Colours::grey.withAlpha(0.4f);
