/*
  ==============================================================================

    LineReaderComponent.h
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderComponent.h"

#define COLOUR juce::Colour::fromRGB(100, 100, 250)

class LineReaderComponent : public ReaderComponent
{
public:
    LineReaderComponent (MapSynthAudioProcessor& p);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    fxme::FxmeKnob lineCxKnob, lineCyKnob, lineLengthKnob, lineAngleKnob, lineVolumeKnob,
                     modLineCxAmountKnob, modLineCyAmountKnob, modLineAngleAmountKnob, modLineLengthAmountKnob, modLineVolumeAmountKnob,
                     filterFreqKnob, filterQualityKnob,
                     modFilterFreqAmountKnob, modFilterQualityAmountKnob;

    juce::ComboBox modLineCxSelectBox, modLineCySelectBox, modLineAngleSelectBox, modLineLengthSelectBox, modLineVolumeSelectBox,
                   filterTypeBox,
                   modFilterFreqSelectBox, modFilterQualitySelectBox;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        modLineCxSelectAttachment, modLineCySelectAttachment, modLineAngleSelectAttachment, modLineLengthSelectAttachment, modLineVolumeSelectAttachment,
        filterTypeAttachment,
        modFilterFreqSelectAttachment, modFilterQualitySelectAttachment;
};