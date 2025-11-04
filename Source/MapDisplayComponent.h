/*
  ==============================================================================

    MapDisplayComponent.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "colours.h"

class MapSynthAudioProcessor;
class MapSynthAudioProcessorEditor;

/**
    This component displays the image from an ImageBuffer and overlays the
    lines from one or more LineReaders.
*/
class MapDisplayComponent  : public juce::Component,
                             public juce::ChangeListener,
                             private juce::Timer
{
public:
    MapDisplayComponent (MapSynthAudioProcessor& p);
    ~MapDisplayComponent() override;
    void paint (juce::Graphics& g) override;
    void setEditor(MapSynthAudioProcessorEditor* editor);
    void addButtons(juce::Component& toggleButton, juce::Component& fsButton);
    void resized() override;

private:
    void timerCallback() override;
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    // New stuff for handle interaction
    enum class HandleType
    {
        None,
        EllipseCenter,
        EllipseR1,
        EllipseR2
    };

    struct Handle
    {
        HandleType type { HandleType::None };
        int readerIndex { -1 };

        bool operator!= (const Handle& other) const { return type != other.type || readerIndex != other.readerIndex; }
    };

    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;

    juce::Rectangle<float> getHandleRect (juce::Point<float> center) const;
    Handle getHandleAt (juce::Point<int> position);

    Handle activeHandle;
    const float handleSize = 10.0f;

    juce::Rectangle<int> displayArea;

    MapSynthAudioProcessor& processor;
    MapSynthAudioProcessorEditor* editor = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapDisplayComponent)
};