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
    setOpaque (true);

    processor.imageBuffer.addChangeListener (this);

    // We don't need to listen to the readers anymore, as we are repainting
    // on a timer for smooth animation.
    startTimerHz (30);
}

MapDisplayComponent::~MapDisplayComponent()
{
    stopTimer();
    processor.imageBuffer.removeChangeListener (this);
}

void MapDisplayComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);

    if (displayArea.isEmpty())
        return;
    
    auto image = processor.imageBuffer.getImage();

    if (image.isValid())
    {
        g.drawImage(image, displayArea.toFloat());
    }

    juce::Graphics::ScopedSaveState s(g);
    g.setOrigin(displayArea.getPosition());
    
    auto& apvts = processor.apvts;

    // Get latest LFO values from the audio thread
    const float lfo1Val = processor.lfo.getLatestValue();
    const float lfo2Val = processor.lfo2.getLatestValue();
    const float lfo3Val = processor.lfo3.getLatestValue();
    const float lfo4Val = processor.lfo4.getLatestValue();

    const float w = (float) displayArea.getWidth();
    const float h = (float) displayArea.getHeight();

    auto getModVal = [&] (int select)
    {
        switch (select)
        {
            case 0: return lfo1Val;
            case 1: return lfo2Val;
            case 2: return lfo3Val;
            case 3: return lfo4Val;
            case 4: // ADSR 1 - not visualized yet
            case 5: // ADSR 2 - not visualized yet
            default: return 0.5f; // For ADSRs, just show unmodulated for now
        }
    };


    // Draw main (LFO-modulated only) paths
    {
        g.setColour (LFOMODULATEDCOLOUR);
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
        g.setColour (LFOMODULATEDCOLOUR);
        const float cx_base = apvts.getRawParameterValue ("CX")->load();
        const float cy_base = apvts.getRawParameterValue ("CY")->load();
        const float radius_base_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("R"))->get();

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
        const float radiusParam = radius_base_actual * (1.0f + modRadiusAmount * (modValForRadius * 2.0f - 1.0f));
        
        const float radius = radiusParam * juce::jmin (w, h);
        g.drawEllipse (cx * w - radius,
                       cy * h - radius,
                       radius * 2.0f, radius * 2.0f, 2.0f);
    }

    // Draw static unmodulated paths
    {
        g.setColour (LINECOLOUR);
        const float cx_base = apvts.getRawParameterValue ("LineCX")->load();
        const float cy_base = apvts.getRawParameterValue ("LineCY")->load();
        const float length_base = apvts.getRawParameterValue ("LineLength")->load();
        const float angle_base = apvts.getRawParameterValue ("LineAngle")->load();

        const float halfLength = length_base * 0.5f;
        const float dx = halfLength * std::cos (angle_base);
        const float dy = halfLength * std::sin (angle_base);

        const float x1 = cx_base - dx;
        const float y1 = cy_base - dy;
        const float x2 = cx_base + dx;
        const float y2 = cy_base + dy;

        g.drawLine (x1 * w, y1 * h, x2 * w, y2 * h, 2.0f);
    }
    {
        g.setColour (CIRCLECOLOUR);
        const float cx_base = apvts.getRawParameterValue ("CX")->load();
        const float cy_base = apvts.getRawParameterValue ("CY")->load();
        const float radius_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("R"))->get();

        const float radius = radius_actual * juce::jmin (w, h);
        g.drawEllipse (cx_base * w - radius,
                       cy_base * h - radius,
                       radius * 2.0f, radius * 2.0f, 2.0f);
    }

    // Draw handles
    { // Line handles
        g.setColour (LINECOLOUR.withAlpha (0.7f));
        const float cx_base = apvts.getRawParameterValue ("LineCX")->load();
        const float cy_base = apvts.getRawParameterValue ("LineCY")->load();
        const float length_base = apvts.getRawParameterValue ("LineLength")->load();
        const float angle_base = apvts.getRawParameterValue ("LineAngle")->load();

        const float halfLength = length_base * 0.5f;
        const float dx = halfLength * std::cos (angle_base);
        const float dy = halfLength * std::sin (angle_base);

        const float x2 = cx_base + dx;
        const float y2 = cy_base + dy;

        // Center handle
        g.fillRect (getHandleRect ({cx_base * w, cy_base * h}));
        // Endpoint handle
        g.fillRect (getHandleRect ({x2 * w, y2 * h}));
    }
    { // Circle handles
        g.setColour (CIRCLECOLOUR.withAlpha (0.7f));
        const float cx_base = apvts.getRawParameterValue ("CX")->load();
        const float cy_base = apvts.getRawParameterValue ("CY")->load();
        const float radius_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("R"))->get();
        const float radius_pixels = radius_actual * juce::jmin(w, h);

        // Center handle
        g.fillRect (getHandleRect ({cx_base * w, cy_base * h}));
        // Radius handle (on the right edge of the circle)
        g.fillRect (getHandleRect ({cx_base * w + radius_pixels, cy_base * h}));
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
                g.setColour (LINECOLOUR.withAlpha (readerInfo.volume));
                g.drawLine (readerInfo.x1 * w, readerInfo.y1 * h,
                            readerInfo.x2 * w, readerInfo.y2 * h, 2.0f);
            }
            else if (readerInfo.type == ReaderBase::Type::Circle)
            {
                g.setColour (CIRCLECOLOUR.withAlpha (readerInfo.volume));
                const float radius = readerInfo.radius * juce::jmin (w, h);
                g.drawEllipse (readerInfo.cx * w - radius,
                               readerInfo.cy * h - radius,
                               radius * 2.0f, radius * 2.0f, 2.0f);
            }
        }
    }
}

void MapDisplayComponent::resized()
{
    auto bounds = getLocalBounds();
    int squareSize = juce::jmin (bounds.getWidth(), bounds.getHeight());
    displayArea = juce::Rectangle<int> (squareSize, squareSize).withCentre (bounds.getCentre());

    repaint();
}

void MapDisplayComponent::mouseDown (const juce::MouseEvent& event)
{
    if (! displayArea.contains (event.getPosition()))
        return;

    activeHandle = getHandleAt (event.getPosition());

    if (activeHandle != HandleType::None)
    {
        // Begin a parameter change gesture for automation recording in the host
        processor.apvts.getParameter("LineCX")->beginChangeGesture();
        processor.apvts.getParameter("LineCY")->beginChangeGesture();
        processor.apvts.getParameter("LineLength")->beginChangeGesture();
        processor.apvts.getParameter("LineAngle")->beginChangeGesture();
        processor.apvts.getParameter("CX")->beginChangeGesture();
        processor.apvts.getParameter("CY")->beginChangeGesture();
        processor.apvts.getParameter("R")->beginChangeGesture();
    }
}

void MapDisplayComponent::mouseDrag (const juce::MouseEvent& event)
{
    if (activeHandle == HandleType::None)
        return;

    auto& apvts = processor.apvts;
    auto relativePos = event.getEventRelativeTo(this).getPosition() - displayArea.getPosition();
    
    const float w = (float) displayArea.getWidth();
    const float h = (float) displayArea.getHeight();

    // Clamp relative position to be within the display area
    relativePos.x = juce::jlimit(0, (int)w, relativePos.x);
    relativePos.y = juce::jlimit(0, (int)h, relativePos.y);

    const float newNormX = relativePos.x / w;
    const float newNormY = relativePos.y / h;

    if (activeHandle == HandleType::LineCenter)
    {
        apvts.getParameter("LineCX")->setValueNotifyingHost (newNormX);
        apvts.getParameter("LineCY")->setValueNotifyingHost (newNormY);
    }
    else if (activeHandle == HandleType::LineEndpoint)
    {
        auto* cxParam = apvts.getParameter("LineCX");
        auto* cyParam = apvts.getParameter("LineCY");
        auto* lengthParam = apvts.getParameter("LineLength");
        auto* angleParam = apvts.getParameter("LineAngle");

        const float centerX = cxParam->getValue();
        const float centerY = cyParam->getValue();

        const float dx = newNormX - centerX;
        const float dy = newNormY - centerY;

        const float newLength = 2.0f * std::sqrt(dx*dx + dy*dy);
        float newAngle = std::atan2(dy, dx);
        if (newAngle < 0.0f)
            newAngle += juce::MathConstants<float>::twoPi;

        lengthParam->setValueNotifyingHost(juce::jlimit(0.0f, 1.0f, newLength));
        angleParam->setValueNotifyingHost(newAngle / juce::MathConstants<float>::twoPi);
    }
    else if (activeHandle == HandleType::CircleCenter)
    {
        apvts.getParameter("CX")->setValueNotifyingHost (newNormX);
        apvts.getParameter("CY")->setValueNotifyingHost (newNormY);
    }
    else if (activeHandle == HandleType::CircleRadius)
    {
        auto* cxParam = apvts.getParameter("CX");
        auto* rParam = static_cast<juce::RangedAudioParameter*>(apvts.getParameter("R"));

        const float centerX = cxParam->getValue();
        const float newRadiusValue = std::abs(newNormX - centerX);

        // The parameter's actual value is the radius, which has a range like [0, 0.5].
        // We clamp the calculated radius (from mouse position) to this range.
        const auto& range = rParam->getNormalisableRange();
        const float clampedRadiusValue = juce::jlimit(range.start, range.end, newRadiusValue);

        // Convert the clamped actual value to a normalized [0,1] value for the host.
        rParam->setValueNotifyingHost(range.convertTo0to1(clampedRadiusValue));
    }
}

void MapDisplayComponent::mouseUp (const juce::MouseEvent& event)
{
    if (activeHandle != HandleType::None)
    {
        activeHandle = HandleType::None;

        // End the parameter change gesture
        processor.apvts.getParameter("LineCX")->endChangeGesture();
        processor.apvts.getParameter("LineCY")->endChangeGesture();
        processor.apvts.getParameter("LineLength")->endChangeGesture();
        processor.apvts.getParameter("LineAngle")->endChangeGesture();
        processor.apvts.getParameter("CX")->endChangeGesture();
        processor.apvts.getParameter("CY")->endChangeGesture();
        processor.apvts.getParameter("R")->endChangeGesture();
    }
}

juce::Rectangle<float> MapDisplayComponent::getHandleRect (juce::Point<float> center) const
{
    return juce::Rectangle<float> (handleSize, handleSize).withCentre (center);
}

MapDisplayComponent::HandleType MapDisplayComponent::getHandleAt (juce::Point<int> position)
{
    auto relativePos = position - displayArea.getPosition();
    const float w = (float) displayArea.getWidth();
    const float h = (float) displayArea.getHeight();
    auto& apvts = processor.apvts;

    // Check in reverse order of drawing to pick the one on top
    const float cx_c = apvts.getRawParameterValue("CX")->load();
    const float cy_c = apvts.getRawParameterValue("CY")->load();
    const float r_c_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("R"))->get();

    if (getHandleRect({cx_c * w + r_c_actual * juce::jmin(w, h), cy_c * h}).contains((float)relativePos.x, (float)relativePos.y)) return HandleType::CircleRadius;
    if (getHandleRect({cx_c * w, cy_c * h}).contains((float)relativePos.x, (float)relativePos.y)) return HandleType::CircleCenter;

    const float cx_l = apvts.getRawParameterValue("LineCX")->load(), cy_l = apvts.getRawParameterValue("LineCY")->load();
    const float len_l = apvts.getRawParameterValue("LineLength")->load(), ang_l = apvts.getRawParameterValue("LineAngle")->load();
    const float x2 = cx_l + (len_l * 0.5f * std::cos(ang_l));
    const float y2 = cy_l + (len_l * 0.5f * std::sin(ang_l));
    if (getHandleRect({x2 * w, y2 * h}).contains((float)relativePos.x, (float)relativePos.y)) return HandleType::LineEndpoint;
    if (getHandleRect({cx_l * w, cy_l * h}).contains((float)relativePos.x, (float)relativePos.y)) return HandleType::LineCenter;

    return HandleType::None;
}

void MapDisplayComponent::timerCallback()
{
    repaint();
}

void MapDisplayComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &processor.imageBuffer)
    {
        repaint();
    }
}