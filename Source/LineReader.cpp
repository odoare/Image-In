/*
  ==============================================================================

    LineReader.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "LineReader.h"
#include "LFO.h"

LineReader::LineReader()
{
    // Endpoints are calculated on-the-fly, no need to pre-calculate here.
}

LineReader::~LineReader()
{
}

void LineReader::prepareToPlay (double sr) 
{
    ReaderBase::prepareToPlay(sr);
    sampleRate = sr;
    const double rampTimeSeconds = 0.05;
    cxs.reset (sampleRate, rampTimeSeconds); cxs.setCurrentAndTargetValue (cx.load());
    cys.reset (sampleRate, rampTimeSeconds); cys.setCurrentAndTargetValue (cy.load());
    lengths.reset (sampleRate, rampTimeSeconds); lengths.setCurrentAndTargetValue (length.load());
    angles.reset (sampleRate, rampTimeSeconds); angles.setCurrentAndTargetValue (angle.load());
    volumeSmoother.reset (sampleRate, rampTimeSeconds);
    volumeSmoother.setCurrentAndTargetValue (volume);
    panSmoother.setCurrentAndTargetValue(pan.load());
}

void LineReader::setCentre (float newCx, float newCy)
{
    if (newCx == cx.load() && newCy == cy.load())
        return;

    cx = juce::jlimit (0.0f, 1.0f, newCx);
    cy = juce::jlimit (0.0f, 1.0f, newCy);
    cxs.setTargetValue (cx.load());
    cys.setTargetValue (cy.load());
}

void LineReader::setLength (float newLength)
{
    if (newLength == length.load())
        return;

    length = juce::jlimit (0.0f, 1.0f, newLength);
    lengths.setTargetValue (length.load());
}

void LineReader::setAngle (float newAngle)
{
    if (newAngle == angle.load())
        return;

    angle = newAngle;
    angles.setTargetValue (angle.load());
}

void LineReader::updateParameters (const LineReaderParameters& params)
{
    setCentre (params.cx, params.cy);
    setLength (params.length);
    setAngle (params.angle);
    setVolume (params.volume);
    setPan(params.pan);
    updateFilterParameters(params.filter);

    modCxAmount = params.modCxAmount;
    modCyAmount = params.modCyAmount;
    modAngleAmount = params.modAngleAmount;
    modLengthAmount = params.modLengthAmount;
    modVolumeAmount = params.modVolumeAmount;

    modCxSelect = params.modCxSelect;
    modCySelect = params.modCySelect;
    modAngleSelect = params.modAngleSelect;
    modLengthSelect = params.modLengthSelect;
    modVolumeSelect = params.modVolumeSelect;
    modPanAmount = params.modPanAmount;
    modPanSelect = params.modPanSelect;
    modFreqAmount = params.modFreqAmount;
    modFreqSelect = params.modFreqSelect;
}

void LineReader::processBlock (const juce::Image& imageToRead, juce::AudioBuffer<float>& buffer, int startSample, int numSamples,
                               const juce::AudioBuffer<float>& modulatorBuffer)
{
    if (! imageToRead.isValid())
    {
        buffer.clear (startSample, numSamples);
        return;
    }

    juce::Image::BitmapData bitmapData (imageToRead, juce::Image::BitmapData::readOnly);

    if (bitmapData.data == nullptr || bitmapData.width <= 1 || bitmapData.height <= 1)
    {
        buffer.clear (startSample, numSamples);
        return;
    }

    const float imageWidth = (float) (bitmapData.width - 1);
    const float imageHeight = (float) (bitmapData.height - 1);
    const int numChannels = buffer.getNumChannels();

    auto applyMod = [] (float base, float modAmount, float modSignal, bool isBipolar)
    {
        if (isBipolar)
        {
            // Bipolar modulation: value swings around the base.
            // modAmount is a bipolar depth control from -1 (inverted) to 1 (normal).
            // modSignal is [0, 1], so we map it to [-1, 1].
            float bipolarSignal = modSignal * 2.0f - 1.0f;
            return base * (1.0f + modAmount * bipolarSignal);
        }
        else
        {
            // Unipolar modulation: value is scaled by the modulator.
            // modAmount is a bipolar mix control: 0=no mod, 1=full pos, -1=full inv.
            float multiplier;
            if (modAmount >= 0.0f)
                // Interpolate between 1.0 (no mod) and modSignal (full positive mod)
                multiplier = 1.0f + modAmount * (modSignal - 1.0f);
            else // modAmount < 0.0f
                // Interpolate between 1.0 (no mod) and (1.0 - modSignal) (full inverted mod)
                multiplier = 1.0f + modAmount * modSignal;
            return base * multiplier;
        }
    };

    for (int sample = startSample; sample < startSample + numSamples; ++sample)
    {
        // Get smoothed base values
        float cx_base = cxs.getNextValue();
        float cy_base = cys.getNextValue();
        float length_base = lengths.getNextValue();
        float angle_base = angles.getNextValue();
        float volume_base = volumeSmoother.getNextValue();
        float pan_base = panSmoother.getNextValue();

        // Apply modulation
        float cx_sv = applyMod (cx_base, modCxAmount.load(), modulatorBuffer.getSample (modCxSelect.load(), sample), true);
        float cy_sv = applyMod (cy_base, modCyAmount.load(), modulatorBuffer.getSample (modCySelect.load(), sample), true);
        float length_sv = applyMod (length_base, modLengthAmount.load(), modulatorBuffer.getSample (modLengthSelect.load(), sample), true);
        float angle_sv = applyMod (angle_base, modAngleAmount.load(), modulatorBuffer.getSample (modAngleSelect.load(), sample), true);
        float volume_sv = applyMod (volume_base, modVolumeAmount.load(), modulatorBuffer.getSample (modVolumeSelect.load(), sample), false);
        float pan_sv = applyMod(pan_base, modPanAmount.load(), modulatorBuffer.getSample(modPanSelect.load(), sample), true);

        // --- Frequency Modulation ---
        const float freqModSignal = modulatorBuffer.getSample(modFreqSelect.load(), sample);
        const float bipolarFreqMod = freqModSignal * 2.0f - 1.0f;
        const float numOctaves = 1.0f; // Modulate over a +/- 1 octave range for vibrato
        const float modulatedFreq = frequency * std::pow(2.0f, modFreqAmount.load() * bipolarFreqMod * numOctaves);

        const float phaseIncrement = modulatedFreq / (float) sampleRate;
        const float phaseIncrementLow = phaseIncrement * 0.5f;
        const float phaseIncrementHigh = phaseIncrement * 2.0f;

        // Clamp modulated values to a safe range
        cx_sv = juce::jlimit (0.0f, 1.0f, cx_sv);
        cy_sv = juce::jlimit (0.0f, 1.0f, cy_sv);
        length_sv = juce::jlimit (0.0f, 1.0f, length_sv);

        const float halfLength = length_sv * 0.5f;
        const float dx_calc = halfLength * juce::dsp::FastMathApproximations::cos (angle_sv);
        const float dy_calc = halfLength * juce::dsp::FastMathApproximations::sin (angle_sv);

        const float x1sv = cx_sv - dx_calc;
        const float y1sv = cy_sv - dy_calc;
        const float x2sv = cx_sv + dx_calc;
        const float y2sv = cy_sv + dy_calc;

        // --- Amplitudes from length ---
        if (sample == startSample + numSamples - 1)
        {
            lastDrawingInfo.isActive = true;
            lastDrawingInfo.type = Type::Line;
            lastDrawingInfo.volume = volume_sv;
            lastDrawingInfo.x1 = x1sv;
            lastDrawingInfo.y1 = y1sv;
            lastDrawingInfo.x2 = x2sv;
            lastDrawingInfo.y2 = y2sv;
        }


        const float normalizedLength = juce::jlimit (0.0f, 1.0f, length_sv / std::sqrt (2.0f));

        const float ampHigh = juce::jmax (0.0f, 1.0f - normalizedLength * 2.0f);
        const float ampBase = 1.0f - std::abs (normalizedLength - 0.5f) * 2.0f;
        const float ampLow  = juce::jmax (0.0f, (normalizedLength - 0.5f) * 2.0f);

        float finalSampleValue = 0.0f;

        auto getSampleAtPhase = [&] (float currentPhase) -> float
        {
            const float currentX = x1sv + (x2sv - x1sv) * currentPhase;
            const float currentY = y1sv + (y2sv - y1sv) * currentPhase;

            float pixelX = juce::jlimit (0.0f, imageWidth,  currentX * imageWidth);
            float pixelY = juce::jlimit (0.0f, imageHeight, currentY * imageHeight);

            const int ix = (int) pixelX;
            const int iy = (int) pixelY;
            const float fx = pixelX - ix;
            const float fy = pixelY - iy;

            const int ix1 = std::min (ix + 1, bitmapData.width - 1);
            const int iy1 = std::min (iy + 1, bitmapData.height - 1);

            // Micro-optimization: avoid creating juce::Colour objects.
            // This assumes ARGB format, which is what JUCE typically uses for loaded images.
            // In JUCE, for ARGB format on little-endian systems, pixel data is B, G, R, A.
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
        const float modFreqSignal = modulatorBuffer.getSample(modFilterFreqSelect.load(), sample);
        const float modQualitySignal = modulatorBuffer.getSample(modFilterQualitySelect.load(), sample);
        finalSampleValue = applyFilter(finalSampleValue, modFreqSignal, modQualitySignal);

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