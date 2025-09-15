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
                     lfoLineAngleAmountKnob, lfoLineLengthAmountKnob;

    fxme::FxmeButton lfoLineAngleSelectButton, lfoLineLengthSelectButton;
};