/*
  ==============================================================================

    ImageBuffer.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "ImageBuffer.h"

ImageBuffer::ImageBuffer()
{
}

ImageBuffer::~ImageBuffer()
{
}

void ImageBuffer::setImage (const juce::Image& newImage)
{
    const juce::ScopedLock lock (imageLock);
    image = newImage;
    sendChangeMessage();
}

juce::Image ImageBuffer::getImage() const
{
    const juce::ScopedLock lock (imageLock);
    return image;
}