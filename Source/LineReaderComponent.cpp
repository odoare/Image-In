/*
  ==============================================================================

    LineReaderComponent.cpp
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "LineReaderComponent.h"
#include "PluginProcessor.h"

LineReaderComponent::LineReaderComponent(MapSynthAudioProcessor& p)
    : ReaderComponent(p),
      lineCxKnob(p.apvts, "LineCX", COLOUR),
      lineCyKnob(p.apvts, "LineCY", COLOUR),
      lineLengthKnob(p.apvts, "LineLength", COLOUR),
      lineAngleKnob(p.apvts, "LineAngle", COLOUR),
      lineVolumeKnob(p.apvts, "LineVolume", COLOUR),
      filterFreqKnob(p.apvts, "LineFilterFreq", juce::Colours::yellow),
      filterQualityKnob(p.apvts, "LineFilterQuality", juce::Colours::yellow),
      panKnob(p.apvts, "LinePan", COLOUR),
      modCx(p, "Mod_LineCX_Amount", "Mod_LineCX_Select"),
      modCy(p, "Mod_LineCY_Amount", "Mod_LineCY_Select"),
      modAngle(p, "Mod_LineAngle_Amount", "Mod_LineAngle_Select"),
      modLength(p, "Mod_LineLength_Amount", "Mod_LineLength_Select"),
      modVolume(p, "Mod_LineVolume_Amount", "Mod_LineVolume_Select"),
      modFilterFreq(p, "Mod_LineFilterFreq_Amount", "Mod_LineFilterFreq_Select"),
      modFilterQuality(p, "Mod_LineFilterQuality_Amount", "Mod_LineFilterQuality_Select"),
      modPan(p, "Mod_LinePan_Amount", "Mod_LinePan_Select"),
      modFreq(p, "Mod_LineFreq_Amount", "Mod_LineFreq_Select")
{
    setupKnob(lineCxKnob);
    setupKnob(lineCyKnob);
    setupKnob(lineLengthKnob);
    setupKnob(lineAngleKnob);
    setupKnob(lineVolumeKnob);
    setupKnob(filterFreqKnob);
    setupKnob(filterQualityKnob);
    setupKnob(panKnob);

    addAndMakeVisible(modCx);
    addAndMakeVisible(modCy);
    addAndMakeVisible(modAngle);
    addAndMakeVisible(modLength);
    addAndMakeVisible(modVolume);
    addAndMakeVisible(modFilterFreq);
    addAndMakeVisible(modFilterQuality);
    addAndMakeVisible(modPan);
    addAndMakeVisible(modFreq);

    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItemList(filterTypeChoices, 1);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "LineFilterType", filterTypeBox);
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

    fb.items.add (juce::FlexItem (lineCxKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineCyKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineLengthKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineAngleKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add (juce::FlexItem (lineVolumeKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

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
    addItem(modAngle);
    addItem(modLength);
    addItem(modVolume);
    addItem(modFilterFreq);
    addItem(modFilterQuality);
    addItem(modPan);
    addItem(modFreq);

    fb.performLayout (bounds.toFloat());
}
