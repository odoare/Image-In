/*
  ==============================================================================

    ImageBuffer.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
    This class holds a juce::Image and provides thread-safe access to it.
    It acts as a ChangeBroadcaster to notify listeners when the image is updated.
*/
class ImageBuffer : public juce::ChangeBroadcaster
{
public:
    ImageBuffer();
    ~ImageBuffer() override;

    void setImage (const juce::Image& newImage);
    bool setImage (const juce::File& imageFile);
    juce::Image getImage() const;
    juce::File getFile() const;

private:
    juce::Image image;
    juce::File sourceFile;
    juce::CriticalSection imageLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageBuffer)
};