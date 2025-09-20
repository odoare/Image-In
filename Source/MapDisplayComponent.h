/*
  ==============================================================================

    MapDisplayComponent.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <juce_opengl/juce_opengl.h>

#define CIRCLECOLOUR juce::Colours::blue
#define LINECOLOUR juce::Colours::red


class MapSynthAudioProcessor;

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
    MapDisplayComponent (MapSynthAudioProcessor& p);
    ~MapDisplayComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void renderOpenGL() override;

private:
    void timerCallback() override;
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    // New stuff for handle interaction
    enum class HandleType
    {
        None,
        LineCenter,
        LineEndpoint,
        CircleCenter,
        CircleRadius
    };

    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;

    juce::Rectangle<float> getHandleRect (juce::Point<float> center) const;
    HandleType getHandleAt (juce::Point<int> position);

    HandleType activeHandle { HandleType::None };
    const float handleSize = 10.0f;

    juce::Rectangle<int> displayArea;

    MapSynthAudioProcessor& processor;
    juce::OpenGLContext openGLContext;
    
    juce::Image lastImage;
    juce::OpenGLTexture imageTexture;

    juce::TextButton loadImageButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapDisplayComponent)
};