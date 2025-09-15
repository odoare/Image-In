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
                     lfoCxAmountKnob, lfoRadiusAmountKnob;

    fxme::FxmeButton lfoCxSelectButton, lfoRadiusSelectButton;
};