/*
  ==============================================================================

    BitmapDataManager.cpp
    Created: 24 May 2024 10:00:00am
    Author:  Gemini Code Assist

  ==============================================================================
*/

#include "BitmapDataManager.h"

BitmapDataManager::BitmapDataManager (ImageBuffer& bufferToFollow)
    : imageBuffer (bufferToFollow)
{
    imageBuffer.addChangeListener (this);
    updateBitmap();
}

BitmapDataManager::~BitmapDataManager()
{
    imageBuffer.removeChangeListener (this);
}

void BitmapDataManager::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &imageBuffer)
    {
        updateBitmap();
    }
}

void BitmapDataManager::updateBitmap()
{
    const juce::ScopedLock lock (bitmapLock);
    sourceImage = imageBuffer.getImage();
    if (sourceImage.isValid())
        bitmapData = std::make_unique<juce::Image::BitmapData> (sourceImage, juce::Image::BitmapData::readOnly);
    else
        bitmapData.reset();
}

// ==============================================================================
BitmapDataManager::ScopedAccess::ScopedAccess (BitmapDataManager& manager)
    : owner (manager)
{
    owner.bitmapLock.enter();
    bitmapData = owner.bitmapData.get();
}

BitmapDataManager::ScopedAccess::~ScopedAccess()
{
    owner.bitmapLock.exit();
}