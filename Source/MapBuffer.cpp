/*
  ==============================================================================

    MapBuffer.cpp
    Created: 10 Sep 2025 4:30:36pm
    Author:  doare

  ==============================================================================
*/

#include "MapBuffer.h"

MapBuffer::MapBuffer() : x1(0.2f), y1(0.2f), x2(0.8f), y2(0.8f), sampleRate(44100.0), frequency(440.0f)
{
}

MapBuffer::~MapBuffer()
{
}

juce::Image& MapBuffer::getImage()
{
    return image;
}

void MapBuffer::setImage(const juce::Image& newImage)
{
    image = newImage;
}

// juce::Image::BitmapData MapBuffer::getBitmapData(juce::Image::BitmapData::ReadWriteMode mode)
// {
//     return juce::Image::BitmapData(image, mode);
// }

void MapBuffer::setLineCoordinates(float startX, float startY, float endX, float endY)
{
    if (startX==x1 && startY==y1 && endX==x2 && endY==y2)
    {
        return;
    }

    if (startX!=x1)
    {
        x1 = juce::jlimit(0.0f, 1.0f, startX);
        x1s.setTargetValue(x1);
    }

    if (startY != y1)
    {
        y1 = juce::jlimit(0.0f, 1.0f, startY);
        y1s.setTargetValue(y1);
    }

    if (endX != x2)
    {
        x2 = juce::jlimit(0.0f, 1.0f, endX);
        x2s.setTargetValue(x2);
    }

    if (endY != y2)
    {
        y2 = juce::jlimit(0.0f, 1.0f, endY);
        y2s.setTargetValue(y2);
    }

    hasChanged = true;

}

void MapBuffer::setFrequency(float freq)
{
    frequency = freq;
}

float MapBuffer::getFrequency() const
{
    return frequency;
}

void MapBuffer::prepareToPlay(double sr)
{
    sampleRate = sr;
    x1s.reset(1000); x1s.setCurrentAndTargetValue(x1);
    y1s.reset(1000); y1s.setCurrentAndTargetValue(y1);
    x2s.reset(1000); x2s.setCurrentAndTargetValue(x2);
    y2s.reset(1000); y2s.setCurrentAndTargetValue(y2);
}

void MapBuffer::playBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    juce::Image::BitmapData bitmapData(image, juce::Image::BitmapData::readOnly);

    if (bitmapData.data == nullptr || bitmapData.width == 0 || bitmapData.height == 0)
        return; // No valid image data

    int numChannels = buffer.getNumChannels();

    float phaseIncrement = frequency / (float)sampleRate;

    for (int sample = startSample; sample < startSample + numSamples; ++sample)
    {
        // Update smoothed coordinates
        float x1sv = x1s.getNextValue();
        float y1sv = y1s.getNextValue();
        float x2sv = x2s.getNextValue();
        float y2sv = y2s.getNextValue();

        float totalDistance = std::sqrt((x2sv - x1sv) * (x2sv - x1sv) + (y2sv - y1sv) * (y2sv - y1sv));

        if (totalDistance <= 0.0f)
        {
            buffer.clear(sample, 1);
            continue;
        }

        // Calculate current position on the line based on phase
        float currentX = x1sv + (x2sv - x1sv) * phase;
        float currentY = y1sv + (y2sv - y1sv) * phase;

        // Calculate pixel position
        float pixelX = (currentX * (bitmapData.width - 1));
        float pixelY = (currentY * (bitmapData.height - 1));

        // Clamp to image bounds
        pixelX = juce::jlimit(0.0f, (float)(bitmapData.width - 1), pixelX);
        pixelY = juce::jlimit(0.0f, (float)(bitmapData.height - 1), pixelY);

        // Get pixel color by interpolating between pixels if necessary
        float sampleValue;
        int pixelXInt = static_cast<int>(std::floor(pixelX));
        int pixelYInt = static_cast<int>(std::floor(pixelY));
        float pixelXFrac = pixelX - pixelXInt;
        float pixelYFrac = pixelY - pixelYInt;

        if (pixelXFrac == 0.0f && pixelYFrac == 0.0f)
        {
            sampleValue = (bitmapData.getPixelColour(pixelXInt, pixelYInt)).getBrightness() * 2.0f - 1.0f;
        }
        else
        {
            float colour1 = (bitmapData.getPixelColour(pixelXInt, pixelYInt)).getBrightness() * 2.0f - 1.0f;
            float colour2 = (bitmapData.getPixelColour(pixelXInt + 1, pixelYInt)).getBrightness() * 2.0f - 1.0f;
            float colour3 = (bitmapData.getPixelColour(pixelXInt, pixelYInt + 1)).getBrightness() * 2.0f - 1.0f;
            float colour4 = (bitmapData.getPixelColour(pixelXInt + 1, pixelYInt + 1)).getBrightness() * 2.0f - 1.0f;

            sampleValue = colour1 * (1.0f - pixelXFrac) * (1.0f - pixelYFrac)
                        + colour2 * pixelXFrac * (1.0f - pixelYFrac)
                        + colour3 * (1.0f - pixelXFrac) * pixelYFrac
                        + colour4 * pixelXFrac * pixelYFrac;
        }

        // Write to all channels
        for (int channel = 0; channel < numChannels; ++channel)
            buffer.setSample(channel, sample, sampleValue);

        phase += phaseIncrement;
        phase = std::fmod(phase, 1.0f);
    }
}

MapBufferComponent::MapBufferComponent(MapBuffer& mb) : mapBuffer(mb)
{
    startTimerHz(30); // Check for updates at 30 Hz
    
}

void MapBufferComponent::paint(juce::Graphics& g)
{
    g.drawImage(mapBuffer.image, getLocalBounds().toFloat());
    g.drawLine(mapBuffer.x1 * getWidth(), mapBuffer.y1 * getHeight(),
               mapBuffer.x2 * getWidth(), mapBuffer.y2 * getHeight(),
               2.0f);
}

void MapBufferComponent::timerCallback()
{
    if (mapBuffer.hasChanged)
    {
        mapBuffer.hasChanged = false;
        repaint();
    }
}
