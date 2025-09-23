/*
  ==============================================================================

    LineReaderComponent.h
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ReaderComponent.h"
#include "ModControlBox.h"
#include "colours.h"

class LineReaderComponent : public ReaderComponent
{
public:
    LineReaderComponent (MapSynthAudioProcessor& p);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    fxme::FxmeKnob lineCxKnob, lineCyKnob, lineLengthKnob, lineAngleKnob, lineVolumeKnob,
                     filterFreqKnob, filterQualityKnob,
                     panKnob;

    juce::ComboBox filterTypeBox;

    ModControlBox modCx, modCy, modAngle, modLength, modVolume,
                  modFilterFreq, modFilterQuality, modPan, modFreq;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
};