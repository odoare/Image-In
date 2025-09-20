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
    startTimerHz (30);

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
    g.addTransform (juce::AffineTransform::translation ((float) displayArea.getX(), (float) displayArea.getY()));
    
    auto& apvts = processor.apvts;

    // Get latest LFO values from the audio thread
    const float lfo1Val = processor.lfo.getLatestValue();
    const float lfo2Val = processor.lfo2.getLatestValue();

    const float w = (float) displayArea.getWidth();
    const float h = (float) displayArea.getHeight();

    auto getModVal = [&] (int select)
    {
        if (select < 2) // LFO 1 or LFO 2
            return (select == 0) ? lfo1Val : lfo2Val;
        return 0.5f; // For ADSRs, just show unmodulated for now
    };

    // Draw main (LFO-modulated only) paths in grey
    {
        g.setColour (juce::Colours::grey);
        const float cx_base = apvts.getRawParameterValue ("LineCX")->load();
        const float cy_base = apvts.getRawParameterValue ("LineCY")->load();
        const float length_base = apvts.getRawParameterValue ("LineLength")->load();
        const float angle_base = apvts.getRawParameterValue ("LineAngle")->load();

        // Get modulation parameters
        const float modCxAmount = apvts.getRawParameterValue ("Mod_LineCX_Amount")->load();
        const int   modCxSelect = (int) apvts.getRawParameterValue ("Mod_LineCX_Select")->load();
        const float modCyAmount = apvts.getRawParameterValue ("Mod_LineCY_Amount")->load();
        const int   modCySelect = (int) apvts.getRawParameterValue ("Mod_LineCY_Select")->load();
        const float modAngleAmount = apvts.getRawParameterValue ("Mod_LineAngle_Amount")->load();
        const int   modAngleSelect = (int) apvts.getRawParameterValue ("Mod_LineAngle_Select")->load();
        const float modLengthAmount = apvts.getRawParameterValue ("Mod_LineLength_Amount")->load();
        const int   modLengthSelect = (int) apvts.getRawParameterValue ("Mod_LineLength_Select")->load();

        // Apply LFO modulation
        const float modValForCx = getModVal (modCxSelect);
        const float cx = cx_base * (1.0f + modCxAmount * (modValForCx * 2.0f - 1.0f));

        const float modValForCy = getModVal (modCySelect);
        const float cy = cy_base * (1.0f + modCyAmount * (modValForCy * 2.0f - 1.0f));

        const float modValForAngle = getModVal (modAngleSelect);
        const float angle = angle_base * (1.0f + modAngleAmount * (modValForAngle * 2.0f - 1.0f));

        const float modValForLength = getModVal (modLengthSelect);
        const float length = length_base * (1.0f + modLengthAmount * (modValForLength * 2.0f - 1.0f));

        const float halfLength = length * 0.5f;
        const float dx = halfLength * std::cos (angle);
        const float dy = halfLength * std::sin (angle);

        const float x1 = cx - dx;
        const float y1 = cy - dy;
        const float x2 = cx + dx;
        const float y2 = cy + dy;

        g.drawLine (x1 * w, y1 * h, x2 * w, y2 * h, 2.0f);
    }
    {
        g.setColour (juce::Colours::grey);
        const float cx_base = apvts.getRawParameterValue ("CX")->load();
        const float cy_base = apvts.getRawParameterValue ("CY")->load();
        const float radius_base = apvts.getRawParameterValue ("R")->load();

        const float modCxAmount = apvts.getRawParameterValue("Mod_CircleCX_Amount")->load();
        const int modCxSelect = (int) apvts.getRawParameterValue("Mod_CircleCX_Select")->load();
        const float modValForCx = getModVal (modCxSelect);
        const float cx = cx_base * (1.0f + modCxAmount * (modValForCx * 2.0f - 1.0f));

        const float modCyAmount = apvts.getRawParameterValue("Mod_CircleCY_Amount")->load();
        const int modCySelect = (int) apvts.getRawParameterValue("Mod_CircleCY_Select")->load();
        const float modValForCy = getModVal (modCySelect);
        const float cy = cy_base * (1.0f + modCyAmount * (modValForCy * 2.0f - 1.0f));

        const float modRadiusAmount = apvts.getRawParameterValue("Mod_CircleRadius_Amount")->load();
        const int modRadiusSelect = (int) apvts.getRawParameterValue("Mod_CircleRadius_Select")->load();
        const float modValForRadius = getModVal (modRadiusSelect);
        const float radiusParam = radius_base * (1.0f + modRadiusAmount * (modValForRadius * 2.0f - 1.0f));
        
        const float radius = radiusParam * juce::jmin (w, h);
        g.drawEllipse (cx * w - radius,
                       cy * h - radius,
                       radius * 2.0f, radius * 2.0f, 2.0f);
    }

    // Draw per-voice paths
    std::array<VoiceDisplayState, NUM_VOICES> statesCopy;
    {
        juce::ScopedLock lock (processor.displayStateLock);
        statesCopy = processor.voiceDisplayStates;
    }

    for (const auto& voiceState : statesCopy)
    {
        if (! voiceState.isActive)
            continue;

        for (const auto& readerInfo : voiceState.readerInfos)
        {
            if (readerInfo.type == ReaderBase::Type::Line)
            {
                g.setColour (juce::Colours::white.withAlpha (readerInfo.volume));
                g.drawLine (readerInfo.x1 * w, readerInfo.y1 * h,
                            readerInfo.x2 * w, readerInfo.y2 * h, 2.0f);
            }
            else if (readerInfo.type == ReaderBase::Type::Circle)
            {
                g.setColour (juce::Colours::orange.withAlpha (readerInfo.volume));
                const float radius = readerInfo.radius * juce::jmin (w, h);
                g.drawEllipse (readerInfo.cx * w - radius,
                               readerInfo.cy * h - radius,
                               radius * 2.0f, radius * 2.0f, 2.0f);
            }
        }
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