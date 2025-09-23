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
    {
        g.setColour(LFOMODULATEDCOLOUR);
        const float cx_base = apvts.getRawParameterValue("EllipseCX")->load();
        const float cy_base = apvts.getRawParameterValue("EllipseCY")->load();
        const float r1_base_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR1"))->get();
        const float r2_base_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR2"))->get();
        const float angle_base = apvts.getRawParameterValue("EllipseAngle")->load();

        const float modCxAmount = apvts.getRawParameterValue("Mod_EllipseCX_Amount")->load();
        const int modCxSelect = (int)apvts.getRawParameterValue("Mod_EllipseCX_Select")->load();
        const float cx = cx_base * (1.0f + modCxAmount * (getModVal(modCxSelect) * 2.0f - 1.0f));

        const float modCyAmount = apvts.getRawParameterValue("Mod_EllipseCY_Amount")->load();
        const int modCySelect = (int)apvts.getRawParameterValue("Mod_EllipseCY_Select")->load();
        const float cy = cy_base * (1.0f + modCyAmount * (getModVal(modCySelect) * 2.0f - 1.0f));

        const float modR1Amount = apvts.getRawParameterValue("Mod_EllipseR1_Amount")->load();
        const int modR1Select = (int)apvts.getRawParameterValue("Mod_EllipseR1_Select")->load();
        const float r1_param = r1_base_actual * (1.0f + modR1Amount * (getModVal(modR1Select) * 2.0f - 1.0f));

        const float modR2Amount = apvts.getRawParameterValue("Mod_EllipseR2_Amount")->load();
        const int modR2Select = (int)apvts.getRawParameterValue("Mod_EllipseR2_Select")->load();
        const float r2_param = r2_base_actual * (1.0f + modR2Amount * (getModVal(modR2Select) * 2.0f - 1.0f));

        const float modAngleAmount = apvts.getRawParameterValue("Mod_EllipseAngle_Amount")->load();
        const int modAngleSelect = (int)apvts.getRawParameterValue("Mod_EllipseAngle_Select")->load();
        const float angle = angle_base * (1.0f + modAngleAmount * (getModVal(modAngleSelect) * 2.0f - 1.0f));

        juce::Path p;
        const float r1_pixels = r1_param * juce::jmin(w, h);
        const float r2_pixels = r2_param * juce::jmin(w, h);
        p.addEllipse(cx * w - r1_pixels, cy * h - r2_pixels, r1_pixels * 2.0f, r2_pixels * 2.0f);
        p.applyTransform(juce::AffineTransform::rotation(angle, cx * w, cy * h));
        g.strokePath(p, juce::PathStrokeType(2.0f));
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
    {
        g.setColour(ELLIPSECOLOUR);
        const float cx_base = apvts.getRawParameterValue("EllipseCX")->load();
        const float cy_base = apvts.getRawParameterValue("EllipseCY")->load();
        const float r1_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR1"))->get();
        const float r2_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR2"))->get();
        const float angle_base = apvts.getRawParameterValue("EllipseAngle")->load();

        juce::Path p;
        const float r1_pixels = r1_actual * juce::jmin(w, h);
        const float r2_pixels = r2_actual * juce::jmin(w, h);
        p.addEllipse(cx_base * w - r1_pixels, cy_base * h - r2_pixels, r1_pixels * 2.0f, r2_pixels * 2.0f);
        p.applyTransform(juce::AffineTransform::rotation(angle_base, cx_base * w, cy_base * h));
        g.strokePath(p, juce::PathStrokeType(2.0f));
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
    { // Ellipse handles
        g.setColour(ELLIPSECOLOUR.withAlpha(0.7f));
        const float cx_base = apvts.getRawParameterValue("EllipseCX")->load();
        const float cy_base = apvts.getRawParameterValue("EllipseCY")->load();
        const float r1_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR1"))->get();
        const float r2_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR2"))->get();
        const float angle_base = apvts.getRawParameterValue("EllipseAngle")->load();

        const float r1_pixels = r1_actual * juce::jmin(w, h);
        const float r2_pixels = r2_actual * juce::jmin(w, h);

        const float cosAngle = std::cos(angle_base);
        const float sinAngle = std::sin(angle_base);

        // Center handle
        g.fillRect(getHandleRect({ cx_base * w, cy_base * h }));

        // R1 handle
        g.fillRect(getHandleRect({ cx_base * w + r1_pixels * cosAngle, cy_base * h + r1_pixels * sinAngle }));

        // R2 handle
        g.fillRect(getHandleRect({ cx_base * w - r2_pixels * sinAngle, cy_base * h + r2_pixels * cosAngle }));
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
            else if (readerInfo.type == ReaderBase::Type::Ellipse)
            {
                g.setColour(ELLIPSECOLOUR.withAlpha(readerInfo.volume));
                juce::Path p;
                const float r1_pixels = readerInfo.r1 * juce::jmin(w, h);
                const float r2_pixels = readerInfo.r2 * juce::jmin(w, h);
                p.addEllipse(readerInfo.cx * w - r1_pixels, readerInfo.cy * h - r2_pixels, r1_pixels * 2.0f, r2_pixels * 2.0f);
                p.applyTransform(juce::AffineTransform::rotation(readerInfo.angle, readerInfo.cx * w, readerInfo.cy * h));
                g.strokePath(p, juce::PathStrokeType(2.0f));
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
        processor.apvts.getParameter("EllipseCX")->beginChangeGesture();
        processor.apvts.getParameter("EllipseCY")->beginChangeGesture();
        processor.apvts.getParameter("EllipseR1")->beginChangeGesture();
        processor.apvts.getParameter("EllipseR2")->beginChangeGesture();
        processor.apvts.getParameter("EllipseAngle")->beginChangeGesture();
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
    else if (activeHandle == HandleType::EllipseCenter)
    {
        apvts.getParameter("EllipseCX")->setValueNotifyingHost(newNormX);
        apvts.getParameter("EllipseCY")->setValueNotifyingHost(newNormY);
    }
    else if (activeHandle == HandleType::EllipseR1)
    {
        auto* cxParam = apvts.getParameter("EllipseCX");
        auto* cyParam = apvts.getParameter("EllipseCY");
        auto* r1Param = static_cast<juce::RangedAudioParameter*>(apvts.getParameter("EllipseR1"));
        auto* angleParam = apvts.getParameter("EllipseAngle");

        const float centerX = cxParam->getValue();
        const float centerY = cyParam->getValue();
        const float dx = newNormX - centerX;
        const float dy = newNormY - centerY;

        const float newR1Value = std::sqrt(dx * dx + dy * dy);
        float newAngle = std::atan2(dy, dx);
        if (newAngle < 0.0f)
            newAngle += juce::MathConstants<float>::twoPi;

        const auto& range = r1Param->getNormalisableRange();
        const float clampedR1 = juce::jlimit(range.start, range.end, newR1Value);
        r1Param->setValueNotifyingHost(range.convertTo0to1(clampedR1));
        angleParam->setValueNotifyingHost(newAngle / juce::MathConstants<float>::twoPi);
    }
    else if (activeHandle == HandleType::EllipseR2)
    {
        auto* cxParam = apvts.getParameter("EllipseCX");
        auto* cyParam = apvts.getParameter("EllipseCY");
        auto* r2Param = static_cast<juce::RangedAudioParameter*>(apvts.getParameter("EllipseR2"));
        auto* angleParam = apvts.getParameter("EllipseAngle");

        const float centerX = cxParam->getValue();
        const float centerY = cyParam->getValue();
        const float dx = newNormX - centerX;
        const float dy = newNormY - centerY;

        const float newR2Value = std::sqrt(dx * dx + dy * dy);
        float newAngleForR2Handle = std::atan2(dy, dx);
        float newEllipseAngle = newAngleForR2Handle - juce::MathConstants<float>::halfPi;
        if (newEllipseAngle < 0.0f) newEllipseAngle += juce::MathConstants<float>::twoPi;

        const auto& range = r2Param->getNormalisableRange();
        const float clampedR2 = juce::jlimit(range.start, range.end, newR2Value);
        r2Param->setValueNotifyingHost(range.convertTo0to1(clampedR2));
        angleParam->setValueNotifyingHost(newEllipseAngle / juce::MathConstants<float>::twoPi);
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
        processor.apvts.getParameter("EllipseCX")->endChangeGesture();
        processor.apvts.getParameter("EllipseCY")->endChangeGesture();
        processor.apvts.getParameter("EllipseR1")->endChangeGesture();
        processor.apvts.getParameter("EllipseR2")->endChangeGesture();
        processor.apvts.getParameter("EllipseAngle")->endChangeGesture();
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

    // Check ellipse handles first
    const float cx_e = apvts.getRawParameterValue("EllipseCX")->load();
    const float cy_e = apvts.getRawParameterValue("EllipseCY")->load();
    const float r1_e_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR1"))->get();
    const float r2_e_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter("EllipseR2"))->get();
    const float angle_e = apvts.getRawParameterValue("EllipseAngle")->load();
    const float r1_pixels = r1_e_actual * juce::jmin(w, h);
    const float r2_pixels = r2_e_actual * juce::jmin(w, h);
    const float cosAngle_e = std::cos(angle_e);
    const float sinAngle_e = std::sin(angle_e);

    if (getHandleRect({ cx_e * w - r2_pixels * sinAngle_e, cy_e * h + r2_pixels * cosAngle_e }).contains((float)relativePos.x, (float)relativePos.y)) return HandleType::EllipseR2;
    if (getHandleRect({ cx_e * w + r1_pixels * cosAngle_e, cy_e * h + r1_pixels * sinAngle_e }).contains((float)relativePos.x, (float)relativePos.y)) return HandleType::EllipseR1;
    if (getHandleRect({ cx_e * w, cy_e * h }).contains((float)relativePos.x, (float)relativePos.y)) return HandleType::EllipseCenter;

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