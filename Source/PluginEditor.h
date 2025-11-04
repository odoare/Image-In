/*
  ==============================================================================

    PluginEditor.h
    Created: 16 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
// #include <juce_gui_extra/juce_gui_extra.h>
// #include "PluginProcessor.h" // We will forward-declare to break the include cycle
class MapSynthAudioProcessor;

#include "MapDisplayComponent.h"
#include "EllipseReaderComponent.h"

class LFOsComponent;
class ADSRsComponent;

//==============================================================================
/**
*/
class MapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor,                                      
                                      public juce::AudioProcessorValueTreeState::Listener
{
public:
    MapSynthAudioProcessorEditor (MapSynthAudioProcessor&);
    ~MapSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void parameterChanged (const juce::String& parameterID, float newValue) override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MapSynthAudioProcessor& audioProcessor;

    fxme::FxmeLookAndFeel fxmeLookAndFeel;

    std::unique_ptr<MapDisplayComponent> mapDisplayComponentCPU;

    juce::TextButton fullscreenButton { "[]" };
    juce::Component* fullscreenComponent = nullptr;
    juce::TextButton togglePanelButton;

    EllipseReaderComponent ellipseReaderComponent1;
    EllipseReaderComponent ellipseReaderComponent2;
    EllipseReaderComponent ellipseReaderComponent3;
    std::unique_ptr<LFOsComponent> lfosComponent;
    std::unique_ptr<ADSRsComponent> adsrsComponent;

    juce::TabbedComponent readerTabs { juce::TabbedButtonBar::TabsAtLeft };

    juce::Label factoryImageLabel;
    juce::ComboBox factoryImageSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> factoryImageAttachment;

    juce::TextButton loadImageButton;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::TextButton importStateButton;
    juce::TextButton exportStateButton;

    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    fxme::HorizontalVuMeter meterL{[&]() { return audioProcessor.getMaxLevel(0); },
                                [&]() { return audioProcessor.getSmoothedMaxLevel(0); }},
                         meterR{[&]() { return audioProcessor.getMaxLevel(1); },
                                [&]() { return audioProcessor.getSmoothedMaxLevel(1); }};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessorEditor)
};
