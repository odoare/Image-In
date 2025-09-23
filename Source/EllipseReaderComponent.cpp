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
    filterFreqKnob    = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "FilterFreq", ELLIPSECOLOURS[readerIndex - 1]);
    filterQualityKnob = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "FilterQuality", ELLIPSECOLOURS[readerIndex - 1]);
    panKnob           = std::make_unique<fxme::FxmeKnob>(p.apvts, idPrefix + "Pan", ELLIPSECOLOURS[readerIndex - 1]);

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

    juce::FlexBox fbM, fbRow1, fbRow2, fbRow3, fbRow4;
    fbM.flexDirection = juce::FlexBox::Direction::column;
    fbRow1.flexDirection = juce::FlexBox::Direction::row;
    fbRow2.flexDirection = juce::FlexBox::Direction::row;
    fbRow3.flexDirection = juce::FlexBox::Direction::row;
    fbRow4.flexDirection = juce::FlexBox::Direction::row;

    // auto addItem = [&fb](juce::Component& c) {
    //     fb.items.add(juce::FlexItem(c).withMinWidth(70.0f).withMinHeight(70.0f).withFlex(1.0f));
    // };

    using fi = juce::FlexItem;
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

    const float comboBoxHeight = getLocalBounds().getHeight()*0.05;
    fbRow3.items.add(juce::FlexItem(filterTypeBox).withFlex(1.0f).withHeight(comboBoxHeight).withAlignSelf(juce::FlexItem::AlignSelf::center));
    fbRow3.items.add(fi(*filterFreqKnob).withFlex(1.f));
    fbRow3.items.add(fi(*filterQualityKnob).withFlex(1.f));
    fbRow3.items.add(fi(*panKnob).withFlex(1.f));
    fbRow3.items.add(fi(*ellipseVolumeKnob).withFlex(1.f));
    fbRow3.performLayout(bounds);

    fbRow4.items.add(fi(*modFreq).withFlex(1.f));
    fbRow4.items.add(fi(*modFilterFreq).withFlex(1.f));
    fbRow4.items.add(fi(*modFilterQuality).withFlex(1.f));
    fbRow4.items.add(fi(*modPan).withFlex(1.f));
    fbRow4.items.add(fi(*modVolume).withFlex(1.f));
    fbRow4.performLayout(bounds);

    fbM.items.add(fi(fbRow1).withFlex(1.f));
    fbM.items.add(fi(fbRow2).withFlex(1.f));
    fbM.items.add(fi(fbRow3).withFlex(1.f).withMargin(juce::FlexItem::Margin(10.f,0.f,0.f,0.f)));
    fbM.items.add(fi(fbRow4).withFlex(1.f));

    fbM.performLayout(bounds.toFloat());
}