/*
  ==============================================================================

    MapDisplayComponent.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "MapOscillator.h"
#include <juce_opengl/juce_opengl.h>

/**
    This component displays the image from an ImageBuffer and overlays the
    lines from one or more LineReaders.
*/
class MapDisplayComponent  : public juce::Component,
                             private juce::OpenGLRenderer,
                             public juce::ChangeListener,
                             private juce::Timer
{
public:
    MapDisplayComponent (MapOscillator& osc);
    ~MapDisplayComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void renderOpenGL() override;

private:
    void timerCallback() override;
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    MapOscillator& oscillator;
    juce::OpenGLContext openGLContext;
    
    juce::Image lastImage;
    juce::OpenGLTexture imageTexture;

    juce::TextButton loadImageButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapDisplayComponent)
};