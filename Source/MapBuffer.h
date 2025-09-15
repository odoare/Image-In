/*
  ==============================================================================

    MapBuffer.h
    Created: 10 Sep 2025 4:30:36pm
    Author:  doare

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MapBuffer
{
public:
    MapBuffer();
    ~MapBuffer();

    juce::Image& getImage();
    void setImage(const juce::Image& newImage);

    void setLineCoordinates(float startX, float startY, float endX, float endY);
    void setFrequency(float freq);
    float getFrequency() const;

    void prepareToPlay(double sampleRate);

    void playBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    juce::Image image;
    float x1,y1,x2,y2; // coordinates of the line to be played
    bool hasChanged;

private:
    
    double sampleRate; // sample rate of the audio processing
    float frequency; // frequency of the note to be played

    float phase = 0.0f;

    juce::LinearSmoothedValue<float> x1s, y1s, x2s, y2s;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapBuffer)
};

class MapBufferComponent : public juce::Component, public juce::Timer
{
public:
    MapBufferComponent(MapBuffer& mb);

    void paint(juce::Graphics& g) override;

private:
    void timerCallback() override;

    MapBuffer& mapBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapBufferComponent)
};
