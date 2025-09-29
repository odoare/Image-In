/*
  ==============================================================================

    BitmapDataManager.h
    Created: 24 May 2024 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ImageBuffer.h"

/**
    Manages a juce::Image::BitmapData object in a thread-safe way.
    It listens to an ImageBuffer and recreates the BitmapData when the image changes.
    This avoids creating BitmapData on the audio thread.
*/
class BitmapDataManager : private juce::ChangeListener
{
public:
    BitmapDataManager (ImageBuffer& bufferToFollow);
    ~BitmapDataManager() override;

    // Provides safe, RAII-style access to the BitmapData.
    class ScopedAccess
    {
    public:
        ScopedAccess (BitmapDataManager& manager);
        ~ScopedAccess();
        const juce::Image::BitmapData* operator->() const { return bitmapData; }
        const juce::Image::BitmapData* get() const { return bitmapData; }

    private:
        BitmapDataManager& owner;
        const juce::Image::BitmapData* bitmapData;
    };

private:
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void updateBitmap();

    ImageBuffer& imageBuffer;
    juce::Image sourceImage;
    std::unique_ptr<juce::Image::BitmapData> bitmapData;
    juce::CriticalSection bitmapLock;
};