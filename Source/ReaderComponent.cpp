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
    addAndMakeVisible (knob.slider);
    knob.slider.setLookAndFeel (&fxmeLookAndFeel);
}

void ReaderComponent::setupModulatorBox (juce::ComboBox& box,
                                         std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>& attachment,
                                         const juce::String& paramId)
{
    addAndMakeVisible (box);
    box.addItemList (modulatorChoices, 1);
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, paramId, box);
}