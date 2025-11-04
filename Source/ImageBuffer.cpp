/*
  ==============================================================================

    ImageBuffer.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

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
        sourceFile = juce::File();
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
            g.drawImageAt (newImage, 0, yOffset); // Draw the original image in the center

            // Mirrored top
            {
                g.saveState();
                auto transform = juce::AffineTransform::scale (1.0f, -1.0f)
                                                      .translated (0.0f, (float)yOffset);
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }

            // Mirrored bottom
            {
                g.saveState();
                auto transform = juce::AffineTransform::scale (1.0f, -1.0f)
                                                      .translated (0.0f, (float)(yOffset + 2 * h));
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }
        }
        else // Portrait image
        {
            const int xOffset = (finalSize - w) / 2;
            g.drawImageAt (newImage, xOffset, 0); // Draw the original image in the center

            // Mirrored left
            {
                g.saveState();
                auto transform = juce::AffineTransform::scale (-1.0f, 1.0f)
                                                      .translated ((float)xOffset, 0.0f);
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }

            // Mirrored right
            {
                g.saveState();
                auto transform = juce::AffineTransform::scale (-1.0f, 1.0f)
                                                      .translated ((float)(xOffset + 2 * w), 0.0f);
                g.drawImageTransformed (newImage, transform);
                g.restoreState();
            }
        }
    }

    const juce::ScopedLock lock (imageLock);
    image = processedImage;    
    sourceFile = juce::File(); // An image from memory has no source file path
    sendChangeMessage();
}

bool ImageBuffer::setImage (const juce::File& imageFile)
{
    juce::Image newImage = juce::ImageFileFormat::loadFrom (imageFile);

    if (newImage.isValid())
    {
        // This will process the image and also clear the sourceFile path
        setImage (newImage);

        // We restore the path immediately after
        const juce::ScopedLock lock (imageLock);
        sourceFile = imageFile;
        return true;
    }

    return false;
}

juce::Image ImageBuffer::getImage() const
{
    const juce::ScopedLock lock (imageLock);
    return image;
}
juce::File ImageBuffer::getFile() const
{
    const juce::ScopedLock lock (imageLock);
    return sourceFile;
}