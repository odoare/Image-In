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
      modCxAmountKnob(p.apvts, "Mod_CircleCX_Amount", juce::Colours::hotpink),
      modCyAmountKnob(p.apvts, "Mod_CircleCY_Amount", juce::Colours::hotpink),
      modRadiusAmountKnob(p.apvts, "Mod_CircleRadius_Amount", juce::Colours::hotpink),
      modVolumeAmountKnob(p.apvts, "Mod_CircleVolume_Amount", juce::Colours::hotpink),
      filterFreqKnob(p.apvts, "CircleFilterFreq", juce::Colours::yellow),
      filterQualityKnob(p.apvts, "CircleFilterQuality", juce::Colours::yellow),
      modFilterFreqAmountKnob(p.apvts, "Mod_CircleFilterFreq_Amount", juce::Colours::hotpink),
      modFilterQualityAmountKnob(p.apvts, "Mod_CircleFilterQuality_Amount", juce::Colours::hotpink),
      modPanAmountKnob(p.apvts, "Mod_CirclePan_Amount", juce::Colours::hotpink),
      modFreqAmountKnob(p.apvts, "Mod_CircleFreq_Amount", juce::Colours::hotpink)
{
    setupKnob(cxKnob);
    setupKnob(cyKnob);
    setupKnob(rKnob);
    setupKnob(circleVolumeKnob);
    setupKnob(modVolumeAmountKnob);

    setupKnob(modCxAmountKnob);
    setupKnob(modCyAmountKnob);
    setupKnob(modRadiusAmountKnob);

    setupKnob(filterFreqKnob);
    setupKnob(filterQualityKnob);
    setupKnob(modFilterFreqAmountKnob);
    setupKnob(modFilterQualityAmountKnob);
    setupKnob(panKnob);
    setupKnob(modPanAmountKnob);
    setupKnob(modFreqAmountKnob);

    setupModulatorBox (modCxSelectBox, modCxSelectAttachment, "Mod_CircleCX_Select");
    setupModulatorBox (modCySelectBox, modCySelectAttachment, "Mod_CircleCY_Select");
    setupModulatorBox (modRadiusSelectBox, modRadiusSelectAttachment, "Mod_CircleRadius_Select");
    setupModulatorBox (modVolumeSelectBox, modVolumeSelectAttachment, "Mod_CircleVolume_Select");

    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItemList(filterTypeChoices, 1);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "CircleFilterType", filterTypeBox);
    setupModulatorBox(modFilterFreqSelectBox, modFilterFreqSelectAttachment, "Mod_CircleFilterFreq_Select");
    setupModulatorBox(modFilterQualitySelectBox, modFilterQualitySelectAttachment, "Mod_CircleFilterQuality_Select");
    setupModulatorBox(modPanSelectBox, modPanSelectAttachment, "Mod_CirclePan_Select");
    setupModulatorBox(modFreqSelectBox, modFreqSelectAttachment, "Mod_CircleFreq_Select");
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

    juce::FlexBox filterBox;
    filterBox.flexDirection = juce::FlexBox::Direction::column;
    filterBox.items.add(juce::FlexItem(filterFreqKnob.flex()).withFlex(3.0f));
    filterBox.items.add(juce::FlexItem(filterTypeBox).withFlex(1.0f).withMargin(juce::FlexItem::Margin(2.f, 0, 0, 0)));
    fb.items.add(juce::FlexItem(filterBox).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add(juce::FlexItem(filterQualityKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add(juce::FlexItem(panKnob.flex()).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto createModControlBox = [] (fxme::FxmeKnob& knob, juce::ComboBox& box)
    {
        juce::FlexBox flexBox;
        flexBox.flexDirection = juce::FlexBox::Direction::column;
        flexBox.items.add (juce::FlexItem (knob.flex()).withFlex (3.0f));
        flexBox.items.add (juce::FlexItem (box).withFlex (1.0f).withMargin(juce::FlexItem::Margin(2.f, 0, 0, 0)));
        return flexBox;
    };

    auto modCxBox = createModControlBox(modCxAmountKnob, modCxSelectBox);
    fb.items.add (juce::FlexItem (modCxBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modCyBox = createModControlBox(modCyAmountKnob, modCySelectBox);
    fb.items.add (juce::FlexItem (modCyBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modRadiusBox = createModControlBox(modRadiusAmountKnob, modRadiusSelectBox);
    fb.items.add (juce::FlexItem (modRadiusBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modVolumeBox = createModControlBox(modVolumeAmountKnob, modVolumeSelectBox);
    fb.items.add (juce::FlexItem (modVolumeBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modFltFreqBox = createModControlBox(modFilterFreqAmountKnob, modFilterFreqSelectBox);
    fb.items.add (juce::FlexItem (modFltFreqBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modFltQBox = createModControlBox(modFilterQualityAmountKnob, modFilterQualitySelectBox);
    fb.items.add (juce::FlexItem (modFltQBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modPanBox = createModControlBox(modPanAmountKnob, modPanSelectBox);
    fb.items.add (juce::FlexItem (modPanBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modFreqBox = createModControlBox(modFreqAmountKnob, modFreqSelectBox);
    fb.items.add (juce::FlexItem (modFreqBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    fb.performLayout (bounds.toFloat());
}
