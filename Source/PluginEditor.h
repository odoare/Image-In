/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
// #include <juce_gui_extra/juce_gui_extra.h>
// #include "PluginProcessor.h" // We will forward-declare to break the include cycle
class MapSynthAudioProcessor;

#include "MapDisplayComponent.h"
#include "MapDisplayComponent_GL.h"
#include "EllipseReaderComponent.h"

class LFOsComponent;
class ADSRsComponent;

//==============================================================================
/**
*/
class MapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::ChangeListener
{
public:
    MapSynthAudioProcessorEditor (MapSynthAudioProcessor&);
    ~MapSynthAudioProcessorEditor() override;

    //==============================================================================
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MapSynthAudioProcessor& audioProcessor;

    fxme::FxmeLookAndFeel fxmeLookAndFeel;

    std::unique_ptr<MapDisplayComponent> mapDisplayComponentCPU;
    std::unique_ptr<MapDisplayComponent_GL> mapDisplayComponentGL;

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
    juce::ToggleButton useOpenGLButton;
    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    fxme::HorizontalVuMeter meterL{[&]() { return audioProcessor.getMaxLevel(0); },
                                [&]() { return audioProcessor.getSmoothedMaxLevel(0); }},
                         meterR{[&]() { return audioProcessor.getMaxLevel(1); },
                                [&]() { return audioProcessor.getSmoothedMaxLevel(1); }};

    void updateRendererVisibility();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessorEditor)
};
