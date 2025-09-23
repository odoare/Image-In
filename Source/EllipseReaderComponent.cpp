/*
  ==============================================================================

    EllipseReaderComponent.cpp
    Created: 21 Sep 2025 10:00:00am
    Author:  Gemini Code Assist

  ==============================================================================
*/

#include "EllipseReaderComponent.h"
#include "PluginProcessor.h"

EllipseReaderComponent::EllipseReaderComponent(MapSynthAudioProcessor& p, int readerIndex)
    : ReaderComponent(p)
{
    juce::String idPrefix = "Ellipse" + juce::String(readerIndex) + "_";

    ellipseCxKnob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "CX", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseCyKnob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "CY", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseR1Knob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "R1", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseR2Knob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "R2", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseAngleKnob  = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Angle", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseVolumeKnob = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Volume", ELLIPSECOLOURS[readerIndex - 1]);
    filterFreqKnob    = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "FilterFreq", juce::Colours::yellow);
    filterQualityKnob = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "FilterQuality", juce::Colours::yellow);
    panKnob           = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Pan", ELLIPSECOLOURS[readerIndex - 1]);

    modCx            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "CX_Amount", "Mod_" + idPrefix + "CX_Select");
    modCy            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "CY_Amount", "Mod_" + idPrefix + "CY_Select");
    modR1            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "R1_Amount", "Mod_" + idPrefix + "R1_Select");
    modR2            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "R2_Amount", "Mod_" + idPrefix + "R2_Select");
    modAngle         = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Angle_Amount", "Mod_" + idPrefix + "Angle_Select");
    modVolume        = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Volume_Amount", "Mod_" + idPrefix + "Volume_Select");
    modFilterFreq    = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "FilterFreq_Amount", "Mod_" + idPrefix + "FilterFreq_Select");
    modFilterQuality = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "FilterQuality_Amount", "Mod_" + idPrefix + "FilterQuality_Select");
    modPan           = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Pan_Amount", "Mod_" + idPrefix + "Pan_Select");
    modFreq          = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Freq_Amount", "Mod_" + idPrefix + "Freq_Select");

    setupKnob(*ellipseCxKnob);
    setupKnob(*ellipseCyKnob);
    setupKnob(*ellipseR1Knob);
    setupKnob(*ellipseR2Knob);
    setupKnob(*ellipseAngleKnob);
    setupKnob(*ellipseVolumeKnob);
    setupKnob(*filterFreqKnob);
    setupKnob(*filterQualityKnob);
    setupKnob(*panKnob);

    addAndMakeVisible(*modCx);
    addAndMakeVisible(*modCy);
    addAndMakeVisible(*modR1);
    addAndMakeVisible(*modR2);
    addAndMakeVisible(*modAngle);
    addAndMakeVisible(*modVolume);
    addAndMakeVisible(*modFilterFreq);
    addAndMakeVisible(*modFilterQuality);
    addAndMakeVisible(*modPan);
    addAndMakeVisible(*modFreq);

    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItemList(filterTypeChoices, 1);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, idPrefix + "FilterType", filterTypeBox);
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
        fb.items.add(juce::FlexItem(c).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0f));
    };

    addItem(*ellipseCxKnob);    addItem(*ellipseCyKnob);    addItem(*ellipseR1Knob);
    addItem(*ellipseR2Knob);    addItem(*ellipseAngleKnob); addItem(*ellipseVolumeKnob);

    juce::FlexBox filterBox;
    filterBox.flexDirection = juce::FlexBox::Direction::column;
    filterBox.items.add(juce::FlexItem(*filterFreqKnob).withFlex(3.0f));
    filterBox.items.add(juce::FlexItem(filterTypeBox).withFlex(1.0f).withMargin(juce::FlexItem::Margin(2.f, 0, 0, 0)));
    fb.items.add(juce::FlexItem(filterBox).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0));
    addItem(*filterQualityKnob);
    addItem(*panKnob);

    addItem(*modCx);    addItem(*modCy);    addItem(*modR1);
    addItem(*modR2);    addItem(*modAngle); addItem(*modVolume);
    addItem(*modPan);   addItem(*modFreq);  addItem(*modFilterFreq);
    addItem(*modFilterQuality);

    fb.performLayout(bounds.toFloat());
}