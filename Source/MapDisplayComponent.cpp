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

    addAndMakeVisible (loadImageButton);
    loadImageButton.setButtonText ("Load Image");
    loadImageButton.onClick = [this]
    {
        fileChooser = std::make_unique<juce::FileChooser> ("Select an image file...",
                                                           juce::File{},
                                                           "*.png,*.jpg,*.jpeg,*.gif");

        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                if (! oscillator.getImageBuffer().setImage (file))
                {
                    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon, "Image Load Error", "Could not load the image file: " + file.getFileName());
                }
            }
        });
    };
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
    auto bounds = getLocalBounds();
    int squareSize = juce::jmin (bounds.getWidth(), bounds.getHeight());
    displayArea = juce::Rectangle<int> (squareSize, squareSize).withCentre (bounds.getCentre());

    // Position the button within the square display area
    loadImageButton.setBounds (displayArea.getRight() - 80 - 10, displayArea.getY() + 10, 80, 24);

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

    if (displayArea.isEmpty())
        return;

    // Set the viewport to the square display area
    juce::gl::glViewport (displayArea.getX(), getHeight() - displayArea.getBottom(), displayArea.getWidth(), displayArea.getHeight());
    
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
                juce::gl::glTexCoord2f (0.0f, 0.0f); juce::gl::glVertex2f (-1.0f, -1.0f); // Bottom-Left
                juce::gl::glTexCoord2f (1.0f, 0.0f); juce::gl::glVertex2f ( 1.0f, -1.0f); // Bottom-Right
                juce::gl::glTexCoord2f (1.0f, 1.0f); juce::gl::glVertex2f ( 1.0f,  1.0f); // Top-Right
                juce::gl::glTexCoord2f (0.0f, 1.0f); juce::gl::glVertex2f (-1.0f,  1.0f); // Top-Left
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
            const float w = (float) displayArea.getWidth();
            const float h = (float) displayArea.getHeight();
            g.setColour (juce::Colours::white);
            g.drawLine (displayArea.getX() + lineReader->getX1() * w, displayArea.getY() + lineReader->getY1() * h,
                        displayArea.getX() + lineReader->getX2() * w, displayArea.getY() + lineReader->getY2() * h, 2.0f);
        }
        else if (const auto* circleReader = dynamic_cast<const CircleReader*> (readerBase))
        {
            g.setColour (juce::Colours::orange);
            const float w = (float) displayArea.getWidth();
            const float h = (float) displayArea.getHeight();
            const float radius = circleReader->getRadius() * juce::jmin (w, h);
            g.drawEllipse (displayArea.getX() + circleReader->getCX() * w - radius,
                           displayArea.getY() + circleReader->getCY() * h - radius,
                           radius * 2.0f, radius * 2.0f, 2.0f);
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