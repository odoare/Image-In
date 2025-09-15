/*
  ==============================================================================

    LineReaderComponent.cpp
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "LineReaderComponent.h"

LineReaderComponent::LineReaderComponent(MapSynthAudioProcessor& p)
    : ReaderComponent(p),
      lineCxKnob(p.apvts, "LineCX", COLOUR),
      lineCyKnob(p.apvts, "LineCY", COLOUR),
      lineLengthKnob(p.apvts, "LineLength", COLOUR),
      lineAngleKnob(p.apvts, "LineAngle", COLOUR),
      lineVolumeKnob(p.apvts, "LineVolume", COLOUR),
      lfoLineAngleAmountKnob(p.apvts, "LFO_LineAngle_Amount", juce::Colours::hotpink),
      lfoLineLengthAmountKnob(p.apvts, "LFO_LineLength_Amount", juce::Colours::hotpink)
{
    setupKnob(lineCxKnob);
    setupKnob(lineCyKnob);
    setupKnob(lineLengthKnob);
    setupKnob(lineAngleKnob);
    setupKnob(lineVolumeKnob);

    setupKnob(lfoLineAngleAmountKnob);
    setupKnob(lfoLineLengthAmountKnob);
}

void LineReaderComponent::paint(juce::Graphics& g)
{
    g.setColour (juce::Colours::darkgrey.withAlpha(0.5f));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("Line Reader", getLocalBounds().reduced(5).removeFromTop(20), juce::Justification::centredTop, 1);
}

void LineReaderComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(10);

    juce::FlexBox fb;
    fb.flexWrap = juce::FlexBox::Wrap::wrap;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    fb.items.add (juce::FlexItem (lineCxKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineCyKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineLengthKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineAngleKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineVolumeKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lfoLineAngleAmountKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lfoLineLengthAmountKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    fb.performLayout (bounds.toFloat());
}
