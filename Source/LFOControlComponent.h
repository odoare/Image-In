/*
  ==============================================================================

    LFOControlComponent.h
    Created: 18 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ParameterStructs.h"
#include "colours.h"

class LFOControlComponent : public juce::Component
{
public:
    LFOControlComponent(MapSynthAudioProcessor& p, int lfoIndex);
    ~LFOControlComponent() override = default;

    void resized() override;

private:
    // Private nested component to group sync controls for robust layout
    class SyncControls : public juce::Component
    {
    public:
        SyncControls(juce::ToggleButton& button, juce::ComboBox& box);
        void resized() override;

    private:
        juce::ToggleButton& syncButton;
        juce::ComboBox& rateBox;
        juce::FlexBox flexBox;
    };

    static juce::String getFreqParamId(int index) { return (index == 1) ? "LFOFreq" : "LFO" + juce::String(index) + "Freq"; }

    MapSynthAudioProcessor& audioProcessor;
    fxme::FxmeLookAndFeel fxmeLookAndFeel;
    int index;

    fxme::FxmeKnob freqKnob;
    fxme::FxmeKnob phaseKnob;
    juce::ToggleButton syncButton;
    juce::ComboBox rateBox;
    juce::ComboBox waveformBox;
    SyncControls syncControls;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;

    // FlexBox layout objects
    juce::FlexBox fbM;
    juce::FlexBox fbRow1, fbRow2;
};
