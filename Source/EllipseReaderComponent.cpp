/*
  ==============================================================================

    EllipseReaderComponent.cpp
    Created: 21 Sep 2025 10:00:00am
    Author:  Gemini Code Assist

  ==============================================================================
*/

#include "EllipseReaderComponent.h"
#include "PluginProcessor.h"

EllipseReaderComponent::EllipseReaderComponent(MapSynthAudioProcessor& p)
    : ReaderComponent(p),
    ellipseCxKnob(p.apvts, "EllipseCX", ELLIPSECOLOUR),
    ellipseCyKnob(p.apvts, "EllipseCY", ELLIPSECOLOUR),
    ellipseR1Knob(p.apvts, "EllipseR1", ELLIPSECOLOUR),
    ellipseR2Knob(p.apvts, "EllipseR2", ELLIPSECOLOUR),
    ellipseAngleKnob(p.apvts, "EllipseAngle", ELLIPSECOLOUR),
    ellipseVolumeKnob(p.apvts, "EllipseVolume", ELLIPSECOLOUR),
    filterFreqKnob(p.apvts, "EllipseFilterFreq", juce::Colours::yellow),
    filterQualityKnob(p.apvts, "EllipseFilterQuality", juce::Colours::yellow),
    panKnob(p.apvts, "EllipsePan", ELLIPSECOLOUR),
    modCx(p, "Mod_EllipseCX_Amount", "Mod_EllipseCX_Select"),
    modCy(p, "Mod_EllipseCY_Amount", "Mod_EllipseCY_Select"),
    modR1(p, "Mod_EllipseR1_Amount", "Mod_EllipseR1_Select"),
    modR2(p, "Mod_EllipseR2_Amount", "Mod_EllipseR2_Select"),
    modAngle(p, "Mod_EllipseAngle_Amount", "Mod_EllipseAngle_Select"),
    modVolume(p, "Mod_EllipseVolume_Amount", "Mod_EllipseVolume_Select"),
    modFilterFreq(p, "Mod_EllipseFilterFreq_Amount", "Mod_EllipseFilterFreq_Select"),
    modFilterQuality(p, "Mod_EllipseFilterQuality_Amount", "Mod_EllipseFilterQuality_Select"),
    modPan(p, "Mod_EllipsePan_Amount", "Mod_EllipsePan_Select"),
    modFreq(p, "Mod_EllipseFreq_Amount", "Mod_EllipseFreq_Select")
{
    setupKnob(ellipseCxKnob);
    setupKnob(ellipseCyKnob);
    setupKnob(ellipseR1Knob);
    setupKnob(ellipseR2Knob);
    setupKnob(ellipseAngleKnob);
    setupKnob(ellipseVolumeKnob);
    setupKnob(filterFreqKnob);
    setupKnob(filterQualityKnob);
    setupKnob(panKnob);

    addAndMakeVisible(modCx);
    addAndMakeVisible(modCy);
    addAndMakeVisible(modR1);
    addAndMakeVisible(modR2);
    addAndMakeVisible(modAngle);
    addAndMakeVisible(modVolume);
    addAndMakeVisible(modFilterFreq);
    addAndMakeVisible(modFilterQuality);
    addAndMakeVisible(modPan);
    addAndMakeVisible(modFreq);

    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItemList(filterTypeChoices, 1);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "EllipseFilterType", filterTypeBox);
}

void EllipseReaderComponent::paint(juce::Graphics& g)
{
    // The background is transparent, so no painting is needed here.
}

void EllipseReaderComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    juce::FlexBox fb;
    fb.flexWrap = juce::FlexBox::Wrap::wrap;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignContent = juce::FlexBox::AlignContent::spaceBetween;

    auto addItem = [&fb](juce::Component& c) {
        fb.items.add(juce::FlexItem(c).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    };

    addItem(ellipseCxKnob);    addItem(ellipseCyKnob);    addItem(ellipseR1Knob);
    addItem(ellipseR2Knob);    addItem(ellipseAngleKnob); addItem(ellipseVolumeKnob);
    addItem(panKnob);

    juce::FlexBox filterBox;
    filterBox.flexDirection = juce::FlexBox::Direction::column;
    filterBox.items.add(juce::FlexItem(filterFreqKnob).withFlex(3.0f));
    filterBox.items.add(juce::FlexItem(filterTypeBox).withFlex(1.0f).withMargin(juce::FlexItem::Margin(2.f, 0, 0, 0)));
    fb.items.add(juce::FlexItem(filterBox).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add(juce::FlexItem(filterQualityKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    fb.items.add(juce::FlexItem(panKnob).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));

    addItem(modCx);    addItem(modCy);    addItem(modR1);
    addItem(modR2);    addItem(modAngle); addItem(modVolume);
    addItem(modPan);   addItem(modFreq);  addItem(modFilterFreq);
    addItem(modFilterQuality);

    fb.performLayout(bounds.toFloat());
}