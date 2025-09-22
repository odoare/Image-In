/*
  ==============================================================================

    CircleReaderComponent.cpp
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "CircleReaderComponent.h"
#include "PluginProcessor.h"

CircleReaderComponent::CircleReaderComponent(MapSynthAudioProcessor& p)
    : ReaderComponent(p),
      cxKnob(p.apvts, "CX", CIRCLECOLOUR),
      cyKnob(p.apvts, "CY", CIRCLECOLOUR),
      rKnob(p.apvts, "R", CIRCLECOLOUR),
      circleVolumeKnob(p.apvts, "CircleVolume", CIRCLECOLOUR),
      panKnob(p.apvts, "CirclePan", CIRCLECOLOUR),
      filterFreqKnob(p.apvts, "CircleFilterFreq", juce::Colours::yellow),
      filterQualityKnob(p.apvts, "CircleFilterQuality", juce::Colours::yellow),
      modCx(p, "Mod_CircleCX_Amount", "Mod_CircleCX_Select"),
      modCy(p, "Mod_CircleCY_Amount", "Mod_CircleCY_Select"),
      modRadius(p, "Mod_CircleRadius_Amount", "Mod_CircleRadius_Select"),
      modVolume(p, "Mod_CircleVolume_Amount", "Mod_CircleVolume_Select"),
      modFilterFreq(p, "Mod_CircleFilterFreq_Amount", "Mod_CircleFilterFreq_Select"),
      modFilterQuality(p, "Mod_CircleFilterQuality_Amount", "Mod_CircleFilterQuality_Select"),
      modPan(p, "Mod_CirclePan_Amount", "Mod_CirclePan_Select"),
      modFreq(p, "Mod_CircleFreq_Amount", "Mod_CircleFreq_Select")
{
    setupKnob(cxKnob);
    setupKnob(cyKnob);
    setupKnob(rKnob);
    setupKnob(circleVolumeKnob);
    setupKnob(filterFreqKnob);
    setupKnob(filterQualityKnob);
    setupKnob(panKnob);

    addAndMakeVisible(modCx);
    addAndMakeVisible(modCy);
    addAndMakeVisible(modRadius);
    addAndMakeVisible(modVolume);
    addAndMakeVisible(modFilterFreq);
    addAndMakeVisible(modFilterQuality);
    addAndMakeVisible(modPan);
    addAndMakeVisible(modFreq);

    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItemList(filterTypeChoices, 1);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "CircleFilterType", filterTypeBox);
}

void CircleReaderComponent::paint(juce::Graphics& g)
{
    g.setColour (juce::Colours::darkgrey.withAlpha(0.5f));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("Circle Reader", getLocalBounds().reduced(5).removeFromTop(20), juce::Justification::centredTop, 1);
}

void CircleReaderComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(10);

    juce::FlexBox fb;
    fb.flexWrap = juce::FlexBox::Wrap::wrap;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    fb.items.add (juce::FlexItem (cxKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (cyKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (rKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (circleVolumeKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    juce::FlexBox filterBox;
    filterBox.flexDirection = juce::FlexBox::Direction::column;
    filterBox.items.add(juce::FlexItem(filterFreqKnob).withFlex(3.0f));
    filterBox.items.add(juce::FlexItem(filterTypeBox).withFlex(1.0f).withMargin(juce::FlexItem::Margin(2.f, 0, 0, 0)));
    fb.items.add(juce::FlexItem(filterBox).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add(juce::FlexItem(filterQualityKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add(juce::FlexItem(panKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    
    auto addItem = [&fb] (juce::Component& c) {
        fb.items.add(juce::FlexItem(c).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    };

    addItem(modCx);
    addItem(modCy);
    addItem(modRadius);
    addItem(modVolume);
    addItem(modFilterFreq);
    addItem(modFilterQuality);
    addItem(modPan);
    addItem(modFreq);

    fb.performLayout (bounds.toFloat());
}
