/*
  ==============================================================================

    MapDisplayComponent.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "MapDisplayComponent.h"

MapDisplayComponent::MapDisplayComponent (MapOscillator& osc)
    : oscillator (osc)
{
    // Using OpenGL, so this component should be opaque.
    setOpaque (true);

    openGLContext.setRenderer (this);
    openGLContext.attachTo (*this);
    openGLContext.setContinuousRepainting (false); // We'll trigger repaints from a timer

    oscillator.getImageBuffer().addChangeListener (this);

    // We don't need to listen to the readers anymore, as we are repainting
    // on a timer for smooth animation.
    startTimerHz (60);
}

MapDisplayComponent::~MapDisplayComponent()
{
    stopTimer();
    openGLContext.detach();
    oscillator.getImageBuffer().removeChangeListener (this);
}

void MapDisplayComponent::paint (juce::Graphics& g)
{
    // This method is intentionally left blank.
    // All rendering is now handled by renderOpenGL().
}

void MapDisplayComponent::resized()
{
    openGLContext.triggerRepaint();
}

void MapDisplayComponent::newOpenGLContextCreated()
{
    // One-time OpenGL setup can go here.
}

void MapDisplayComponent::openGLContextClosing()
{
    // Release any OpenGL resources before the context is destroyed.
    imageTexture.release();
}

void MapDisplayComponent::renderOpenGL()
{
    juce::OpenGLHelpers::clear (juce::Colours::darkgrey);
    
    auto image = oscillator.getImageBuffer().getImage();

    if (image.isValid())
    {
        // If the image has changed since last frame, create a new GPU texture
        if (image != lastImage)
        {
            lastImage = image;
            imageTexture.release();
            imageTexture.loadImage (lastImage);
        }

        // If the texture is valid, draw it.
        if (imageTexture.getTextureID() != 0)
        {
            imageTexture.bind();

            // Draw a quad that fills the whole component
            juce::gl::glBegin (juce::gl::GL_QUADS);
                juce::gl::glTexCoord2f (0.0f, 1.0f); juce::gl::glVertex2f (-1.0f, -1.0f); // Bottom-Left
                juce::gl::glTexCoord2f (1.0f, 1.0f); juce::gl::glVertex2f ( 1.0f, -1.0f); // Bottom-Right
                juce::gl::glTexCoord2f (1.0f, 0.0f); juce::gl::glVertex2f ( 1.0f,  1.0f); // Top-Right
                juce::gl::glTexCoord2f (0.0f, 0.0f); juce::gl::glVertex2f (-1.0f,  1.0f); // Top-Left
            juce::gl::glEnd();
        }
    }

    // Create a juce::Graphics to draw our 2D overlays on top of the texture
    std::unique_ptr<juce::LowLevelGraphicsContext> glContext (createOpenGLGraphicsContext (openGLContext, getWidth(), getHeight()));
    juce::Graphics g (*glContext);
    
    for (const auto* readerBase : oscillator.getReaders())
    {
        if (const auto* lineReader = dynamic_cast<const LineReader*> (readerBase))
        {
            g.setColour (juce::Colours::white);
            g.drawLine (lineReader->getX1() * getWidth(), lineReader->getY1() * getHeight(), lineReader->getX2() * getWidth(), lineReader->getY2() * getHeight(), 2.0f);
        }
        else if (const auto* circleReader = dynamic_cast<const CircleReader*> (readerBase))
        {
            g.setColour (juce::Colours::orange);
            const float w = (float) getWidth();
            const float h = (float) getHeight();
            const float radius = circleReader->getRadius() * juce::jmin (w, h);
            g.drawEllipse (circleReader->getCX() * w - radius, circleReader->getCY() * h - radius, radius * 2.0f, radius * 2.0f, 2.0f);
        }
    }
}

void MapDisplayComponent::timerCallback()
{
    openGLContext.triggerRepaint();
}

void MapDisplayComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &oscillator.getImageBuffer())
    {
        // The image has changed. Invalidate our cached image so the texture
        // gets recreated on the next render pass.
        lastImage = juce::Image();
    }
}