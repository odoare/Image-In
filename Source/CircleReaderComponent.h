/*
  ==============================================================================

    CircleReaderComponent.h
    Created: 15 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include "ReaderComponent.h"
#include "ModControlBox.h"
#include "colours.h"

class CircleReaderComponent : public ReaderComponent
{
public:
    CircleReaderComponent (MapSynthAudioProcessor& p);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    fxme::FxmeKnob cxKnob, cyKnob, rKnob, circleVolumeKnob,
                     filterFreqKnob, filterQualityKnob,
                     panKnob;

    juce::ComboBox filterTypeBox;

    ModControlBox modCx, modCy, modRadius, modVolume,
                  modFilterFreq, modFilterQuality, modPan, modFreq;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
};