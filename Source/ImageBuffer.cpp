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
    if (! newImage.isValid())
    {
        const juce::ScopedLock lock (imageLock);
        image = {};
        sendChangeMessage();
        return;
    }

    juce::Image processedImage;
    const int w = newImage.getWidth();
    const int h = newImage.getHeight();

    if (w == h)
    {
        processedImage = newImage;
    }
    else
    {
        const int finalSize = juce::jmax (w, h);
        processedImage = juce::Image (juce::Image::ARGB, finalSize, finalSize, true);
        juce::Graphics g (processedImage);

        if (w > h) // Landscape image
        {
            const int yOffset = (finalSize - h) / 2;
            g.drawImageAt (newImage, 0, yOffset);

            // Mirrored top
            {
                g.saveState();
//                g.setClipRegion (juce::Rectangle<int> (0, 0, w, yOffset));
                auto transform = juce::AffineTransform::scale (1.0f, -1.0f)
                                                      .translated (0.0f, (float) yOffset);
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }

            // Mirrored bottom
            {
                g.saveState();
                //g.setClipRegion (juce::Rectangle<int> (0, yOffset + h, w, yOffset));
                auto transform = juce::AffineTransform::scale (1.0f, -1.0f)
                                                      .translated (0.0f, (float) (yOffset + 2 * h));
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }
        }
        else // Portrait image
        {
            const int xOffset = (finalSize - w) / 2;
            g.drawImageAt (newImage, xOffset, 0);

            // Mirrored left
            {
                g.saveState();
                //g.setClipRegion (juce::Rectangle<int> (0, 0, xOffset, h));
                auto transform = juce::AffineTransform::scale (-1.0f, 1.0f)
                                                      .translated ((float) xOffset, 0.0f);
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }

            // Mirrored right
            {
                g.saveState();
                //g.setClipRegion (juce::Rectangle<int> (xOffset + w, 0, xOffset, h));
                auto transform = juce::AffineTransform::scale (-1.0f, 1.0f)
                                                      .translated ((float) (xOffset + 2 * w), 0.0f);
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }
        }
    }

    const juce::ScopedLock lock (imageLock);
    image = processedImage;
    sendChangeMessage();
}

juce::Image ImageBuffer::getImage() const
{
    const juce::ScopedLock lock (imageLock);
    return image;
}