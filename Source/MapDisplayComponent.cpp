/*
  ==============================================================================

    MapDisplayComponent.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

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
    for (int i = 0; i < 3; ++i)
    {
        juce::String prefix = "Ellipse" + juce::String(i + 1) + "_";
        const bool showLFO = apvts.getRawParameterValue(prefix + "ShowLFO")->load() > 0.5f;
        if (showLFO && apvts.getRawParameterValue(prefix + "Volume")->load() > 0.01f)
        {
            g.setColour(LFOMODULATEDCOLOUR);
            const float cx_base = apvts.getRawParameterValue(prefix + "CX")->load();
            const float cy_base = apvts.getRawParameterValue(prefix + "CY")->load();
            const float r1_base_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R1"))->get();
            const float r2_base_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R2"))->get();
            const float angle_base = apvts.getRawParameterValue(prefix + "Angle")->load();

            const float modCxAmount = apvts.getRawParameterValue("Mod_" + prefix + "CX_Amount")->load();
            const int modCxSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "CX_Select")->load();
            const float cx = cx_base * (1.0f + modCxAmount * (getModVal(modCxSelect) * 2.0f - 1.0f));

            const float modCyAmount = apvts.getRawParameterValue("Mod_" + prefix + "CY_Amount")->load();
            const int modCySelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "CY_Select")->load();
            const float cy = cy_base * (1.0f + modCyAmount * (getModVal(modCySelect) * 2.0f - 1.0f));

            const float modR1Amount = apvts.getRawParameterValue("Mod_" + prefix + "R1_Amount")->load();
            const int modR1Select = (int)apvts.getRawParameterValue("Mod_" + prefix + "R1_Select")->load();
            const float r1_param = r1_base_actual * (1.0f + modR1Amount * (getModVal(modR1Select) * 2.0f - 1.0f));

            const float modR2Amount = apvts.getRawParameterValue("Mod_" + prefix + "R2_Amount")->load();
            const int modR2Select = (int)apvts.getRawParameterValue("Mod_" + prefix + "R2_Select")->load();
            const float r2_param = r2_base_actual * (1.0f + modR2Amount * (getModVal(modR2Select) * 2.0f - 1.0f));

            const float modAngleAmount = apvts.getRawParameterValue("Mod_" + prefix + "Angle_Amount")->load();
            const int modAngleSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "Angle_Select")->load();
            const float angle = angle_base * (1.0f + modAngleAmount * (getModVal(modAngleSelect) * 2.0f - 1.0f));

            juce::Path p;
            const float r1_pixels = r1_param * juce::jmin(w, h);
            const float r2_pixels = r2_param * juce::jmin(w, h);
            p.addEllipse(cx * w - r1_pixels, cy * h - r2_pixels, r1_pixels * 2.0f, r2_pixels * 2.0f);
            p.applyTransform(juce::AffineTransform::rotation(angle, cx * w, cy * h));
            g.strokePath(p, juce::PathStrokeType(2.0f));
        }
    }

    // Draw glow for static paths
    for (int i = 0; i < 3; ++i)
    {
        juce::String prefix = "Ellipse" + juce::String(i + 1) + "_";
        const float volume = apvts.getRawParameterValue(prefix + "Volume")->load();
        if (volume > 0.01f)
        {
            const float cx_base = apvts.getRawParameterValue(prefix + "CX")->load();
            const float cy_base = apvts.getRawParameterValue(prefix + "CY")->load();
            const float r1_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R1"))->get();
            const float r2_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R2"))->get();
            const float angle_base = apvts.getRawParameterValue(prefix + "Angle")->load();

            juce::Path p;
            const float r1_pixels = r1_actual * juce::jmin(w, h);
            const float r2_pixels = r2_actual * juce::jmin(w, h);
            p.addEllipse(cx_base * w - r1_pixels, cy_base * h - r2_pixels, r1_pixels * 2.0f, r2_pixels * 2.0f);
            p.applyTransform(juce::AffineTransform::rotation(angle_base, cx_base * w, cy_base * h));

            // // Create a path that represents the stroke of the ellipse
            // juce::PathStrokeType stroke (2.0f);
            // juce::Path strokedPath;
            // stroke.createStrokedPath (strokedPath, p);

            // // Create and draw the glow effect for the stroked path
            // juce::DropShadow shadow (ELLIPSECOLOURS[i].withMultipliedSaturation(.2f).withAlpha (volume), (int)(5 + volume * 50.0f), {});
            // shadow.drawForPath (g, strokedPath);
        }
    }

    // Draw static unmodulated paths
    for (int i = 0; i < 3; ++i)
    {
        juce::String prefix = "Ellipse" + juce::String(i + 1) + "_";
        const bool showMaster = apvts.getRawParameterValue(prefix + "ShowMaster")->load() > 0.5f;

        if (showMaster && apvts.getRawParameterValue(prefix + "Volume")->load() > 0.01f)
        {
            g.setColour(ELLIPSECOLOURS[i]);
            const float cx_base = apvts.getRawParameterValue(prefix + "CX")->load();
            const float cy_base = apvts.getRawParameterValue(prefix + "CY")->load();
            const float r1_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R1"))->get();
            const float r2_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R2"))->get();
            const float angle_base = apvts.getRawParameterValue(prefix + "Angle")->load();

            juce::Path p;
            const float r1_pixels = r1_actual * juce::jmin(w, h);
            const float r2_pixels = r2_actual * juce::jmin(w, h);
            p.addEllipse(cx_base * w - r1_pixels, cy_base * h - r2_pixels, r1_pixels * 2.0f, r2_pixels * 2.0f);
            p.applyTransform(juce::AffineTransform::rotation(angle_base, cx_base * w, cy_base * h));
            g.strokePath(p, juce::PathStrokeType(2.0f));
        }
    }


    // Draw handles
    for (int i = 0; i < 3; ++i)
    {
        juce::String prefix = "Ellipse" + juce::String(i + 1) + "_";
        const bool showMaster = apvts.getRawParameterValue(prefix + "ShowMaster")->load() > 0.5f;

        if (showMaster && apvts.getRawParameterValue(prefix + "Volume")->load() > 0.01f)
        {
            g.setColour(ELLIPSECOLOURS[i].withAlpha(0.7f));
            const float cx_base = apvts.getRawParameterValue(prefix + "CX")->load();
            const float cy_base = apvts.getRawParameterValue(prefix + "CY")->load();
            const float r1_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R1"))->get();
            const float r2_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R2"))->get();
            const float angle_base = apvts.getRawParameterValue(prefix + "Angle")->load();

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
    }


    // Draw per-voice paths
    std::array<std::array<VoiceDisplayState, NUM_VOICES>, 3> statesCopy;
    {
        juce::ScopedLock lock (processor.displayStateLock);
        statesCopy = processor.voiceDisplayStates;
    }

    for (int synthIndex = 0; synthIndex < 3; ++synthIndex)
    {
        for (int voiceIndex = 0; voiceIndex < NUM_VOICES; ++voiceIndex)
        {
            const auto& voiceState = statesCopy[synthIndex][voiceIndex];

            if (! voiceState.isActive)
                continue;

            // A voice only has one reader, so we can get the first one.
            if (voiceState.readerInfos.isEmpty())
                continue;

            const auto& readerInfo = voiceState.readerInfos.getUnchecked(0);
            if (readerInfo.type == ReaderBase::Type::Ellipse)
            {
                const float alpha = readerInfo.volume;
                if (alpha < 0.01f) continue;
                
                g.setColour(ELLIPSECOLOURS[synthIndex].withAlpha(alpha));
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

    activeHandle = getHandleAt(event.getPosition());

    if (activeHandle.type != HandleType::None)
    {
        // Begin a parameter change gesture for automation recording in the host
        for (int i = 1; i <= 3; ++i)
        {
            juce::String prefix = "Ellipse" + juce::String(i) + "_";
            processor.apvts.getParameter(prefix + "CX")->beginChangeGesture();
            processor.apvts.getParameter(prefix + "CY")->beginChangeGesture();
            processor.apvts.getParameter(prefix + "R1")->beginChangeGesture();
            processor.apvts.getParameter(prefix + "R2")->beginChangeGesture();
            processor.apvts.getParameter(prefix + "Angle")->beginChangeGesture();
        }
    }
}

void MapDisplayComponent::mouseDrag (const juce::MouseEvent& event)
{
    if (activeHandle.type == HandleType::None)
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

    juce::String prefix = "Ellipse" + juce::String(activeHandle.readerIndex) + "_";

    if (activeHandle.type == HandleType::EllipseCenter)
    {
        apvts.getParameter(prefix + "CX")->setValueNotifyingHost(newNormX);
        apvts.getParameter(prefix + "CY")->setValueNotifyingHost(newNormY);
    }
    else if (activeHandle.type == HandleType::EllipseR1)
    {
        auto* cxParam = apvts.getParameter(prefix + "CX");
        auto* cyParam = apvts.getParameter(prefix + "CY");
        auto* r1Param = static_cast<juce::RangedAudioParameter*>(apvts.getParameter(prefix + "R1"));
        auto* angleParam = apvts.getParameter(prefix + "Angle");

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
    else if (activeHandle.type == HandleType::EllipseR2)
    {
        auto* cxParam = apvts.getParameter(prefix + "CX");
        auto* cyParam = apvts.getParameter(prefix + "CY");
        auto* r2Param = static_cast<juce::RangedAudioParameter*>(apvts.getParameter(prefix + "R2"));
        auto* angleParam = apvts.getParameter(prefix + "Angle");

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
    if (activeHandle.type != HandleType::None)
    {
        activeHandle = {};

        // End the parameter change gesture
        for (int i = 1; i <= 3; ++i)
        {
            juce::String prefix = "Ellipse" + juce::String(i) + "_";
            processor.apvts.getParameter(prefix + "CX")->endChangeGesture();
            processor.apvts.getParameter(prefix + "CY")->endChangeGesture();
            processor.apvts.getParameter(prefix + "R1")->endChangeGesture();
            processor.apvts.getParameter(prefix + "R2")->endChangeGesture();
            processor.apvts.getParameter(prefix + "Angle")->endChangeGesture();
        }
    }
}

juce::Rectangle<float> MapDisplayComponent::getHandleRect (juce::Point<float> center) const
{
    return juce::Rectangle<float> (handleSize, handleSize).withCentre (center);
}

MapDisplayComponent::Handle MapDisplayComponent::getHandleAt (juce::Point<int> position)
{
    auto relativePos = position - displayArea.getPosition();
    const float w = (float) displayArea.getWidth();
    const float h = (float) displayArea.getHeight();
    auto& apvts = processor.apvts;

    // Check in reverse order so we pick the one on top
    for (int i = 2; i >= 0; --i)
    {
        juce::String prefix = "Ellipse" + juce::String(i + 1) + "_";
        if (apvts.getRawParameterValue(prefix + "Volume")->load() > 0.01f)
        {
            const float cx_e = apvts.getRawParameterValue(prefix + "CX")->load();
            const float cy_e = apvts.getRawParameterValue(prefix + "CY")->load();
            const float r1_e_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R1"))->get();
            const float r2_e_actual = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(prefix + "R2"))->get();
            const float angle_e = apvts.getRawParameterValue(prefix + "Angle")->load();
            const float r1_pixels = r1_e_actual * juce::jmin(w, h);
            const float r2_pixels = r2_e_actual * juce::jmin(w, h);
            const float cosAngle_e = std::cos(angle_e);
            const float sinAngle_e = std::sin(angle_e);

            if (getHandleRect({ cx_e * w - r2_pixels * sinAngle_e, cy_e * h + r2_pixels * cosAngle_e }).contains((float)relativePos.x, (float)relativePos.y)) return { HandleType::EllipseR2, i + 1 };
            if (getHandleRect({ cx_e * w + r1_pixels * cosAngle_e, cy_e * h + r1_pixels * sinAngle_e }).contains((float)relativePos.x, (float)relativePos.y)) return { HandleType::EllipseR1, i + 1 };
            if (getHandleRect({ cx_e * w, cy_e * h }).contains((float)relativePos.x, (float)relativePos.y)) return { HandleType::EllipseCenter, i + 1 };
        }
    }

    return {};
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