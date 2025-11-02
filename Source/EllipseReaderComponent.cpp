/*
  ==============================================================================

    EllipseReaderComponent.cpp
    Created: 21 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#include "EllipseReaderComponent.h"
#include "PluginProcessor.h"

EllipseReaderComponent::EllipseReaderComponent(MapSynthAudioProcessor& p, int readerIndex):
    ReaderComponent(p)
{
    juce::String idPrefix = "Ellipse" + juce::String(readerIndex) + "_";

    ellipseCxKnob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "CX", "X", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseCyKnob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "CY", "Y", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseR1Knob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "R1", "R1", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseR2Knob     = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "R2", "R2", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseAngleKnob  = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Angle", "Angle", ELLIPSECOLOURS[readerIndex - 1]);
    ellipseVolumeKnob = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Volume", "Vol", ELLIPSECOLOURS[readerIndex - 1]);
    filterFreqKnob    = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "FilterFreq", "Freq", ELLIPSECOLOURS[readerIndex - 1]);
    filterQualityKnob = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "FilterQuality", "Q", ELLIPSECOLOURS[readerIndex - 1]);
    panKnob           = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Pan", "Pan", ELLIPSECOLOURS[readerIndex - 1]);
    detuneKnob        = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Detune", "Detune", ELLIPSECOLOURS[readerIndex - 1]);

    modCx            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "CX_Amount", "Mod_" + idPrefix + "CX_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modCy            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "CY_Amount", "Mod_" + idPrefix + "CY_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modR1            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "R1_Amount", "Mod_" + idPrefix + "R1_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modR2            = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "R2_Amount", "Mod_" + idPrefix + "R2_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modAngle         = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Angle_Amount", "Mod_" + idPrefix + "Angle_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modVolume        = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Volume_Amount", "Mod_" + idPrefix + "Volume_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modFilterFreq    = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "FilterFreq_Amount", "Mod_" + idPrefix + "FilterFreq_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modFilterQuality = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "FilterQuality_Amount", "Mod_" + idPrefix + "FilterQuality_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modPan           = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Pan_Amount", "Mod_" + idPrefix + "Pan_Select", ELLIPSECOLOURS[readerIndex - 1]);
    modFreq          = std::make_unique<ModControlBox>(p, "Mod_" + idPrefix + "Freq_Amount", "Mod_" + idPrefix + "Freq_Select", ELLIPSECOLOURS[readerIndex - 1]);

    setupKnob(*ellipseCxKnob);
    setupKnob(*ellipseCyKnob);
    setupKnob(*ellipseR1Knob);
    setupKnob(*ellipseR2Knob);
    setupKnob(*ellipseAngleKnob);
    setupKnob(*ellipseVolumeKnob);
    setupKnob(*filterFreqKnob);
    setupKnob(*filterQualityKnob);
    setupKnob(*panKnob);
    setupKnob(*detuneKnob);
    detuneKnob->slider.setTextValueSuffix(" st");

    addAndMakeVisible(filterLabel);
    filterLabel.setText("Filter",juce::dontSendNotification);
    filterLabel.setJustificationType(juce::Justification::centred);
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
    filterTypeBox.setColour(juce::ComboBox::backgroundColourId,juce::Colours::transparentBlack);
    filterTypeBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    filterTypeBox.addItemList(filterTypeChoices, 1);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, idPrefix + "FilterType", filterTypeBox);

    addAndMakeVisible(midiChannelBox);
    midiChannelBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    midiChannelBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(p.apvts.getParameter(idPrefix + "MidiChannel")))
        midiChannelBox.addItemList(choiceParam->choices, 1);
    midiChannelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, idPrefix + "MidiChannel", midiChannelBox);

    onButton = std::make_unique<fxme::FxmeButton>(p.apvts, idPrefix + "On", "On", ELLIPSECOLOURS[readerIndex - 1]);
    addAndMakeVisible(*onButton);
    onButton->setLookAndFeel(&fxmeLookAndFeel);
    showMasterButton = std::make_unique<fxme::FxmeButton>(p.apvts, idPrefix + "ShowMaster", "Show Master", ELLIPSECOLOURS[readerIndex - 1]);
    addAndMakeVisible(*showMasterButton);
    showMasterButton->setLookAndFeel(&fxmeLookAndFeel);
    showLFOButton = std::make_unique<fxme::FxmeButton>(p.apvts, idPrefix + "ShowLFO", "Show LFO", ELLIPSECOLOURS[readerIndex - 1]);
    addAndMakeVisible(*showLFOButton);
    showLFOButton->setLookAndFeel(&fxmeLookAndFeel);
}

void EllipseReaderComponent::paint(juce::Graphics& g)
{
    // The background is transparent, so no painting is needed here.
}

void EllipseReaderComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    juce::FlexBox fbM, fbRow1, fbRow2, fbColumn1, fbColumn2, fbColumn3, fbR1, fbR2, fbRow3, fbRow4;
    fbM.flexDirection = juce::FlexBox::Direction::column;
    fbRow1.flexDirection = juce::FlexBox::Direction::row;
    fbRow2.flexDirection = juce::FlexBox::Direction::row;
    fbRow3.flexDirection = juce::FlexBox::Direction::row;
    fbRow4.flexDirection = juce::FlexBox::Direction::row;
    fbColumn1.flexDirection = juce::FlexBox::Direction::column;
    fbColumn2.flexDirection = juce::FlexBox::Direction::column;
    fbColumn3.flexDirection = juce::FlexBox::Direction::column;
    fbR1.flexDirection = juce::FlexBox::Direction::row;
    fbR2.flexDirection = juce::FlexBox::Direction::row;

    // auto addItem = [&fb](juce::Component& c) {
    //     fb.items.add(juce::FlexItem(c).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0f));
    // };

    using fi = juce::FlexItem;

    juce::FlexBox midiAndTogglesBox;
    midiAndTogglesBox.flexDirection = juce::FlexBox::Direction::row;
    midiAndTogglesBox.items.add(fi(*onButton).withFlex(1.f).withMargin(juce::FlexItem::Margin(0.f,10.f,0.f,10.f)));
    midiAndTogglesBox.items.add(fi(*showMasterButton).withFlex(1.f).withMargin(juce::FlexItem::Margin(0.f,10.f,0.f,10.f)));
    midiAndTogglesBox.items.add(fi(*showLFOButton).withFlex(1.f).withMargin(juce::FlexItem::Margin(0.f,10.f,0.f,10.f)));
    midiAndTogglesBox.items.add(fi(midiChannelBox).withFlex(1.f));

    fbRow1.items.add(fi(*ellipseCxKnob).withFlex(1.f));
    fbRow1.items.add(fi(*ellipseCyKnob).withFlex(1.f));
    fbRow1.items.add(fi(*ellipseR1Knob).withFlex(1.f));
    fbRow1.items.add(fi(*ellipseR2Knob).withFlex(1.f));
    fbRow1.items.add(fi(*ellipseAngleKnob).withFlex(1.f));
    fbRow1.performLayout(bounds);

    fbRow2.items.add(fi(*modCx).withFlex(1.f));
    fbRow2.items.add(fi(*modCy).withFlex(1.f));
    fbRow2.items.add(fi(*modR1).withFlex(1.f));
    fbRow2.items.add(fi(*modR2).withFlex(1.f));
    fbRow2.items.add(fi(*modAngle).withFlex(1.f));
    fbRow2.performLayout(bounds);
    
    fbColumn1.items.add(fi(filterLabel).withFlex(0.5f));
    fbColumn1.items.add(fi(*filterFreqKnob).withFlex(2.f));
    fbColumn1.items.add(fi(*modFilterFreq).withFlex(2.f));

    fbColumn2.items.add(fi(filterTypeBox).withFlex(0.5f));
    fbColumn2.items.add(fi(*filterQualityKnob).withFlex(2.f));
    fbColumn2.items.add(fi(*modFilterQuality).withFlex(2.f));

    fbR1.items.add(fi(fbColumn1).withFlex(1.0f));
    fbR1.items.add(fi(fbColumn2).withFlex(1.0f));
    
    fbRow3.items.add(fi(*detuneKnob).withFlex(1.f));
    fbRow3.items.add(fi(*panKnob).withFlex(1.f));
    fbRow3.items.add(fi(*ellipseVolumeKnob).withFlex(1.f));

    fbRow4.items.add(fi(*modFreq).withFlex(1.f));
    fbRow4.items.add(fi(*modPan).withFlex(1.f));
    fbRow4.items.add(fi(*modVolume).withFlex(1.f));

    fbColumn3.items.add(fi(fbRow3).withFlex(1.0f));
    fbColumn3.items.add(fi(fbRow4).withFlex(1.0f));

    fbR2.items.add(fi(fbR1).withFlex(1.7f));
    fbR2.items.add(fi(fbColumn3).withFlex(2.f));

    fbM.items.add(fi(midiAndTogglesBox).withFlex(.3f).withMargin(juce::FlexItem::Margin(0.f,0.f,10.f,0.f)));
    fbM.items.add(fi(fbRow1).withFlex(1.f));
    fbM.items.add(fi(fbRow2).withFlex(1.f));
    fbM.items.add(fi(fbR2).withFlex(2.f).withMargin(juce::FlexItem::Margin(10.f,0.f,0.f,0.f)));

    fbM.performLayout(bounds.toFloat());
}