/*
  ==============================================================================

    ReaderComponent.h
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MapSynthAudioProcessor;

class ReaderComponent : public juce::Component
{
public:
    ReaderComponent(MapSynthAudioProcessor& p);
    ~ReaderComponent() override = default;

protected:
    void setupKnob (fxme::FxmeKnob& knob);

    MapSynthAudioProcessor& audioProcessor;
    fxme::FxmeLookAndFeel fxmeLookAndFeel;
};