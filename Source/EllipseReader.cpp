/*
  ==============================================================================

    EllipseReader.cpp
    Created: 21 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#include "EllipseReader.h"
#include "LFO.h"

EllipseReader::EllipseReader() {}
EllipseReader::~EllipseReader() {}

void EllipseReader::prepareToPlay (double sr)
{
    ReaderBase::prepareToPlay(sr);
    sampleRate = sr;
    const double rampTimeSeconds = 0.05;
    cxs.reset (sampleRate, rampTimeSeconds); cxs.setCurrentAndTargetValue (cx.load());
    cys.reset (sampleRate, rampTimeSeconds); cys.setCurrentAndTargetValue (cy.load());
    r1s.reset (sampleRate, rampTimeSeconds); r1s.setCurrentAndTargetValue (r1.load());
    r2s.reset (sampleRate, rampTimeSeconds); r2s.setCurrentAndTargetValue (r2.load());
    angles.reset (sampleRate, rampTimeSeconds); angles.setCurrentAndTargetValue (angle.load());
    volumeSmoother.reset (sampleRate, rampTimeSeconds);
    volumeSmoother.setCurrentAndTargetValue (volume);
    panSmoother.setCurrentAndTargetValue(pan.load());
}

void EllipseReader::setCentre (float newCx, float newCy)
{
    cx = juce::jlimit (0.0f, 1.0f, newCx);
    cy = juce::jlimit (0.0f, 1.0f, newCy);
    cxs.setTargetValue (cx.load());
    cys.setTargetValue (cy.load());
}

void EllipseReader::setRadii (float newR1, float newR2)
{
    r1 = juce::jlimit (0.0f, 0.5f, newR1);
    r2 = juce::jlimit (0.0f, 0.5f, newR2);
    r1s.setTargetValue (r1.load());
    r2s.setTargetValue (r2.load());
}

void EllipseReader::setAngle (float newAngle)
{
    angle = newAngle;
    angles.setTargetValue (angle.load());
}

void EllipseReader::updateParameters (const EllipseReaderParameters& params)
{
    setCentre (params.cx, params.cy);
    setRadii (params.r1, params.r2);
    setAngle (params.angle);
    setVolume (params.volume);
    setPan(params.pan);
    detune = params.detune;
    updateFilterParameters(params.filter);

    modCxAmount = params.modCxAmount;
    modCyAmount = params.modCyAmount;
    modR1Amount = params.modR1Amount;
    modR2Amount = params.modR2Amount;
    modAngleAmount = params.modAngleAmount;
    modVolumeAmount = params.modVolumeAmount;

    modCxSelect = params.modCxSelect;
    modCySelect = params.modCySelect;
    modR1Select = params.modR1Select;
    modR2Select = params.modR2Select;
    modAngleSelect = params.modAngleSelect;
    modVolumeSelect = params.modVolumeSelect;
    modPanAmount = params.modPanAmount;
    modPanSelect = params.modPanSelect;
    modFreqAmount = params.modFreqAmount;
    modFreqSelect = params.modFreqSelect;
}

void EllipseReader::processBlock (const juce::Image::BitmapData& bitmapData, juce::AudioBuffer<float>& buffer, int startSample, int numSamples,
                                 const juce::AudioBuffer<float>& modulatorBuffer)
{
    if (bitmapData.data == nullptr || bitmapData.width <= 1 || bitmapData.height <= 1)
    {
        buffer.clear(startSample, numSamples);
        return;
    }

    const auto twoPi = juce::MathConstants<float>::twoPi;

    const float imageWidth = (float) (bitmapData.width - 1);
    const float imageHeight = (float) (bitmapData.height - 1);
    const int numChannels = buffer.getNumChannels();

    auto applyMod = [] (float base, float modAmount, float modSignal, bool isBipolar)
    {
        if (isBipolar)
        {
            float bipolarSignal = modSignal * 2.0f - 1.0f;
            return base * (1.0f + modAmount * bipolarSignal);
        }
        else
        {
            float multiplier;
            if (modAmount >= 0.0f)
                multiplier = 1.0f + modAmount * (modSignal - 1.0f);
            else
                multiplier = 1.0f + modAmount * modSignal;
            return base * multiplier;
        }
    };

    for (int sample = startSample; sample < startSample + numSamples; ++sample)
    {
        // Get smoothed base values
        float cx_base = cxs.getNextValue();
        float cy_base = cys.getNextValue();
        float r1_base = r1s.getNextValue();
        float r2_base = r2s.getNextValue();
        float angle_base = angles.getNextValue();
        float volume_base = volumeSmoother.getNextValue();
        float pan_base = panSmoother.getNextValue();

        // Apply modulation
        float cx_sv = applyMod (cx_base, modCxAmount.load(), modulatorBuffer.getSample (modCxSelect.load(), sample), true);
        float cy_sv = applyMod (cy_base, modCyAmount.load(), modulatorBuffer.getSample (modCySelect.load(), sample), true);
        float r1_sv = applyMod (r1_base, modR1Amount.load(), modulatorBuffer.getSample (modR1Select.load(), sample), true);
        float r2_sv = applyMod (r2_base, modR2Amount.load(), modulatorBuffer.getSample (modR2Select.load(), sample), true);
        float angle_sv = applyMod (angle_base, modAngleAmount.load(), modulatorBuffer.getSample (modAngleSelect.load(), sample), true);
        float volume_sv = applyMod (volume_base, modVolumeAmount.load(), modulatorBuffer.getSample (modVolumeSelect.load(), sample), false);

        // Pan is additive, not multiplicative
        const float panModSignal = modulatorBuffer.getSample(modPanSelect.load(), sample) * 2.0f - 1.0f; // to [-1, 1]
        float pan_sv = pan_base + modPanAmount.load() * panModSignal;

        // --- Frequency Modulation ---
        const float freqModSignal = modulatorBuffer.getSample(modFreqSelect.load(), sample);
        const float bipolarFreqMod = freqModSignal * 2.0f - 1.0f;
        const float numOctaves = 1.0f;
        const float modulatedFreq = frequency * std::pow(2.0f, modFreqAmount.load() * bipolarFreqMod * numOctaves); 

        const float detunedFreq = modulatedFreq * std::pow(2.0f, detune.load() / 12.0f);
        const float phaseIncrement = detunedFreq / (float) sampleRate;
        const float phaseIncrementLow = phaseIncrement * 0.5f;
        const float phaseIncrementHigh = phaseIncrement * 2.0f;

        // Optimization: if volume is zero, we can skip the expensive sample reading part.
        if (volume_sv < 0.0001f)
        {
            // We still need to advance the phases to keep them in sync
            phase = std::fmod (phase + phaseIncrement, 1.0f);
            phaseLow = std::fmod (phaseLow + phaseIncrementLow, 1.0f);
            phaseHigh = std::fmod (phaseHigh + phaseIncrementHigh, 1.0f);

            if (sample == startSample + numSamples - 1)
                lastDrawingInfo.isActive = false;

            continue; // Skip to next sample, buffer is additive so no sound is added
        }

        // Clamp modulated values
        cx_sv = juce::jlimit (0.0f, 1.0f, cx_sv);
        cy_sv = juce::jlimit (0.0f, 1.0f, cy_sv);
        r1_sv = juce::jlimit (0.0f, 0.5f, r1_sv);
        r2_sv = juce::jlimit (0.0f, 0.5f, r2_sv);

        if (sample == startSample + numSamples - 1)
        {
            lastDrawingInfo.isActive = true;
            lastDrawingInfo.type = Type::Ellipse;
            lastDrawingInfo.volume = volume_sv;
            lastDrawingInfo.cx = cx_sv;
            lastDrawingInfo.cy = cy_sv;
            lastDrawingInfo.r1 = r1_sv;
            lastDrawingInfo.r2 = r2_sv;
            lastDrawingInfo.angle = angle_sv;
        }

        const float normalizedLength = (r1_sv + r2_sv); // Map average radius to [0, 1] for amplitude calculation

        const float ampHigh = juce::jmax (0.0f, 1.0f - normalizedLength * 2.0f);
        const float ampBase = 1.0f - std::abs (normalizedLength - 0.5f) * 2.0f;
        const float ampLow  = juce::jmax (0.0f, (normalizedLength - 0.5f) * 2.0f);

        float finalSampleValue = 0.0f;

        auto getSampleAtPhase = [&] (float currentPhase) -> float
        {
            const float phaseAngle = currentPhase * twoPi;
            // const float cosPhase = juce::dsp::FastMathApproximations::cos (phaseAngle);
            // const float sinPhase = juce::dsp::FastMathApproximations::sin (phaseAngle);
            // const float cosAngle = juce::dsp::FastMathApproximations::cos (angle_sv);
            // const float sinAngle = juce::dsp::FastMathApproximations::sin (angle_sv);

            const float cosPhase = std::cos (phaseAngle);
            const float sinPhase = std::sin (phaseAngle);
            const float cosAngle = std::cos (angle_sv);
            const float sinAngle = std::sin (angle_sv);

            const float currentX = cx_sv + (r1_sv * cosPhase * cosAngle - r2_sv * sinPhase * sinAngle);
            const float currentY = cy_sv + (r1_sv * cosPhase * sinAngle + r2_sv * sinPhase * cosAngle);

            float pixelX = juce::jlimit (0.0f, imageWidth,  currentX * imageWidth);
            float pixelY = juce::jlimit (0.0f, imageHeight, currentY * imageHeight);

            const int ix = (int) pixelX;
            const int iy = (int) pixelY;
            const float fx = pixelX - ix;
            const float fy = pixelY - iy;

            const int ix1 = std::min (ix + 1, bitmapData.width - 1);
            const int iy1 = std::min (iy + 1, bitmapData.height - 1);

            auto getBrightnessAt = [&bitmapData](int x, int y) -> float {
                auto* p = bitmapData.getPixelPointer(x, y);
                return (float) juce::jmax (p[0], p[1], p[2]) / 255.0f;
            };

            const float c00 = getBrightnessAt (ix, iy);
            const float c10 = getBrightnessAt (ix1, iy);
            const float c01 = getBrightnessAt (ix, iy1);
            const float c11 = getBrightnessAt (ix1, iy1);

            const float top = c00 + fx * (c10 - c00);
            const float bottom = c01 + fx * (c11 - c01);
            
            return (top + fy * (bottom - top)) * 2.0f - 1.0f;
        };

        if (ampLow > 0.0f)  finalSampleValue += ampLow  * getSampleAtPhase (phaseLow);
        if (ampBase > 0.0f) finalSampleValue += ampBase * getSampleAtPhase (phase);
        if (ampHigh > 0.0f) finalSampleValue += ampHigh * getSampleAtPhase (phaseHigh);

        // Apply filter
        const float modFreqSignalForFilter = modulatorBuffer.getSample(modFilterFreqSelect.load(), sample);
        const float modQualitySignal = modulatorBuffer.getSample(modFilterQualitySelect.load(), sample);
        finalSampleValue = applyFilter(finalSampleValue, modFreqSignalForFilter, modQualitySignal);

        // Apply Volume and Pan
        finalSampleValue *= volume_sv;

        const float panAngle = (juce::jlimit(-1.0f, 1.0f, pan_sv) * 0.5f + 0.5f) * juce::MathConstants<float>::halfPi;
        const float leftGain = std::cos(panAngle);
        const float rightGain = std::sin(panAngle);

        if (numChannels > 0)
            buffer.addSample(0, sample, finalSampleValue * leftGain);
        if (numChannels > 1)
            buffer.addSample(1, sample, finalSampleValue * rightGain);

        phase += phaseIncrement;
        phase = std::fmod (phase, 1.0f);

        phaseLow += phaseIncrementLow;
        phaseLow = std::fmod (phaseLow, 1.0f);

        phaseHigh += phaseIncrementHigh;
        phaseHigh = std::fmod (phaseHigh, 1.0f);
    }
}