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
#include "LineReaderComponent.h"
#include "CircleReaderComponent.h"

class GlobalControlsComponent;

//==============================================================================
/**
*/
class MapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MapSynthAudioProcessorEditor (MapSynthAudioProcessor&);
    ~MapSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MapSynthAudioProcessor& audioProcessor;

    fxme::FxmeLookAndFeel fxmeLookAndFeel;

    MapDisplayComponent mapDisplayComponent;

    LineReaderComponent lineReaderComponent;
    CircleReaderComponent circleReaderComponent;
    std::unique_ptr<GlobalControlsComponent> globalControlsComponent;

    juce::TabbedComponent readerTabs { juce::TabbedButtonBar::TabsAtLeft };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessorEditor)
};
