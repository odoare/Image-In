/*
  ==============================================================================

    ModControlBox.cpp
    Created: 18 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "ModControlBox.h"
#include "PluginProcessor.h"

ModControlBox::ModControlBox(MapSynthAudioProcessor& p,
                             const juce::String& modAmountParamId,
                             const juce::String& modSelectParamId,
                             const juce::Colour& colour)
    : amountKnob(p.apvts, modAmountParamId, "", colour)
{
    amountKnob.slider.setLookAndFeel(&fxmeLookAndFeel);
    addAndMakeVisible(amountKnob);

    addAndMakeVisible(selectBox);
    selectBox.addItemList(modulatorChoices, 1);
    selectAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, modSelectParamId, selectBox);
}

void ModControlBox::resized()
{
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column;
    flexBox.items.add(juce::FlexItem(amountKnob).withFlex(4.0f));
    flexBox.items.add(juce::FlexItem(selectBox).withFlex(1.0f).withMargin(juce::FlexItem::Margin(2.f, 0, 0, 0)));
    flexBox.performLayout(getLocalBounds());
}