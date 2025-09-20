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
      modLineCxAmountKnob(p.apvts, "Mod_LineCX_Amount", juce::Colours::hotpink),
      modLineCyAmountKnob(p.apvts, "Mod_LineCY_Amount", juce::Colours::hotpink),
      modLineAngleAmountKnob(p.apvts, "Mod_LineAngle_Amount", juce::Colours::hotpink),
      modLineLengthAmountKnob(p.apvts, "Mod_LineLength_Amount", juce::Colours::hotpink),
      modLineVolumeAmountKnob(p.apvts, "Mod_LineVolume_Amount", juce::Colours::hotpink),
      filterFreqKnob(p.apvts, "LineFilterFreq", juce::Colours::yellow),
      filterQualityKnob(p.apvts, "LineFilterQuality", juce::Colours::yellow),
      modFilterFreqAmountKnob(p.apvts, "Mod_LineFilterFreq_Amount", juce::Colours::hotpink),
      modFilterQualityAmountKnob(p.apvts, "Mod_LineFilterQuality_Amount", juce::Colours::hotpink),
      panKnob(p.apvts, "LinePan", COLOUR),
      modPanAmountKnob(p.apvts, "Mod_LinePan_Amount", juce::Colours::hotpink)
{
    setupKnob(lineCxKnob);
    setupKnob(lineCyKnob);
    setupKnob(lineLengthKnob);
    setupKnob(lineAngleKnob);
    setupKnob(lineVolumeKnob);
    setupKnob(modLineVolumeAmountKnob);

    setupKnob(modLineCxAmountKnob);
    setupKnob(modLineCyAmountKnob);
    setupKnob(modLineAngleAmountKnob);
    setupKnob(modLineLengthAmountKnob);

    setupKnob(filterFreqKnob);
    setupKnob(filterQualityKnob);
    setupKnob(modFilterFreqAmountKnob);
    setupKnob(modFilterQualityAmountKnob);
    setupKnob(panKnob);
    setupKnob(modPanAmountKnob);

    setupModulatorBox (modLineCxSelectBox, modLineCxSelectAttachment, "Mod_LineCX_Select");
    setupModulatorBox (modLineCySelectBox, modLineCySelectAttachment, "Mod_LineCY_Select");
    setupModulatorBox (modLineAngleSelectBox, modLineAngleSelectAttachment, "Mod_LineAngle_Select");
    setupModulatorBox (modLineLengthSelectBox, modLineLengthSelectAttachment, "Mod_LineLength_Select");
    setupModulatorBox (modLineVolumeSelectBox, modLineVolumeSelectAttachment, "Mod_LineVolume_Select");

    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItemList(filterTypeChoices, 1);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "LineFilterType", filterTypeBox);
    setupModulatorBox(modFilterFreqSelectBox, modFilterFreqSelectAttachment, "Mod_LineFilterFreq_Select");
    setupModulatorBox(modFilterQualitySelectBox, modFilterQualitySelectAttachment, "Mod_LineFilterQuality_Select");
    setupModulatorBox(modPanSelectBox, modPanSelectAttachment, "Mod_LinePan_Select");
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

    auto modCxBox = createModControlBox(modLineCxAmountKnob, modLineCxSelectBox);
    fb.items.add (juce::FlexItem (modCxBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modCyBox = createModControlBox(modLineCyAmountKnob, modLineCySelectBox);
    fb.items.add (juce::FlexItem (modCyBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modAngleBox = createModControlBox(modLineAngleAmountKnob, modLineAngleSelectBox);
    fb.items.add (juce::FlexItem (modAngleBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    auto modLengthBox = createModControlBox(modLineLengthAmountKnob, modLineLengthSelectBox);
    fb.items.add (juce::FlexItem (modLengthBox)
                    .withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    auto modVolumeBox = createModControlBox(modLineVolumeAmountKnob, modLineVolumeSelectBox);
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

    fb.performLayout (bounds.toFloat());
}
