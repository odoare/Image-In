/*
  ==============================================================================

    EllipseReaderComponent.h
    Created: 21 Sep 2025 10:00:00am
    Author:  Gemini Code Assist

  ==============================================================================
*/

#pragma once

#include "ReaderComponent.h"
#include "ModControlBox.h"
#include "colours.h"

class EllipseReaderComponent : public ReaderComponent
{
public:
    EllipseReaderComponent(MapSynthAudioProcessor& p);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    fxme::FxmeKnob ellipseCxKnob, ellipseCyKnob, ellipseR1Knob, ellipseR2Knob, ellipseAngleKnob, ellipseVolumeKnob,
        filterFreqKnob, filterQualityKnob,
        panKnob;

    juce::ComboBox filterTypeBox;

    ModControlBox modCx, modCy, modR1, modR2, modAngle, modVolume,
        modFilterFreq, modFilterQuality, modPan, modFreq;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
};