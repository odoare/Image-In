/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_gui_extra/juce_gui_extra.h>
// #include "PluginProcessor.h" // We will forward-declare to break the include cycle
class MapSynthAudioProcessor;

#include "MapDisplayComponent.h"
#include "MapDisplayComponent_GL.h"
#include "LineReaderComponent.h"
#include "CircleReaderComponent.h"

class GlobalControlsComponent;

//==============================================================================
/**
*/
class MapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      private juce::AudioProcessorValueTreeState::Listener
{
public:
    MapSynthAudioProcessorEditor (MapSynthAudioProcessor&);
    ~MapSynthAudioProcessorEditor() override;

    //==============================================================================
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MapSynthAudioProcessor& audioProcessor;

    fxme::FxmeLookAndFeel fxmeLookAndFeel;

    std::unique_ptr<MapDisplayComponent> mapDisplayComponentCPU;
    std::unique_ptr<MapDisplayComponent_GL> mapDisplayComponentGL;

    LineReaderComponent lineReaderComponent;
    CircleReaderComponent circleReaderComponent;
    std::unique_ptr<GlobalControlsComponent> globalControlsComponent;

    juce::TabbedComponent readerTabs { juce::TabbedButtonBar::TabsAtLeft };

    juce::Label factoryImageLabel;
    juce::ComboBox factoryImageSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> factoryImageAttachment;

    juce::TextButton loadImageButton;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::ToggleButton useOpenGLButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> useOpenGLAttachment;

    void updateRendererVisibility();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessorEditor)
};
