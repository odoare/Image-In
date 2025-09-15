/*
  ==============================================================================

    ReaderComponent.cpp
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "ReaderComponent.h"

ReaderComponent::ReaderComponent(MapSynthAudioProcessor& p) : audioProcessor(p)
{
}

void ReaderComponent::setupKnob (fxme::FxmeKnob& knob)
{
    addAndMakeVisible (knob.slider);
    knob.slider.setLookAndFeel (&fxmeLookAndFeel);
}

void ReaderComponent::setupButton (fxme::FxmeButton& button)
{
    addAndMakeVisible (button.button);
    button.button.setLookAndFeel (&fxmeLookAndFeel);
}