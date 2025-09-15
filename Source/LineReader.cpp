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
    sampleRate = sr;
    const double rampTimeSeconds = 0.05;
    cxs.reset (sampleRate, rampTimeSeconds); cxs.setCurrentAndTargetValue (cx.load());
    cys.reset (sampleRate, rampTimeSeconds); cys.setCurrentAndTargetValue (cy.load());
    lengths.reset (sampleRate, rampTimeSeconds); lengths.setCurrentAndTargetValue (length.load());
    angles.reset (sampleRate, rampTimeSeconds); angles.setCurrentAndTargetValue (angle.load());
    volumeSmoother.reset (sampleRate, rampTimeSeconds);
    volumeSmoother.setCurrentAndTargetValue (volume);
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

float LineReader::getX1() const
{
    const float lfoValAngle = lastLfoValues[lfoAngleSelect.load() ? 1 : 0].load();
    const float currentAngle = angle.load() * (1.0f + 2.0f * (lfoAngleAmount.load() - 0.5f) * (lfoValAngle - 0.5f));
    const float lfoValLength = lastLfoValues[lfoLengthSelect.load() ? 1 : 0].load();
    const float currentLength = length.load() * (1.0f + 2.0f * (lfoLengthAmount.load() - 0.5f) * (lfoValLength - 0.5f));
    const float halfLength = currentLength * 0.5f;
    return cx.load() - halfLength * juce::dsp::FastMathApproximations::cos (currentAngle);
}

float LineReader::getY1() const
{
    const float lfoValAngle = lastLfoValues[lfoAngleSelect.load() ? 1 : 0].load();
    const float currentAngle = angle.load() * (1.0f + 2.0f * (lfoAngleAmount.load() - 0.5f) * (lfoValAngle - 0.5f));
    const float lfoValLength = lastLfoValues[lfoLengthSelect.load() ? 1 : 0].load();
    const float currentLength = length.load() * (1.0f + 2.0f * (lfoLengthAmount.load() - 0.5f) * (lfoValLength - 0.5f));
    const float halfLength = currentLength * 0.5f;
    return cy.load() - halfLength * juce::dsp::FastMathApproximations::sin (currentAngle);
}

float LineReader::getX2() const
{
    const float lfoValAngle = lastLfoValues[lfoAngleSelect.load() ? 1 : 0].load();
    const float currentAngle = angle.load() * (1.0f + 2.0f * (lfoAngleAmount.load() - 0.5f) * (lfoValAngle - 0.5f));
    const float lfoValLength = lastLfoValues[lfoLengthSelect.load() ? 1 : 0].load();
    const float currentLength = length.load() * (1.0f + 2.0f * (lfoLengthAmount.load() - 0.5f) * (lfoValLength - 0.5f));
    const float halfLength = currentLength * 0.5f;
    return cx.load() + halfLength * juce::dsp::FastMathApproximations::cos (currentAngle);
}

float LineReader::getY2() const
{
    const float lfoValAngle = lastLfoValues[lfoAngleSelect.load() ? 1 : 0].load();
    const float currentAngle = angle.load() * (1.0f + 2.0f * (lfoAngleAmount.load() - 0.5f) * (lfoValAngle - 0.5f));
    const float lfoValLength = lastLfoValues[lfoLengthSelect.load() ? 1 : 0].load();
    const float currentLength = length.load() * (1.0f + 2.0f * (lfoLengthAmount.load() - 0.5f) * (lfoValLength - 0.5f));
    const float halfLength = currentLength * 0.5f;
    return cy.load() + halfLength * juce::dsp::FastMathApproximations::sin (currentAngle);
}

void LineReader::processBlock (const juce::Image& imageToRead, juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
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

    const float phaseIncrement = frequency / (float) sampleRate;
    const float phaseIncrementLow = phaseIncrement * 0.5f;
    const float phaseIncrementHigh = phaseIncrement * 2.0f;

    const float imageWidth = (float) (bitmapData.width - 1);
    const float imageHeight = (float) (bitmapData.height - 1);
    const int numChannels = buffer.getNumChannels();

    for (int sample = startSample; sample < startSample + numSamples; ++sample)
    {
        float lfoValues[2] = { 0.0f, 0.0f };
        if (lfos[0] != nullptr)
            lfoValues[0] = lfos[0]->process();
        if (lfos[1] != nullptr)
            lfoValues[1] = lfos[1]->process();

        const float cx_sv = cxs.getNextValue();
        const float cy_sv = cys.getNextValue();

        const float lfoValLength = lfoValues[lfoLengthSelect.load() ? 1 : 0];
        float length_sv = lengths.getNextValue();
        length_sv *= (1.0f + 2.0f * (lfoLengthAmount.load() - 0.5f) * (lfoValLength - 0.5f));

        const float lfoValAngle = lfoValues[lfoAngleSelect.load() ? 1 : 0];
        float angle_sv = angles.getNextValue();
        angle_sv *= (1.0f + 2.0f * (lfoAngleAmount.load() - 0.5f) * (lfoValAngle - 0.5f));

        if (sample == startSample + numSamples - 1)
        {
            lastLfoValues[0] = lfoValues[0];
            lastLfoValues[1] = lfoValues[1];
        }

        const float halfLength = length_sv * 0.5f;
        const float dx_calc = halfLength * juce::dsp::FastMathApproximations::cos (angle_sv);
        const float dy_calc = halfLength * juce::dsp::FastMathApproximations::sin (angle_sv);

        const float x1sv = cx_sv - dx_calc;
        const float y1sv = cy_sv - dy_calc;
        const float x2sv = cx_sv + dx_calc;
        const float y2sv = cy_sv + dy_calc;

        // --- Amplitudes from length ---
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

        finalSampleValue *= volumeSmoother.getNextValue();

        for (int channel = 0; channel < numChannels; ++channel)
            buffer.setSample (channel, sample, finalSampleValue);

        phase += phaseIncrement;
        phase = std::fmod (phase, 1.0f);

        phaseLow += phaseIncrementLow;
        phaseLow = std::fmod (phaseLow, 1.0f);

        phaseHigh += phaseIncrementHigh;
        phaseHigh = std::fmod (phaseHigh, 1.0f);
    }
}