/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MapDisplayComponent.h"
#include "LineReaderComponent.h"
#include "CircleReaderComponent.h"

#define COLOUR juce::Colour::fromRGB(100, 100, 250)

//==============================================================================
/**
*/
class MapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MapSynthAudioProcessorEditor (MapSynthAudioProcessor&);
    ~MapSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MapSynthAudioProcessor& audioProcessor;

    fxme::FxmeLookAndFeel fxmeLookAndFeel;

    MapDisplayComponent mapDisplayComponent;

    LineReaderComponent lineReaderComponent;
    CircleReaderComponent circleReaderComponent;

    fxme::FxmeKnob frequencyKnob {audioProcessor.apvts, "Frequency", juce::Colours::red};
    fxme::FxmeKnob lfoFreqKnob {audioProcessor.apvts, "LFOFreq", juce::Colours::hotpink};

    juce::GroupComponent globalControlsGroup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessorEditor)
};
