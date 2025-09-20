/*
  ==============================================================================

    CircleReaderComponent.h
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderComponent.h"

class CircleReaderComponent : public ReaderComponent
{
public:
    CircleReaderComponent (MapSynthAudioProcessor& p);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    fxme::FxmeKnob cxKnob, cyKnob, rKnob, circleVolumeKnob,
                     modCxAmountKnob, modCyAmountKnob, modRadiusAmountKnob, modVolumeAmountKnob,
                     filterFreqKnob, filterQualityKnob,
                     modFilterFreqAmountKnob, modFilterQualityAmountKnob,
                     panKnob, modPanAmountKnob;

    juce::ComboBox modCxSelectBox, modCySelectBox, modRadiusSelectBox, modVolumeSelectBox,
                   filterTypeBox,
                   modFilterFreqSelectBox, modFilterQualitySelectBox,
                   modPanSelectBox;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        modCxSelectAttachment, modCySelectAttachment, modRadiusSelectAttachment, modVolumeSelectAttachment,
        filterTypeAttachment,
        modFilterFreqSelectAttachment, modFilterQualitySelectAttachment,
        modPanSelectAttachment;
};