/*
  ==============================================================================

    ModControlBox.h
    Created: 18 Sep 2025 11:00:00am
    Author:  Gemini Code Assist

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ParameterStructs.h"

class MapSynthAudioProcessor;

class ModControlBox : public juce::Component
{
public:
    ModControlBox(MapSynthAudioProcessor& p,
                  const juce::String& modAmountParamId,
                  const juce::String& modSelectParamId);

    ~ModControlBox() override = default;

    void resized() override;

private:
    fxme::FxmeLookAndFeel fxmeLookAndFeel;
    fxme::FxmeKnob amountKnob;
    juce::ComboBox selectBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> selectAttachment;
};