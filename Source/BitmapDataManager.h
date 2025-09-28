/*
  ==============================================================================

    BitmapDataManager.h
    Created: 22 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ImageBuffer.h"

/**
    Manages a juce::Image and its associated BitmapData, ensuring that the
    BitmapData is only created when the image changes, and providing thread-safe
    access for the audio thread.
*/
class BitmapDataManager : private juce::ChangeListener
{
public:
    BitmapDataManager (ImageBuffer& ib) : imageBuffer (ib)
    {
        imageBuffer.addChangeListener (this);
        updateBitmap();
    }

    ~BitmapDataManager() override
    {
        imageBuffer.removeChangeListener (this);
    }

    const juce::Image::BitmapData& getReadonlyBitmap() const
    {
        juce::ScopedLock lock (bitmapLock);
        return *bitmapData;
    }

private:
    void changeListenerCallback (juce::ChangeBroadcaster* source) override
    {
        if (source == &imageBuffer)
            updateBitmap(); // This is called on the message thread
    }

    void updateBitmap()
    {
        juce::ScopedLock lock (bitmapLock);
        image = imageBuffer.getImage();
        bitmapData = std::make_unique<juce::Image::BitmapData> (image, juce::Image::BitmapData::readOnly);
    }

    ImageBuffer& imageBuffer;
    juce::Image image; // Keep a copy to ensure BitmapData remains valid
    std::unique_ptr<juce::Image::BitmapData> bitmapData;
    juce::CriticalSection bitmapLock;
};