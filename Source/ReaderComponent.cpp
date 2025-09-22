/*
  ==============================================================================

    ReaderComponent.cpp
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "ReaderComponent.h"
#include "PluginProcessor.h"
#include "ParameterStructs.h"

ReaderComponent::ReaderComponent(MapSynthAudioProcessor& p) : audioProcessor(p)
{
}

void ReaderComponent::setupKnob (fxme::FxmeKnob& knob)
{
    addAndMakeVisible (knob);
    knob.slider.setLookAndFeel (&fxmeLookAndFeel);
}