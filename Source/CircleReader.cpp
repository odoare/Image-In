/*
  ==============================================================================

    CircleReader.cpp
    Created: 13 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "CircleReader.h"
#include "LFO.h"

CircleReader::CircleReader() {}
CircleReader::~CircleReader() {}

void CircleReader::prepareToPlay (double sr)
{
    sampleRate = sr;
    const double rampTimeSeconds = 0.05;
    cxs.reset (sampleRate, rampTimeSeconds); cxs.setCurrentAndTargetValue (cx.load());
    cys.reset (sampleRate, rampTimeSeconds); cys.setCurrentAndTargetValue (cy.load());
    rs.reset (sampleRate, rampTimeSeconds); rs.setCurrentAndTargetValue (radius.load());
    volumeSmoother.reset (sampleRate, rampTimeSeconds);
    volumeSmoother.setCurrentAndTargetValue (volume);
    // Trigonometric values are synced at the start of each processBlock.
}

void CircleReader::setCentre (float newCx, float newCy)
{
    if (newCx == cx.load() && newCy == cy.load())
        return;

    cx = juce::jlimit (0.0f, 1.0f, newCx);
    cy = juce::jlimit (0.0f, 1.0f, newCy);

    cxs.setTargetValue (cx.load());
    cys.setTargetValue (cy.load());
}

void CircleReader::setRadius (float newRadius)
{
    if (newRadius == radius.load())
        return;

    radius = juce::jlimit (0.0f, 0.5f, newRadius);
    rs.setTargetValue (radius.load());
}

float CircleReader::getCX() const
{
    const float lfoVal = lastLfoValues[lfoCxSelect.load() ? 1 : 0].load();
    return cx.load() * (1.0f + 2.0f * (lfoCxAmount.load() - 0.5f) * (lfoVal - 0.5f));
}

float CircleReader::getRadius() const
{
    const float lfoVal = lastLfoValues[lfoRadiusSelect.load() ? 1 : 0].load();
    return radius.load() * (1.0f + 2.0f * (lfoRadiusAmount.load() - 0.5f) * (lfoVal - 0.5f));
}

void CircleReader::processBlock (const juce::Image& imageToRead, juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
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

    const auto twoPi = juce::MathConstants<float>::twoPi;

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

        const float lfoValCx = lfoValues[lfoCxSelect.load() ? 1 : 0];
        float cx_sv = cxs.getNextValue();
        cx_sv *= (1.0f + 2.0f * (lfoCxAmount.load() - 0.5f) * (lfoValCx - 0.5f));

        const float cy_sv = cys.getNextValue();

        const float lfoValRadius = lfoValues[lfoRadiusSelect.load() ? 1 : 0];
        float r_sv = rs.getNextValue();
        r_sv *= (1.0f + 2.0f * (lfoRadiusAmount.load() - 0.5f) * (lfoValRadius - 0.5f));

        if (sample == startSample + numSamples - 1)
        {
            lastLfoValues[0] = lfoValues[0];
            lastLfoValues[1] = lfoValues[1];
        }

        // --- Amplitudes from radius ---
        const float normalizedLength = r_sv * 2.0f; // Map radius [0, 0.5] to [0, 1] for amplitude calculation

        const float ampHigh = juce::jmax (0.0f, 1.0f - normalizedLength * 2.0f);
        const float ampBase = 1.0f - std::abs (normalizedLength - 0.5f) * 2.0f;
        const float ampLow  = juce::jmax (0.0f, (normalizedLength - 0.5f) * 2.0f);

        float finalSampleValue = 0.0f;

        auto getSampleAtPhase = [&] (float currentPhase) -> float
        {
            const float angle = currentPhase * twoPi;
            const float c = juce::dsp::FastMathApproximations::cos (angle);
            const float s = juce::dsp::FastMathApproximations::sin (angle);

            const float currentX = cx_sv + r_sv * c;
            const float currentY = cy_sv + r_sv * s;

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
            buffer.addSample (channel, sample, finalSampleValue);

        // Keep phase variables updated for state saving and re-syncing
        phase += phaseIncrement;
        phase = std::fmod (phase, 1.0f);

        phaseLow += phaseIncrementLow;
        phaseLow = std::fmod (phaseLow, 1.0f);

        phaseHigh += phaseIncrementHigh;
        phaseHigh = std::fmod (phaseHigh, 1.0f);
    }
}