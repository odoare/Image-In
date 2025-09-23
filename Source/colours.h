/*
  ==============================================================================

    colours.h
    Created: 21 Sep 2025 10:00:00am
    Author:  Gemini Code Assist

  ==============================================================================
*/

#pragma once
#include <juce_graphics/juce_graphics.h>

const juce::Colour LINECOLOUR = juce::Colours::red;
const juce::Colour CIRCLECOLOUR = juce::Colours::blue;
const juce::Colour ELLIPSECOLOURS[6] =
  {juce::Colours::red,
    juce::Colours::green,
    juce::Colours::blue,
    juce::Colours::magenta,
    juce::Colours::yellow,
    juce::Colours::cyan};
const juce::Colour LFOMODULATEDCOLOUR = juce::Colours::grey.withAlpha(0.4f);
