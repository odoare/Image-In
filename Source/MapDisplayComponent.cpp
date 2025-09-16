/*
  ==============================================================================

    MapDisplayComponent.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "MapDisplayComponent.h"
#include "PluginProcessor.h"

MapDisplayComponent::MapDisplayComponent (MapSynthAudioProcessor& p)
    : processor (p)
{
    // Using OpenGL, so this component should be opaque.
    setOpaque (true);

    openGLContext.setRenderer (this);
    openGLContext.attachTo (*this);
    openGLContext.setContinuousRepainting (false); // We'll trigger repaints from a timer

    processor.imageBuffer.addChangeListener (this);

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
                if (! processor.imageBuffer.setImage (file))
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
    processor.imageBuffer.removeChangeListener (this);
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
    
    auto image = processor.imageBuffer.getImage();

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
    
    auto& apvts = processor.apvts;

    // Get latest LFO values from the audio thread
    const float lfo1Val = processor.lfo.getLatestValue();
    const float lfo2Val = processor.lfo2.getLatestValue();

    const float w = (float) displayArea.getWidth();
    const float h = (float) displayArea.getHeight();

    // Draw LineReader
    {
        const float cx_base = apvts.getRawParameterValue ("LineCX")->load();
        const float cy_base = apvts.getRawParameterValue ("LineCY")->load();
        const float length_base = apvts.getRawParameterValue ("LineLength")->load();
        const float angle_base = apvts.getRawParameterValue ("LineAngle")->load();

        // Get LFO modulation parameters
        const float lfoAngleAmount = apvts.getRawParameterValue ("LFO_LineAngle_Amount")->load();
        const bool  lfoAngleSelect = apvts.getRawParameterValue ("LFO_LineAngle_Select")->load() > 0.5f;
        const float lfoLengthAmount = apvts.getRawParameterValue ("LFO_LineLength_Amount")->load();
        const bool  lfoLengthSelect = apvts.getRawParameterValue ("LFO_LineLength_Select")->load() > 0.5f;

        // Apply LFO modulation
        const float lfoValForAngle = lfoAngleSelect ? lfo2Val : lfo1Val;
        const float angle = angle_base * (1.0f + 2.0f * (lfoAngleAmount - 0.5f) * (lfoValForAngle - 0.5f));

        const float lfoValForLength = lfoLengthSelect ? lfo2Val : lfo1Val;
        const float length = length_base * (1.0f + 2.0f * (lfoLengthAmount - 0.5f) * (lfoValForLength - 0.5f));

        const float halfLength = length * 0.5f;
        const float dx = halfLength * std::cos (angle);
        const float dy = halfLength * std::sin (angle);

        const float x1 = cx_base - dx;
        const float y1 = cy_base - dy;
        const float x2 = cx_base + dx;
        const float y2 = cy_base + dy;

        g.setColour (juce::Colours::white);
        g.drawLine (displayArea.getX() + x1 * w, displayArea.getY() + y1 * h,
                    displayArea.getX() + x2 * w, displayArea.getY() + y2 * h, 2.0f);
    }

    // Draw CircleReader
    {
        const float cx_base = apvts.getRawParameterValue ("CX")->load();
        const float cy_base = apvts.getRawParameterValue ("CY")->load();
        const float radius_base = apvts.getRawParameterValue ("R")->load();

        const float lfoCxAmount = apvts.getRawParameterValue("LFO_CX_Amount")->load();
        const bool lfoCxSelect = apvts.getRawParameterValue("LFO_CX_Select")->load() > 0.5f;
        const float lfoValForCx = lfoCxSelect ? lfo2Val : lfo1Val;
        const float cx = cx_base * (1.0f + 2.0f * (lfoCxAmount - 0.5f) * (lfoValForCx - 0.5f));

        const float lfoRadiusAmount = apvts.getRawParameterValue("LFO_R_Amount")->load();
        const bool lfoRadiusSelect = apvts.getRawParameterValue("LFO_R_Select")->load() > 0.5f;
        const float lfoValForRadius = lfoRadiusSelect ? lfo2Val : lfo1Val;
        const float radiusParam = radius_base * (1.0f + 2.0f * (lfoRadiusAmount - 0.5f) * (lfoValForRadius - 0.5f));
        
        g.setColour (juce::Colours::orange);
        const float radius = radiusParam * juce::jmin (w, h);
        g.drawEllipse (displayArea.getX() + cx * w - radius,
                       displayArea.getY() + cy_base * h - radius,
                       radius * 2.0f, radius * 2.0f, 2.0f);
    }
}

void MapDisplayComponent::timerCallback()
{
    openGLContext.triggerRepaint();
}

void MapDisplayComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &processor.imageBuffer)
    {
        // The image has changed. Invalidate our cached image so the texture
        // gets recreated on the next render pass.
        lastImage = juce::Image();
    }
}