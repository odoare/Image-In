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
      cxKnob(p.apvts, "CX", juce::Colours::orange),
      cyKnob(p.apvts, "CY", juce::Colours::orange),
      rKnob(p.apvts, "R", juce::Colours::orange),
      circleVolumeKnob(p.apvts, "CircleVolume", juce::Colours::orange),
      lfoCxAmountKnob(p.apvts, "LFO_CX_Amount", juce::Colours::hotpink),
      lfoCyAmountKnob(p.apvts, "LFO_CY_Amount", juce::Colours::hotpink),
      lfoRadiusAmountKnob(p.apvts, "LFO_R_Amount", juce::Colours::hotpink),
      lfoCxSelectButton(p.apvts, "LFO_CX_Select", juce::Colours::hotpink),
      lfoCySelectButton(p.apvts, "LFO_CY_Select", juce::Colours::hotpink),
      lfoRadiusSelectButton(p.apvts, "LFO_R_Select", juce::Colours::hotpink)
{
    setupKnob(cxKnob);
    setupKnob(cyKnob);
    setupKnob(rKnob);
    setupKnob(circleVolumeKnob);

    setupKnob(lfoCxAmountKnob);
    setupKnob(lfoCyAmountKnob);
    setupKnob(lfoRadiusAmountKnob);

    setupButton(lfoCxSelectButton);
    setupButton(lfoCySelectButton);
    setupButton(lfoRadiusSelectButton);
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

    fb.items.add (juce::FlexItem (cxKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (cyKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (rKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (circleVolumeKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto createLfoControlBox = [] (fxme::FxmeKnob& knob, fxme::FxmeButton& button)
    {
        juce::FlexBox box;
        box.flexDirection = juce::FlexBox::Direction::column;
        box.items.add (juce::FlexItem (knob.flex()).withFlex (3.0f));
        box.items.add (juce::FlexItem (button.flex()).withFlex (1.0f));
        return box;
    };

    auto lfoCxBox = createLfoControlBox(lfoCxAmountKnob, lfoCxSelectButton);
    fb.items.add (juce::FlexItem (lfoCxBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto lfoCyBox = createLfoControlBox(lfoCyAmountKnob, lfoCySelectButton);
    fb.items.add (juce::FlexItem (lfoCyBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto lfoRadiusBox = createLfoControlBox(lfoRadiusAmountKnob, lfoRadiusSelectButton);
    fb.items.add (juce::FlexItem (lfoRadiusBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    fb.performLayout (bounds.toFloat());
}
