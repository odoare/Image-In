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
    void setupModulatorBox (juce::ComboBox& box,
                            std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>& attachment,
                            const juce::String& paramId);

    MapSynthAudioProcessor& audioProcessor;
    fxme::FxmeLookAndFeel fxmeLookAndFeel;
};