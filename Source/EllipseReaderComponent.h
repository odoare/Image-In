/*
  ==============================================================================

    EllipseReaderComponent.h
    Created: 21 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include "ReaderComponent.h"
#include "ModControlBox.h"
#include "colours.h"

class EllipseReaderComponent : public ReaderComponent
{
public:
    EllipseReaderComponent(MapSynthAudioProcessor& p, int readerIndex);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    std::unique_ptr<fxme::FxmeKnob> ellipseCxKnob, ellipseCyKnob, ellipseR1Knob, ellipseR2Knob, ellipseAngleKnob, ellipseVolumeKnob,
                                    filterFreqKnob, filterQualityKnob,
                                    panKnob, detuneKnob;

    juce::Label filterLabel;

    juce::ComboBox filterTypeBox;
    juce::ComboBox midiChannelBox;

    std::unique_ptr<ModControlBox> modCx, modCy, modR1, modR2, modAngle, modVolume,
                                   modFilterFreq, modFilterQuality, modPan, modFreq;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> midiChannelAttachment;

    juce::ToggleButton onButton;
    juce::ToggleButton showMasterButton;
    juce::ToggleButton showLFOButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> onAttachment, showMasterAttachment, showLFOAttachment;
};