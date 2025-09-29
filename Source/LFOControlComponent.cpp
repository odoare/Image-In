/*
  ==============================================================================

    LFOControlComponent.cpp
    Created: 18 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#include "LFOControlComponent.h"
#include "colours.h"

LFOControlComponent::SyncControls::SyncControls(juce::ToggleButton& button, juce::ComboBox& box)
    : syncButton(button), rateBox(box)
{
    addAndMakeVisible(syncButton);
    addAndMakeVisible(rateBox);

    flexBox.flexDirection = juce::FlexBox::Direction::row;
}

void LFOControlComponent::SyncControls::resized()
{
    flexBox.items.clear();
    flexBox.items.add(juce::FlexItem(syncButton).withFlex(1.0f));
    flexBox.items.add(juce::FlexItem(rateBox).withFlex(1.0f));
    flexBox.performLayout(getLocalBounds());
}

LFOControlComponent::LFOControlComponent(MapSynthAudioProcessor& p, int lfoIndex)
    : audioProcessor(p),
      index(lfoIndex),
      freqKnob(p.apvts, getFreqParamId(lfoIndex), LFOCONTROLCOLOUR),
      phaseKnob(p.apvts, "LFO" + juce::String(index) + "Phase", LFOCONTROLCOLOUR),
      syncControls(syncButton, rateBox)
{
    addAndMakeVisible(freqKnob);
    freqKnob.slider.setLookAndFeel(&fxmeLookAndFeel);

    addAndMakeVisible(phaseKnob);
    phaseKnob.slider.setLookAndFeel(&fxmeLookAndFeel);

    juce::String syncParamId = "LFO" + juce::String(index) + "Sync";
    juce::String rateParamId = "LFO" + juce::String(index) + "Rate";
    juce::String waveParamId = "LFO" + juce::String(index) + "Wave";

    syncButton.setButtonText("Sync");
    syncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, syncParamId, syncButton);

    addAndMakeVisible(syncControls);

    addAndMakeVisible(waveformBox);
    waveformBox.addItemList(lfoWaveformChoices, 1);
    waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, waveParamId, waveformBox);

    rateBox.addItemList(tempoSyncRateChoices, 1);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, rateParamId, rateBox);

    auto updateKnobEnabledState = [this]
    {
        bool syncEnabled = syncButton.getToggleState();
        freqKnob.slider.setEnabled(!syncEnabled);
        rateBox.setEnabled(syncEnabled);
    };

    syncButton.onStateChange = updateKnobEnabledState;
    updateKnobEnabledState(); // Set initial state

    // Init FlexBoxes
    fbM.flexDirection = juce::FlexBox::Direction::column;
    fbRow1.flexDirection = juce::FlexBox::Direction::row;
    fbRow2.flexDirection = juce::FlexBox::Direction::row;
}

void LFOControlComponent::resized()
{
    // Clear and rebuild layout
    fbM.items.clear();
    fbRow1.items.clear();
    fbRow2.items.clear();

    fbRow1.items.add(juce::FlexItem(waveformBox).withFlex(.8f));
    fbRow1.items.add(juce::FlexItem(syncControls).withFlex(1.f));
    fbRow2.items.add(juce::FlexItem(freqKnob).withFlex(1.f));
    fbRow2.items.add(juce::FlexItem(phaseKnob).withFlex(1.f));
    
    fbM.items.add(juce::FlexItem(fbRow1).withFlex(0.2));
    fbM.items.add(juce::FlexItem(fbRow2).withFlex(1));
    fbM.performLayout(getLocalBounds());
}
